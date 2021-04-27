#include "extras/headers/kittiTextLabels.h"

kittiTextLabels::kittiTextLabels(const QString _folder) :
  folder(_folder),
  frame(0),
  total_frames(0)
{

}

bool kittiTextLabels::setDataSet(const unsigned int _datasetNumber){
  bool result = false;

  folderPath = qApp->applicationDirPath() + "/" + QString("../KittiData/raw/%1_sync/" + folder + "/").arg(QString::number(_datasetNumber).rightJustified(4, '0'));

  frame = 0;
  total_frames = 0;

  //opening the folder
  QDir cDir(folderPath);
  if(cDir.exists()){
    //viewing all the files inside the folder
    cDir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    //counting all the files
    total_frames = cDir.count();

    result = true;
    labels.clear();
  }
  return result;
}

bool kittiTextLabels::gotoFrame(const unsigned int _frameNumber){
  bool result = false;

  //check if the selected frame number is bigger than existing frames
//  if(_frameNumber < total_frames){

    //clear all the laser cloud information existing
    labels.clear();
    //creates temporal variables out of the loop to avoid creating them in each laser point
    XYZLabel datum;

    //this creates the file name string: if frame is 1 then the resulting name is
    //0000000001.bin
    frame = _frameNumber;
    QString fName = QString::number(frame).rightJustified(10, '0') + ".txt";
    QString line;
    QStringList parts;

    //open the bin file of the velodyne_points folder
    QFile file(folderPath + fName);
    if(file.open(QIODevice::ReadOnly)){
      QTextStream stream(&file);
      //this will read each laser point
      while(!stream.atEnd()){
        line = stream.readLine();
        if(!line.isEmpty()){
          parts = line.split("~");
          datum.positionX = parts[0].toFloat();
          datum.positionY = parts[1].toFloat();
          datum.positionZ = parts[2].toFloat();
          datum.r = parts[3].toFloat();
          datum.g = parts[4].toFloat();
          datum.b = parts[5].toFloat();
          datum.label = parts[6];
        }
        labels.push_back(datum);
      }
      //close the opened file
      file.close();
      result = true;
    }else
      //if an error occurs opening the file then this line will pop up
      qWarning() << "the file: " << folderPath + fName << " was not found.";
  //}

  return result;
}

const vector<XYZLabel> *const kittiTextLabels::textLabels(){
  return &labels;
}

kittiTextLabels::~kittiTextLabels(){
  labels.clear();
}
