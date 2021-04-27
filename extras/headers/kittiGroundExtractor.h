#ifndef GROUNDEXTRACTION_H
#define GROUNDEXTRACTION_H

#include <QDir>
#include <QFile>
#include <QString>
#include <QDebug>
#include <vector>
#include <QObject>
#include <QApplication>

#include "headers/definitions.h"

class KittiGroundExtractor : public QObject
{
  Q_OBJECT
public:
  //this is to obtain Laser clouds information from KittiData,
  //use only the parent folder name where the velodyne_points folder is stored
  //you must follow this folder structure:

  //  ∟– KittiData
  //      ∟– raw
  //          ∟– 0001_sync
  //              ∟– _folder
  //  ∟– yourExecutableFolder

  KittiGroundExtractor(const QString _folder);
  ~KittiGroundExtractor();

  //returns the actual frame position
  unsigned int actualFrame();
  //returns the number of total frames in this folder
  unsigned int totalFrames();

  const vector<Boxes> *const GroundData();

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
  vector<Boxes> groundData;
  QString folderPath, folder;
  unsigned int frame, total_frames;
};

#endif // GROUNDEXTRACTION_H
