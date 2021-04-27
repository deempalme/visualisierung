#ifndef MAPS_H
#define MAPS_H

#include <QtSql>
#include <QXmlStreamReader>
#include <QDir>
#include <vector>
#include <QThread>

using namespace std;

#ifndef MAPS_S
#define MAPS_S
struct Signals{
  double latitude;  //contains the latitude position in meters (origin : latitude where you loaded the map)
  double longitude; //contains the longitude position in meters (origin : longitude where you loaded the map)
  float type;       //describes the type of sign (in German code, example: 274 is equal to "speed limit" sign)
  float value;      //in case the sign has a value this will contain it ("speed limit" sign value is the max speed)
};

struct Nodes{
  double latitude;  //contains the latitude position in meters (origin : latitude where you loaded the map)
  double longitude; //contains the longitude position in meters (origin : longitude where you loaded the map)
  int type;         //describes the type of street: service, tertiary, primary, etc.
};
#endif

class LoadMaps : public QThread
{
  Q_OBJECT
public:

  LoadMaps(QObject *parent = 0);
  ~LoadMaps();
  //this will load the nodes and their tags:
  //_dLt must containt the value in meters of the Latitude coordinate
  //if _dLt = 0 then this will call automatically transformCoordinatesToMeters() to convert the Latitude into meters
  //_dLn must containt the value in meters of the Longitude coordinate :
  //if _dLn = 0 then this will call automatically transformCoordinatesToMeters() to convert the Longitude into meters
  //_width is the width of the map range, _length is the length of the map range
  //returns true if streets found
  void loadMap(const double _latitude = 1000, const double _longitude = 1000,
               const double _dLt = 0, const double _dLn = 0,
               const unsigned int _width = 150, const unsigned int _length = 150);
  //this will return the folder path, that is the folder where you must put the .osm files to install
  QString getFolderPath();
  //this will transform the latitude coordinate into meters (represents the distance in meters for this coordinate's degree)
  double transformLatitudeToMeters(const double _latitude);
  //this will transform the longitude coordinate into meters (represents the distance in meters for this coordinate's degree)
  //note that the input still _latitude
  double transformLongitudeToMeters(const double _latitude);

  //contains the list of all the nodes found inside the square of dimensions (_width, _length). They are separated by street segments
  vector<vector<Nodes> > streetList;
  //contains the list of all the tags for each node found inside the square of dimensions (_width, _length)
  //the size of streetTags is equal to streetList and streetTags[i] is the correspondient tag string for streetList[i]
  //example: "heading=288+highway=priority_road+traffic_sign=DE_306_PRIORITY_ROAD+uuid=306103"
  vector<QString> streetTags;
  //contains the list of all the signs found inside the square of dimensions (_width, _length)
  //see Signals struct for more details
  vector<Signals> signsList;
  void stopMapLoading();

  bool occupied;

protected:
  void run() Q_DECL_OVERRIDE;

private:
  void sError(QString txt, int line);

  bool abort;
  double dLt, dLn, pLtM, pLnM, rLt, rLn, rLt1, rLt2, rLn1, rLn2;

signals:
  void MapLoaded();
};

#endif // MAPS_H
