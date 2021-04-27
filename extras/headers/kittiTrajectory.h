#ifndef KITTITRAJECTORY_H
#define KITTITRAJECTORY_H

#include <QObject>
#include "headers/trajectory.h"

// Loads trajectories from KittiData
class kittiTrajectoryLoader : public QObject
{
  Q_OBJECT
public:

  // KittiFolderPath points to directory where "raw"-folder is located
  kittiTrajectoryLoader(const QString& KittiFolderPath);
  void LoadFromFile(const int datasetId);
  const std::vector< std::vector<trajectoryData> >* GetTrajectories() const;
  static std::vector<trajectoryData> ConvertVectorToTrajectory(const QVector<float> &trajectoryVec, const bool multicoloredTrajectory = false);
  void setGUIId(unsigned int id);

public slots:
  //set a new kitti Dataset,
  //returns false if the folder was not found
  //if you set datasetNumber = 1; then the complete folder name will be 0001_sync
  //note that dates from the folder name were removed, example: "2011_09_26_0001_sync" --> "0001_sync"
  bool setDataSet(const unsigned int _datasetNumber = 1);

signals:
  void updateTrajectory(unsigned int id);

private:
  QString folderPath;
  std::vector< std::vector<trajectoryData> > trajectories;
  unsigned int GUI_id = -1;

};

#endif // KITTITRAJECTORY_H
