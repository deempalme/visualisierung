#include "extras/headers/kittiCustomObstacles.h"

kittiCustomObstacleLoader::kittiCustomObstacleLoader(const QString& folderPath)
  : folderPath(folderPath)
{
}

void kittiCustomObstacleLoader::LoadFromFile(const int datasetId, const int frameId){
  obstacles.clear();
  cylinders.clear();
  obstacles_fixed.clear();
  cylinders_fixed.clear();

  QString fileName = QString::number(frameId).rightJustified(10, '0') + ".txt";
  QString filePath = qApp->applicationDirPath() + "/" + folderPath + QString("raw/%1_sync/custom/")
      .arg(QString::number(datasetId).rightJustified(4, '0')) + fileName;

  QFile inputFile(filePath);
  if (inputFile.open(QIODevice::ReadOnly))
  {
    QTextStream in(&inputFile);
    while (!in.atEnd())
    {
      QString line = in.readLine();
      if(line.contains(QChar('#'))) // comments with #
      {
        continue;
      }
      QRegExp rx("(\\ |\\,|\\;)"); //RegEx for ' ' or ',' or ';'
      QStringList query = line.split(rx);
      QVector<float> obstacleVec(16, 0);
      if(query.size() >= 6)
      {
        for(int i=0; i<query.size() && i<obstacleVec.size(); ++i)
        {
          obstacleVec[i] = query[i].toFloat();
        }
        // 14th element of line is cylinder flag
        // if it's greater zero the obstacle is actually a cylinder
        // 15th element is flag if obstacle is relative to dataset origin or relative to car
        if(query.size() > 13 && query[13].toInt() > 0)
        {
            if(query.size() > 14 && query[14].toInt() > 0){
                cylinders_fixed.push_back(ConvertVectorToObstacle(obstacleVec));
            }
            else
                cylinders.push_back(ConvertVectorToObstacle(obstacleVec));
        }
        else
        {
            if(query.size() > 14 && query[14].toInt() > 0)
                obstacles_fixed.push_back(ConvertVectorToObstacle(obstacleVec));
            else
                obstacles.push_back(ConvertVectorToObstacle(obstacleVec));
        }
      }
    }
    inputFile.close();
  }

}

const std::vector<Obstacle>* kittiCustomObstacleLoader::GetObstacles() const{
  return &obstacles;
}

const std::vector<Obstacle>* kittiCustomObstacleLoader::GetCylinders() const{
  return &cylinders;
}

const std::vector<Obstacle>* kittiCustomObstacleLoader::GetObstaclesFixed() const
{
    return &obstacles_fixed;
}

const std::vector<Obstacle>* kittiCustomObstacleLoader::GetCylindersFixed() const
{
    return &cylinders_fixed;
}

Obstacle kittiCustomObstacleLoader::ConvertVectorToObstacle(const QVector<float>& obstacleVec){
  Obstacle obstacle;
  if(obstacleVec.size() > 15){
    obstacle.width = obstacleVec[0];
    obstacle.length = obstacleVec[1];
    obstacle.height = obstacleVec[2];
    obstacle.positionX = obstacleVec[3];
    obstacle.positionY = obstacleVec[4];
    obstacle.positionZ = obstacleVec[5];
    obstacle.roll = obstacleVec[6];
    obstacle.pitch = obstacleVec[7];
    obstacle.yaw = obstacleVec[8];
    obstacle.r = obstacleVec[9];
    obstacle.g = obstacleVec[10];
    obstacle.b = obstacleVec[11];
    obstacle.lineWidth = obstacleVec[12];
    // 13 and 14 are for deciding between circle and box and fixed frame vs sensor frame
    obstacle.arrow = obstacleVec[15] > 0;
  }
  return obstacle;
}


// --------------------- Slots ---------------------------------

bool kittiCustomObstacleLoader::setDataSet(const unsigned int _datasetNumber){
  datasetId = _datasetNumber;
  LoadFromFile(datasetId, frameId);
}

bool kittiCustomObstacleLoader::gotoFrame(const unsigned int _frameNumber){
  frameId = _frameNumber;
  LoadFromFile(datasetId, frameId);
}
