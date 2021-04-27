#include "extras/headers/kittiPCLData.h"

kittiPCLData::kittiPCLData(const QString _folder) :
  folderPath(),
  folder(_folder),
  frame(0),
  total_frames(0)
{

}

bool kittiPCLData::setDataSet(const unsigned int _datasetNumber){
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
    laserCloud.clear();
  }
  return result;
}

bool kittiPCLData::gotoFrame(const unsigned int _frameNumber){
  bool result = false;
  unsigned int float_s = sizeof(float);

  //check if the selected frame number is bigger than existing frames
  if(_frameNumber < total_frames){

    //clear all the laser cloud information existing
    laserCloud.clear();
    //creates temporal variables out of the loop to avoid creating them in each laser point
    pointRGB datum;

    //this creates the file name string: if frame is 1 then the resulting name is
    //0000000001.bin
    frame = _frameNumber;
    QString fName = QString::number(frame).rightJustified(10, '0') + ".bin";

    //open the bin file of the velodyne_points folder
    QFile file(folderPath + fName);
    if(file.open(QFile::ReadOnly)){
      //this will read each laser point
      while(!file.atEnd()){
        //position X, Y, Z
        file.read((char *)&datum, 7 * float_s);

        laserCloud.push_back(datum);
      }
      //close the open file
      file.close();
      result = true;
    }else
      //if an error occurs opening the file then this line will pop up
      qWarning() << "the file: " << folderPath + fName << " was not found.";
    timestamp = "2016-12-31 23:59:99.999";
  }
  return result;
}

unsigned int kittiPCLData::actualFrame(){
  return frame;
}

unsigned int kittiPCLData::totalFrames(){
  return total_frames;
}

const vector<pointRGB> *const kittiPCLData::LaserCloud(){
  return &laserCloud;
}

const QString *const kittiPCLData::Timestamp(){
  return &this->timestamp;
}

kittiPCLData::~kittiPCLData(){
  laserCloud.clear();
}
