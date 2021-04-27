#include "extras/headers/kittiObstacles.h"

kittiObstacles::kittiObstacles(CoordinateConversor *_conversor, bool _doLabeling,
                               double *_latitude, double *_longitude, double *_altitude,
                               double *_pitch, double *_yaw, double *_roll) :
  frame(0),
  folderPath(),
  conversor(_conversor),
  latitude(_latitude),
  longitude(_longitude),
  altitude(_altitude),
  pitch(_pitch),
  yaw(_yaw),
  roll(_roll),
  previousLatitude(0),
  previousLongitude(0),
  previousAltitude(0),
  distance(0),
  oldPositionObject(0, 0, 0),
  oldPositionCar(0, 0, 0),
  softSpeedCar(0),
  softSpeedObject(0),
  labelingActive(_doLabeling)
{
}

bool kittiObstacles::setDataSet(const unsigned int _datasetNumber){
  bool result = false;

  dataset = _datasetNumber;
  folderPath = qApp->applicationDirPath() + "/" + QString("../KittiData/raw/%1_sync/tracklet_labels.xml").arg(QString::number(dataset).rightJustified(4, '0'));

  //cleaning all the data of previous obstacles
  frame = 0;
  obstacles.clear();
  allObstacles.clear();

  //Opening XML file which contains all info of movable objects
  QDomDocument movables;
  QFile fXML(folderPath);
  //checks if the file exists
  if(fXML.open(QIODevice::ReadOnly | QIODevice::Text)){
    //check if is possible to read the document as a XML file
    if(!movables.setContent(&fXML)){
      qWarning() << "Failed read tracklets document: " << folderPath;
      return false;
    }

    QDomNode item, poses, item2;
    int fi, ff, tt = 6;
    QString tp;
    item = movables.firstChildElement().firstChild().firstChildElement("item");

    //read size, position, angles, frames and type of object for each detected movable object
    while(item.isElement()){
      Obstacles tempItem;
      //traveling through the xml file NODES
      poses = item.firstChildElement("poses");
      //first frame appearing
      fi = item.firstChildElement("first_frame").toElement().text().toInt();
      //last frame appearing
      ff = fi + poses.firstChildElement().text().toInt();
      item2 = poses.firstChildElement("item");
      //tracklet type
      tp = item.firstChildElement().text();
      QDomNodeList items = poses.childNodes();

      tempItem.dimension = QVector3D(item.firstChildElement("h").toElement().text().toFloat(),
                                     item.firstChildElement("w").toElement().text().toFloat(),
                                     item.firstChildElement("l").toElement().text().toFloat());

      //selects the type from the tracklets file, the color will be selected depending on the type
      if(tp == "Car"){
        tt = 0;
      }else if(tp == "Pedestrian"){
        tt = 1;
      }else if(tp == "Cyclist"){
        tt = 2;
      }else if(tp == "Van"){
        tt = 3;
      }else if(tp == "Truck"){
        tt = 4;
      }else if(tp == "Misc"){
        tt = 5;
      }
      tempItem.type = tt;
      //this determinates in which frame will this tracklet appear
      //fi = initial frame
      //ff = final frame
      tempItem.frame = QVector2D(fi, ff);

      for(int i = 2; i < items.size(); i++){
        item2 = items.at(i);
        //places the position, remember the coordinate system of OpenGl es different from normal coordinate systems
        tempItem.position.append(QVector3D(item2.firstChildElement("tx").toElement().text().toFloat(),
                                           item2.firstChildElement("ty").toElement().text().toFloat(),
                                           item2.firstChildElement("tz").toElement().text().toFloat()));
        //obtain the euler angles from the xml file and multiply for 57.29577 to convert from radians to degrees
        tempItem.orientation.append(QVector3D(item2.firstChildElement("rx").toElement().text().toFloat(),
                                              item2.firstChildElement("ry").toElement().text().toFloat(),
                                              item2.firstChildElement("rz").toElement().text().toFloat()));
      }
      allObstacles.append(tempItem);
      item = item.nextSibling();
    }

    fXML.close();
    result = true;
  }else
    qWarning() << "Failed to open file: " << folderPath;

  softSpeedCar = 0;
  softSpeedObject = 0;
  oldPositionCar = QVector3D();
  oldPositionObject = QVector3D();
  return result;
}

