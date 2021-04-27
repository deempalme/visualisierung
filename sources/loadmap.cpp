#include "headers/parameters.h"
#include "headers/loadmap.h"

LoadMaps::LoadMaps(QObject *parent) :
  QThread(parent),
  occupied(false),
  abort(false)
{
}

LoadMaps::~LoadMaps(){
  abort = true;
  wait();
}

void LoadMaps::sError(QString txt, int line){
  qWarning() << txt << " - AT LINE:" << line;
}

QString LoadMaps::getFolderPath(){
  return E_MAPSFOLDER;
}

double LoadMaps::transformLatitudeToMeters(const double _latitude){
  //obtain the distance per grade (°) of the current GPS coordinates
  //converting degrees to radians:
  //iLt = (interger latitude) converted into radians because:
  //qCos(radians)
  //for more info see:
  //  https://en.wikipedia.org/wiki/Geographic_coordinate_system#Expressing_latitude_and_longitude_as_linear_units
  float iLt = qRound(_latitude) * 0.01745329252;
  //distance in meters of latitude:
  double dLt = 111132.92 - 559.82 * qCos(2 * iLt) + 1.175 * qCos(4 * iLt) - 0.0023 * qCos(6 * iLt);

  return dLt;
}

double LoadMaps::transformLongitudeToMeters(const double _latitude){
  //obtain the distance per grade (°) of the current GPS coordinates
  //converting degrees to radians:
  //iLt = (interger latitude) converted into radians because:
  //qCos(radians)
  //for more info see:
  //  https://en.wikipedia.org/wiki/Geographic_coordinate_system#Expressing_latitude_and_longitude_as_linear_units
  float iLt = qRound(_latitude) * 0.01745329252;
  //distance in meters of longitude:
  double dLn = 111412.84 * qCos(iLt) - 93.5 * qCos(3 * iLt) - 0.118 * qCos(5 * iLt);

  return dLn;
}

void LoadMaps::stopMapLoading(){
  abort = true;
  wait();
}

