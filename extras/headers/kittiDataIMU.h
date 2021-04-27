#ifndef OBTAINDATAIMU_H
#define OBTAINDATAIMU_H

#include <QDir>
#include <QFile>
#include <QString>
#include <QDebug>
#include <vector>
#include <QtMath>
#include <QObject>
#include <QMatrix4x4>
#include <QApplication>

#include "headers/definitions.h"
#include "headers/coordinateconversor.h"

class kittiDataIMU : public QObject
{
  Q_OBJECT
public:
  //this is to obtain IMU information from KittiData,
  //use only the parent folder name where the oxt folder is stored
  //you must follow this folder structure:

  //  ∟– KittiData
  //      ∟– raw
  //          ∟– 0001_sync
  //              ∟– oxt
  //                  ∟– data
  //  ∟– yourExecutableFolder

  kittiDataIMU();

  //returns the actual frame position
  unsigned int actualFrame();
  //returns the number of total frames in this folder
  unsigned int totalFrames();
  //this will set the Static frame origin
  void setStaticFrameOrigin(const double _latitude, const double _longitude, const double _altitude,
                            const double _pitch = 0, const double _yaw = 0, const double _roll = 0);
  void setConversor(CoordinateConversor *_conversor);

  VehicleParameters vehicle;
  double frameLatitude, frameLongitude, frameAltitude;
  double framePitch, frameYaw, frameRoll;

public slots:
  //set a new kitti Dataset,
  //returns false if the folder was not found
  //if you set datasetNumber = 1; then the complete folder name will be 0001_sync
  //note that dates from the folder name were removed, example: "2011_09_26_0001_sync" --> "0001_sync"
  bool setDataSet(const unsigned int _datasetNumber = 1);
  //set the frame in a specific frame number,
  //returns false if the frame number is bigger than existing frames
  //or if an error occurs (see application output to see messages)
  bool gotoFrame(const unsigned int _frameNumber = 0);

private:
  QString folderPath;
  unsigned int frame, total_frames;
  bool isFrameSet;

  CoordinateConversor *conversor;
};

#endif // OBTAINDATAIMU_H
