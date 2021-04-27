#include "extras/headers/kittiGroundExtractor.h"

KittiGroundExtractor::KittiGroundExtractor(const QString _folder) :
  folderPath(),
  folder(_folder),
  frame(0),
  total_frames(0)
{

}

KittiGroundExtractor::~KittiGroundExtractor(){
  groundData.clear();
}

unsigned int KittiGroundExtractor::actualFrame(){
  return frame;
}

unsigned int KittiGroundExtractor::totalFrames(){
  return total_frames;
}

const vector<Boxes> *const KittiGroundExtractor::GroundData(){
  return &groundData;
}

bool KittiGroundExtractor::setDataSet(const unsigned int _datasetNumber){
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
    groundData.clear();
  }
  return result;
}

bool KittiGroundExtractor::gotoFrame(const unsigned int _frameNumber){
  bool result = false;
  unsigned int float_s = sizeof(float);

  //check if the selected frame number is bigger than existing frames
  if(_frameNumber < total_frames){

    //clear all the laser cloud information existing
    groundData.clear();
    //creates temporal variables out of the loop to avoid creating them in each laser point
    Boxes datum;

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
        file.read((char *)&datum, 9 * float_s);

        groundData.push_back(datum);
      }
      //close the open file
      file.close();
      result = true;
    }else
      //if an error occurs opening the file then this line will pop up
      qWarning() << "the file: " << folderPath + fName << " was not found.";
  }
  return result;
}