void LoadMaps::run(){
  occupied = true;

  streetList.clear();
  streetTags.clear();
  signsList.clear();

  Nodes temp2Dpoint;
  QString wtags;
  QStringList tags, tag;
  int street = 0;
  QSqlQuery ways, signs, points, nodes;

  ways.prepare("SELECT ways.ref, ways.tags FROM gps_map.ways WHERE ways.ref "
               "IN (SELECT DISTINCT nds.way FROM gps_map.nds WHERE nds.ref "
               "IN (SELECT nodes.ref FROM gps_map.nodes WHERE (nodes.lat "
               "BETWEEN :lt1 AND :lt2) AND (nodes.lon BETWEEN :ln1 AND :ln2))) "
               "AND ways.tags LIKE '%highway%'");
  ways.bindValue(":lt1", rLt1);
  ways.bindValue(":lt2", rLt2);
  ways.bindValue(":ln1", rLn1);
  ways.bindValue(":ln2", rLn2);
  if(!ways.exec()){
    sError(ways.lastError().text(), __LINE__);
  }else{
    while(ways.next()){
      wtags = ways.value(1).toString();
      tags = wtags.split("+", QString::SkipEmptyParts);
      for(int i = 0; i < tags.size(); i++){
        tag = tags.value(i).split("=");
        if(tag.value(0) == "highway"){
          if(tag.value(1) == "service")
            street = 0;
          else if(tag.value(1) == "tertiary")
            street = 1;
          else if(tag.value(1) == "unclassified")
            street = 2;
          else if(tag.value(1) == "primary")
            street = 3;
          else if(tag.value(1) == "residential")
            street = 4;
          else if(tag.value(1) == "turning_circle")
            street = 5;
          else
            street = 6;
        }
      }
      if(street < 6){
        nodes.prepare("SELECT ref, pway FROM gps_map.nds WHERE way=:way ORDER BY pway ASC");
        nodes.bindValue(":way", ways.value(0));
        if(!nodes.exec()){
          sError(nodes.lastError().text(), __LINE__);
        }else{
          vector<Nodes> tempPoint2;
          while(nodes.next()){
            points.prepare("SELECT lat, lon FROM gps_map.nodes WHERE ref=:ref");
            points.bindValue(":ref", nodes.value(0));
            if(!points.exec()){
              sError(points.lastError().text(), __LINE__);
            }else{
              points.next();
              temp2Dpoint.latitude = points.value(0).toDouble() * dLt - pLtM;
              temp2Dpoint.longitude = points.value(1).toDouble() * dLn - pLnM;
              temp2Dpoint.type = street;
              tempPoint2.push_back(temp2Dpoint);
            }
          }
          if(nodes.size() > 0){
            streetList.push_back(tempPoint2);
            streetTags.push_back(wtags);
          }
          tempPoint2.clear();
        }
      }
      if(abort){
        occupied = false;
        return;
      }
    }
  }

  Signals sign;

  signs.prepare("SELECT nodes.lat, nodes.lon, nodes.tags FROM gps_map.nodes WHERE (nodes.lat BETWEEN :lt1 AND :lt2) "
                "AND (nodes.lon BETWEEN :ln1 AND :ln2) AND (nodes.tags LIKE '%traffic_sign%') AND NOT (nodes.tags LIKE '%highway%')");
  signs.bindValue(":lt1", rLt1);
  signs.bindValue(":lt2", rLt2);
  signs.bindValue(":ln1", rLn1);
  signs.bindValue(":ln2", rLn2);

  if(!signs.exec())
    sError(signs.lastError().text(), __LINE__);
  else{
    while(signs.next()){
      sign.latitude = signs.value(0).toDouble() * dLt - pLtM;
      sign.longitude = signs.value(1).toDouble() * dLn - pLnM;
      sign.value = 0;
      sign.type = 0;
      tags = signs.value(2).toString().split("+", QString::SkipEmptyParts);
      for(int i = 0; i < tags.size(); i++){
        tag = tags.value(i).split("=");
        if(tag.value(0) == "traffic_sign"){
          if((tag.value(1) == "DE_267_ONEWAY_NO_ENTRY") || (tag.value(1) == "DE:267"))
            sign.type = 267;
          else if((tag.value(1) == "DE_205_GIVE_WAY") || (tag.value(1) == "DE:205"))
            sign.type = 205;
          else if((tag.value(1) == "DE_306_PRIORITY_ROAD") || (tag.value(1) == "DE:306"))
            sign.type = 306;
          else if((tag.value(1) == "DE_215_ROUNDABOUT") || (tag.value(1) == "DE:215"))
            sign.type = 215;
          else if((tag.value(1) == "DE_350_PEDESTRIAN_CROSSING") || (tag.value(1) == "DE:350"))
            sign.type = 350;
          else if((tag.value(1) == "DE_220_ONEWAY") || (tag.value(1) == "DE:220"))
            sign.type = 220;
          else if((tag.value(1) == "DE_108_INCLINE") || (tag.value(1) == "DE:108"))
            sign.type = 108;
          else if((tag.value(1) == "DE_274_SPEED_LIMIT") || (tag.value(1) == "DE:274"))
            sign.type = 274;
        }else if(tag.value(0) == "maxspeed"){
          sign.value = tag.value(1).toDouble();
        }else if(tag.value(0) == "incline"){
          sign.value = tag.value(1).toDouble();
        }else if(tag.value(0) == "traffic_signals:direction"){
          sign.type = 2000;
        }
      }
      signsList.push_back(sign);
      if(abort){
        occupied = false;
        return;
      }
    }
  }
  occupied = false;
  emit MapLoaded();
}

void LoadMaps::loadMap(const double _latitude, const double _longitude, const double _dLt,
                       const double _dLn, const unsigned int _width, const unsigned int _length){
  //check if latitude and longitude have decent values
  if((_latitude < 1000) && (_longitude < 1000)){
    stopMapLoading();

    dLt = (_dLt == 0)? transformLatitudeToMeters(_latitude) : _dLt;
    dLn = (_dLn == 0)? transformLongitudeToMeters(_latitude) : _dLn;

    //obtains distance in meters from the equator
    pLtM = _latitude * dLt;
    //from the Greenwich meridian
    pLnM = _longitude * dLn;

    //this will obtain the range in GPS coordinates
    rLt = (_width/2.0f)/dLt;
    rLn = (_length/2.0f)/dLn;
    rLt1 = _latitude - rLt;
    rLt2 = _latitude + rLt;
    rLn1 = _longitude - rLn;
    rLn2 = _longitude + rLn;

    abort = false;
    start();
  }
}
