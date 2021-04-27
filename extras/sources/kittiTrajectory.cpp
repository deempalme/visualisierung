#include "extras/headers/kittiTrajectory.h"

#include <QFile>

kittiTrajectoryLoader::kittiTrajectoryLoader(const QString& folderPath)
  : folderPath(folderPath)
{
}

void kittiTrajectoryLoader::LoadFromFile(const int datasetId){
  this->trajectories.clear();

  QString fileName = "trajectories.txt";
  QString filePath = folderPath + QString("raw/%1_sync/")
      .arg(QString::number(datasetId).rightJustified(4, '0')) + fileName;

  QFile inputFile(filePath);
  if(inputFile.open(QIODevice::ReadOnly)){
    QTextStream in(&inputFile);
    QVector<float> trajectoryVec;
    bool multicoloredTrajectory = false;
    while(!in.atEnd()){
      QString line = in.readLine();
      if(line.isEmpty() || line.contains(QChar('#'))){ // comments with #
        continue;
      }
      if(line.contains("---")){ // new trajectory starts with ---
        std::vector<trajectoryData> trajectory = ConvertVectorToTrajectory(trajectoryVec, multicoloredTrajectory);
        if(!trajectory.empty()){
          this->trajectories.push_back(trajectory);
        }
        trajectoryVec.clear();
        multicoloredTrajectory = false;
        continue;
      }
      if(line.compare("multicolored") == 0){
        multicoloredTrajectory = true;
        continue;
      }
      QRegExp rx("(\\ |\\,|\\;)"); //RegEx for ' ' or ',' or ';'
      QStringList query = line.split(rx);
      for(int i=0; i<query.size(); ++i){
        trajectoryVec.push_back(query[i].toFloat());
      }
    }
    if(!trajectoryVec.empty()){
      std::vector<trajectoryData> trajectory = ConvertVectorToTrajectory(trajectoryVec, multicoloredTrajectory);
      if(!trajectory.empty()){
        this->trajectories.push_back(trajectory);
      }
    }
    inputFile.close();
  }
}

const std::vector< std::vector<trajectoryData> >* kittiTrajectoryLoader::GetTrajectories() const{
  return &this->trajectories;
}

std::vector<trajectoryData> kittiTrajectoryLoader::ConvertVectorToTrajectory(const QVector<float>& trajectoryVec, const bool multicoloredTrajectory){
  std::vector<trajectoryData> trajectory;
  if(trajectoryVec.size() < 11){
    return trajectory;
  }
  trajectoryData tData;
  tData.lineType = trajectoryVec[0];
//  tData.lineWidth = trajectoryVec[1];
  tData.lineWidth = 4;
  if(!multicoloredTrajectory){
    tData.r = trajectoryVec[2];
    tData.g = trajectoryVec[3];
    tData.b = trajectoryVec[4];
    for(int i = 5; i < trajectoryVec.size(); i+=3){
      tData.positionX = trajectoryVec[i];
      tData.positionY = trajectoryVec[i+1];
      tData.positionZ = trajectoryVec[i+2];
      trajectory.push_back(tData);
    }
  }else{
    for(int i = 2; i < trajectoryVec.size(); i+=6){
      tData.positionX = trajectoryVec[i];
      tData.positionY = trajectoryVec[i+1];
      tData.positionZ = trajectoryVec[i+2];
      tData.r = trajectoryVec[i+3];
      tData.g = trajectoryVec[i+4];
      tData.b = trajectoryVec[i+5];
      trajectory.push_back(tData);
    }
  }
  return trajectory;
}

void kittiTrajectoryLoader::setGUIId(unsigned int id){
  GUI_id = id;
}

// --------------------- Slots ---------------------------------

bool kittiTrajectoryLoader::setDataSet(const unsigned int _datasetNumber){
  LoadFromFile(_datasetNumber);
  emit updateTrajectory(GUI_id);
}
