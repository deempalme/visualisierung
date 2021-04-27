#ifndef OBTAINOBSTACLES_H
#define OBTAINOBSTACLES_H

#include <QtXml>
#include <QString>
#include <QQuaternion>
#include <QMatrix4x4>
#include <QVector3D>
#include <QVector2D>
#include <QDebug>
#include <vector>
#include <QObject>
#include <QFile>
#include <QApplication>

#include "headers/definitions.h"
#include "headers/coordinateconversor.h"

struct Obstacles{
  QVector3D dimension;
  QVector<QVector3D> position;
  QVector<QVector3D> orientation;
  QVector2D frame;
  QVector3D lastPosition;
  double softSpeed = 0;
  unsigned int type;
};

class kittiObstacles : public QObject
{
  Q_OBJECT
public:
  //this is to obtain tracklets information from KittiData,
  //use only the parent folder name where the tracklet_labels.xml file is stored
  //you must follow this folder structure:

  //  ∟– KittiData
  //      ∟– raw
  //          ∟– 0001_sync
  //              ∟– tracklet_labels.xml
  //  ∟– yourExecutableFolder

  kittiObstacles(CoordinateConversor *_conversor = nullptr, bool _doLabeling = true,
                 double *_latitude = nullptr, double *_longitude = nullptr, double *_altitude = nullptr,
                 double *_pitch = nullptr, double *_yaw = nullptr, double *_roll = nullptr);
  ~kittiObstacles();

  //returns the actual frame position
  unsigned int actualFrame();
  //returns the number of obstacles in the actual frame
  unsigned int totalObstacles();
  //if you want to make transformation matrix to this objects you need
  //to define a transformation matrix. This will overwrite the existing transformation matrix
  void setTransformationMatrix(QMatrix4x4 _matrix);
  //this will translate the transformation matrix
  void translateObstacles(const float _x = 0, const float _y = 0, const float _z = 0);
  //this will rotate the transformation matrix using euler angles (degrees)
  void rotateObstaclesFromEuler(const float _pitch = 0, const float _yaw = 0, const float _roll = 0);
  //this will rotate the transformation matrix using angle and axis (degrees)
  void rotateObstaclesFromAxis(const float _angle_x = 0, const float _angle_y = 0, const float _angle_z = 0);
  //this will scale the transformation
  void scaleData(const float _scale_x = 1, const float _scale_y = 1, const float _scale_z = 1);

  //this vector contains all the obstacles found in the actual frame
  const vector<Obstacle> *const getObstacles();
  const QMatrix4x4 *const getTransformationMatrix();

public slots:
  //set a new kitti Dataset,
  //returns false if the folder was not found
  //if you set datasetNumber = 1; then the complete folder name will be 0001_sync
  //note that dates from the folder name were removed, example: "2011_09_26_0001_sync" --> "0001_sync"
  bool setDataSet(const unsigned int _datasetNumber = 1);
  //set the frame in a specific frame number,
  //returns false if there are no obstacles found
  bool gotoFrame(const unsigned int _frameNumber = 0);

private:
  vector<Obstacle> obstacles;
  QMatrix4x4 transformationMatrix;

  QString folderPath;
  unsigned int frame, dataset;
  //this vector contains all the obstacles found in all frames
  QVector<Obstacles> allObstacles;

  CoordinateConversor *conversor;
  double *latitude, *longitude, *altitude, *pitch, *yaw, *roll, previousLatitude, previousLongitude, previousAltitude, distance;
  QVector3D oldPositionObject, oldPositionCar;
  double softSpeedCar, softSpeedObject;

  bool labelingActive;
};

#endif // OBTAINDATAIMU_H
