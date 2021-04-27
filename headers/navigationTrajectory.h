#ifndef NAVIGATIONTRAJECTORY_H
#define NAVIGATIONTRAJECTORY_H

#include <QObject>
#include <QFile>
#include <QDir>
#include "headers/trajectory.h"
#include "headers/coordinateconversor.h"

using namespace std;

// Loads trajectories from KittiData
class NavigationTrajectory : public QObject
{
  Q_OBJECT
public:

  NavigationTrajectory(const QString _folder_path, CoordinateConversor *_conversor);
  const vector<vector<trajectoryData> > *const GetTrajectories();

public slots:
  void calculate();
  void recalculate(bool _alternative);

private:
  QString folder_path;
  CoordinateConversor *conversor;
  vector<vector<trajectoryData> > trajectories;
  bool show_alternative;
};

#endif // NAVIGATIONTRAJECTORY_H
