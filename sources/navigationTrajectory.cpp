#include "headers/navigationTrajectory.h"

#include <QFile>

NavigationTrajectory::NavigationTrajectory(const QString _folder_path, CoordinateConversor *_conversor) :
  folder_path(_folder_path),
  conversor(_conversor),
  trajectories(0),
  show_alternative(false)
{
  calculate();
}

void NavigationTrajectory::calculate(){
  trajectories.clear();

  vector<trajectoryData> vertices;
  trajectoryData vertex;
  QVector2D node;
  QString extra;

  vertex.r = 82;
  vertex.g = 159;
  vertex.b = 219;
  vertex.lineWidth = 4.5;

  QDir dir(folder_path, "*.csv");
  QDir dir2(folder_path, "*.gpx");

  //.csv files
  for(uint i = 0; i < dir.count(); i++){
    QFile file(dir.filePath(dir.entryList().at(i)));
    if(file.open(QIODevice::ReadOnly | QFile::Text)){
      QTextStream in(&file);

      while(!in.atEnd()){
        QString line(in.readLine());
        if(line.isEmpty())
          continue;

        if(line == "+"){
          trajectories.push_back(vertices);
          vertices.clear();
        }else{
          QStringList coordinates(line.split(","));
          if(coordinates.size() == 2){
            node = conversor->GPStoXY(QString(coordinates.at(0)).toDouble(), QString(coordinates.at(1)).toDouble());
            vertex.positionX = -node[0];
            vertex.positionY = -node[1];
            vertices.push_back(vertex);
          }else
            continue;
        }
      }
      file.close();
    }

    if(vertices.size() > 0){
      trajectories.push_back(vertices);
      vertices.clear();
    }
  }

  //.gpx files
  for(uint i = 0; i < dir2.count(); i++){
    if(show_alternative)
      extra = "_alternative";
    else
      extra ="";

    QFile file(dir2.filePath(dir2.entryList().at(i)) + extra);
    if(file.open(QIODevice::ReadOnly | QFile::Text)){
      QTextStream in(&file);

      while(!in.atEnd()){
        QString line(in.readLine());
        if(line.isEmpty())
          continue;

        if(line == "+"){
          trajectories.push_back(vertices);
          vertices.clear();
        }else{
          QStringList coordinates(line.split(","));
          if(coordinates.size() == 2){
            node = conversor->GPStoXY(QString(coordinates.at(1)).toDouble(), QString(coordinates.at(0)).toDouble());
            vertex.positionX = -node[0];
            vertex.positionY = -node[1];
            vertices.push_back(vertex);
          }else
            continue;
        }
      }
      file.close();
    }

    if(vertices.size() > 0){
      trajectories.push_back(vertices);
      vertices.clear();
    }
  }
}

void NavigationTrajectory::recalculate(bool _alternative){
  show_alternative = _alternative;
  calculate();
}

const vector<vector<trajectoryData> > *const NavigationTrajectory::GetTrajectories(){
  return &trajectories;
}

