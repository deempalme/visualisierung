#ifndef OBTAINFROMFILE_H
#define OBTAINFROMFILE_H

#include <QObject>
#include <QApplication>

#include "extras/headers/kittiObstacles.h"

// Loads Custom Obstacles from KittiData
class kittiCustomObstacleLoader : public QObject
{
  Q_OBJECT

public:

  // KittiFolderPath points to directory where "raw"-folder is located
  kittiCustomObstacleLoader(const QString& KittiFolderPath);

  void LoadFromFile(const int datasetId, const int frameId);

  const std::vector<Obstacle>* GetObstacles() const;
  const std::vector<Obstacle>* GetCylinders() const;
  const std::vector<Obstacle>* GetObstaclesFixed() const;
  const std::vector<Obstacle>* GetCylindersFixed() const;

  static Obstacle ConvertVectorToObstacle(const QVector<float> &obstacleVec);

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
  QString folderPath;
  std::vector<Obstacle> obstacles;
  std::vector<Obstacle> cylinders;
  std::vector<Obstacle> obstacles_fixed;
  std::vector<Obstacle> cylinders_fixed;

  int datasetId, frameId;
};

#endif // OBTAINFROMFILE_H
