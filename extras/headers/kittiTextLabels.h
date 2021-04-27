#ifndef KITTITEXTLABELS_H
#define KITTITEXTLABELS_H

#include <QDir>
#include <QFile>
#include <QDebug>
#include <QObject>
#include <QApplication>

#include "headers/definitions.h"

class kittiTextLabels : public QObject
{
  Q_OBJECT
public:
  //this is to obtain Laser clouds information from KittiData,
  //use only the parent folder name where the velodyne_points folder is stored
  //you must follow this folder structure:

  //  ∟– KittiData
  //      ∟– raw
  //          ∟– 00##_sync
  //              ∟– _folder
  //                  ∟– 000000####.bin
  //  ∟– yourExecutableFolder

  kittiTextLabels(const QString _folder);
  ~kittiTextLabels();

  //returns the vector with all the labels
  const vector<XYZLabel> *const textLabels();

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
  vector<XYZLabel> labels;
  QString folderPath, folder;
  unsigned int frame, total_frames;
};

#endif // KITTITEXTLABELS_H