bool kittiObstacles::gotoFrame(const unsigned int _frameNumber){
  bool result = false;

  //clear all the laser cloud information existing
  obstacles.clear();
  //creates temporal variables out of the loop to avoid creating them in each tracklet
  Obstacle temporal;
  //temporal for color
  QVector3D mC;
  //temporal for line width
  float lW;
  unsigned int taframe;

  QVector3D newPositionCar, distanceCar, distanceObject;
  QVector2D gps;

  QFile labeling(qApp->applicationDirPath() + "/" + QString("../KittiData/raw/%1_sync/labels/%2.txt").arg(QString::number(dataset).rightJustified(4, '0')).arg(QString::number(_frameNumber).rightJustified(10, '0')));
  bool exists = labeling.exists();

  if(!exists && labelingActive){
    newPositionCar = QVector3D(-gps[1], *altitude, gps[0]);
    distanceCar = newPositionCar - oldPositionCar;
    oldPositionCar = newPositionCar;
    gps = conversor->GPStoXY(*latitude, *longitude);

    labeling.open(QIODevice::WriteOnly);
  }

  QTextStream stream(&labeling);
  QString what;

  //go through all the obstacles and choose the ones that appear in this frame
  for(int i = 0; i < allObstacles.size(); i++){
    //checks if this tracklet appears in this frame
    if((_frameNumber >= allObstacles[i].frame[0]) && (_frameNumber < allObstacles[i].frame[1])){
      taframe = _frameNumber - allObstacles[i].frame[0];

      switch(allObstacles[i].type){
      case 0:
        mC = QVector3D(5, 164, 217);
        lW = 0.10;
        what = "A";
      break;
      case 1:
        mC = QVector3D(255, 13, 0);
        lW = 0.07;
        what = "P";
      break;
      case 2:
        mC = QVector3D(255, 122, 0);
        lW = 0.07;
        what = "C";
      break;
      case 3:
        mC = QVector3D(36, 17, 224);
        lW = 0.13;
        what = "V";
      break;
      case 4:
        mC = QVector3D(225, 0, 189);
        lW = 0.15;
        what = "T";
      break;
      case 5:
        mC = QVector3D(139, 246, 0);
        lW = 0.10;
        what = "M";
      break;
      default:
        mC = QVector3D(170, 203, 207);
        lW = 0.10;
        what = "U";
      break;
      }

      what += QString::number(i);

      //the position changes every frame, so we need to go to the selected frame and colect its position
      temporal.positionX = allObstacles[i].position[taframe][0];
      temporal.positionY = allObstacles[i].position[taframe][1];
      temporal.positionZ = allObstacles[i].position[taframe][2];

      temporal.width = allObstacles[i].dimension[1];
      temporal.length = allObstacles[i].dimension[2];
      temporal.height = allObstacles[i].dimension[0];

      //the orientation changes every frame, so we need to go to the selected frame and colect its orientation
      temporal.pitch = allObstacles[i].orientation[taframe][1];
      temporal.yaw = allObstacles[i].orientation[taframe][2];
      temporal.roll = allObstacles[i].orientation[taframe][0];

      temporal.arrow_pitch = temporal.pitch;
      temporal.arrow_yaw = temporal.yaw;
      temporal.arrow_roll = temporal.roll;

      temporal.r = mC[0];
      temporal.g = mC[1];
      temporal.b = mC[2];

      temporal.lineWidth = lW;
      //adding this obstacle to the vector
      obstacles.push_back(temporal);

      if(!exists && labelingActive){
        QMatrix4x4 rotation;
        rotation.rotate(QQuaternion::fromEulerAngles(-*pitch * toDEGREES, -90 - *yaw * toDEGREES, -*roll * toDEGREES));
        QVector3D newPosition(temporal.positionX, temporal.positionZ, temporal.positionY);
        newPosition = rotation * newPosition;
        newPosition += oldPositionCar;
        distanceObject = newPosition - allObstacles[i].lastPosition;

        if(taframe == 0){
          distanceObject = QVector3D();
          allObstacles[i].softSpeed = 0;
        }

        allObstacles[i].lastPosition = newPosition;
        double odistance(distanceObject.length());

        //speed softener, it takes 0.5 seconds to change a value from initial value to final value
        //if you have a timer delay of 100 miliseconds
        allObstacles[i].softSpeed += (odistance/0.1 * toKMPH - allObstacles[i].softSpeed)/5.0f;
        what = QString("%1 Km/h").arg(allObstacles[i].softSpeed, 1, 'g', 4, '0');

        stream << QString("%1~%2~%3~%4~%5~%6~%7").arg(temporal.positionX).arg(temporal.positionY)
                  .arg(temporal.positionZ + temporal.height + 0.3).arg(temporal.r).arg(temporal.g)
                  .arg(temporal.b).arg(what) << endl;
      }
    }
  }

//  Obstacle temporal2;
//  temporal2.positionX = 0.1;
//  temporal2.positionY = 1.5;
//  temporal2.positionZ = -1;
//  temporal2.r = 5;
//  temporal2.g = 164;
//  temporal2.b = 217;
//  temporal2.arrow = false;
//  obstacles.push_back(temporal2);

//  obstacles.clear();

//  uint i = 0;
//  for(i = 0; i < 10000; i++){
//    obstacles.push_back(temporal);
//  }


  if(!exists && labelingActive)
    labeling.close();

  if(obstacles.size() > 0)
    result = true;

  return result;
}

unsigned int kittiObstacles::actualFrame(){
  return frame;
}

unsigned int kittiObstacles::totalObstacles(){
  return obstacles.size();
}

void kittiObstacles::setTransformationMatrix(QMatrix4x4 _matrix){
  transformationMatrix = _matrix;
}

void kittiObstacles::translateObstacles(const float _x, const float _y, const float _z){
  transformationMatrix.translate(_x, _y, _z);
}

void kittiObstacles::rotateObstaclesFromEuler(const float _pitch, const float _yaw, const float _roll){
  QQuaternion temporalQuaternion;
  temporalQuaternion.fromEulerAngles(_pitch, _yaw, _roll);
  transformationMatrix.rotate(temporalQuaternion);
}

void kittiObstacles::rotateObstaclesFromAxis(const float _angle_x, const float _angle_y, const float _angle_z){
  QQuaternion temporalQuaternion;
  temporalQuaternion = QQuaternion::fromAxisAndAngle(1, 0, 0, _angle_x)
      * QQuaternion::fromAxisAndAngle(0, 1, 0, _angle_y)
      * QQuaternion::fromAxisAndAngle(0, 0, 1, _angle_z);
  transformationMatrix.rotate(temporalQuaternion);
}

void kittiObstacles::scaleData(const float _scale_x, const float _scale_y, const float _scale_z){
  transformationMatrix.scale(_scale_x, _scale_y, _scale_z);
}

const vector<Obstacle> *const kittiObstacles::getObstacles(){
  return &obstacles;
}

const QMatrix4x4 *const kittiObstacles::getTransformationMatrix(){
  return &transformationMatrix;
}

kittiObstacles::~kittiObstacles(){
  obstacles.clear();
  allObstacles.clear();
}
