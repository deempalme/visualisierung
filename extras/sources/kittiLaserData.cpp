#include "extras/headers/kittiLaserData.h"

kittiLaserData::kittiLaserData() :
  folderPath(),
  frame(0),
  total_frames(0)
{
}

bool kittiLaserData::setDataSet(const unsigned int _datasetNumber){
  bool result = false;

  folderPath = qApp->applicationDirPath() + "/" + QString("../KittiData/raw/%1_sync/velodyne_points/").arg(QString::number(_datasetNumber).rightJustified(4, '0'));

  frame = 0;
  total_frames = 0;

  //opening the folder
  QDir cDir(folderPath + "data/");
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

bool kittiLaserData::gotoFrame(const unsigned int _frameNumber){
  bool result = false;
  unsigned int float_s = sizeof(float);

  //check if the selected frame number is bigger than existing frames
  if(_frameNumber < total_frames){

    //clear all the laser cloud information existing
    laserCloud.clear();
    //creates temporal variables out of the loop to avoid creating them in each laser point
    LaserCloudData datum;

    //this creates the file name string: if frame is 1 then the resulting name is
    //0000000001.bin
    frame = _frameNumber;
    QString fName = QString::number(frame).rightJustified(10, '0') + ".bin";

    //open the bin file of the velodyne_points folder
    QFile file(folderPath + "data/" + fName);
    if(file.open(QFile::ReadOnly)){
      //this will read each laser point
      while(!file.atEnd()){
        //position X
        file.read((char *)&datum.x, float_s);
        //position Y
        file.read((char *)&datum.y, float_s);
        //position Z
        file.read((char *)&datum.z, float_s);
        //note that the coordinate system in OpenGL (z axis pointing towards screen)
        //is different than normal coordinate systems (z axis pointing upwards)

        //intensity, note: in ROS the intensity goes from 0 to 255
        //in KittiData it goes from 0 to 1
        file.read((char *)&datum.intensity, float_s);

//        QVector3D datum2(-datum.y, datum.z, -datum.x);
//        QMatrix4x4 rotation;
//        rotation.rotate(QQuaternion::fromEulerAngles(0, 120, 0));
//        datum2 = rotation * datum2;
//        datum.x = -datum2.z();
//        datum.y = -datum2.x();
//        datum.z = datum2.y();

//        datum.z += 1.46;
//        datum.y += 1.1;
//        datum.x -= 1;

        laserCloud.push_back(datum);
      }
      //close the open file
      file.close();
      result = true;
    }else
      //if an error occurs opening the file then this line will pop up
      qWarning() << "the file: " << folderPath + fName << " was not found.";
    QFile fileT(folderPath + "timestamps.txt");
    if(fileT.open(QFile::ReadOnly)){
      timestamp = fileT.readLine();
      for(int i = 0; i < frame; i++){
        timestamp = fileT.readLine();
      }
      //close the open file
      fileT.close();
      //timestamp format : 0000-12-31 24:60:60.999
      timestamp.resize(23);
    }else{
      result = false;
      //if an error occurs opening the timestamp file then this line will pop up
      qWarning() << "the timestamp file: " << folderPath << "timestamps.txt was not found.";
    }

//    QFile file2("/home/framirez/Documents/web/droiddrive/raw_data/binary.bin");
//    if(file2.open(QIODevice::WriteOnly)){
//      file2.write(reinterpret_cast<char*>(laserCloud.data()), laserCloud.size() * sizeof(LaserCloudData));
//      file2.close();
//    }else{
//      qWarning() << "file not open;";
//    }

  }
  /*
  QElapsedTimer timer;
  timer.start();
  uint i = 0;
  for(i = 0; i < 100000; i++){
    QVector3D point(laserCloud[i].x, laserCloud[i].y, laserCloud[i].z);
    QMatrix4x4 matrix;
    matrix.translate(5, 1, 3);
    matrix.rotate(QQuaternion::fromEulerAngles(20, 15, 60));

    point = matrix * point;
  }
  qWarning("Operations: %u - total time required: %.4f milliseconds", i, 0.000001 * timer.nsecsElapsed());
*/
  return result;
}

unsigned int kittiLaserData::actualFrame(){
  return frame;
}

unsigned int kittiLaserData::totalFrames(){
  return total_frames;
}

const vector<LaserCloudData> *const kittiLaserData::LaserCloud(){
  return &this->laserCloud;
}

const QString *const kittiLaserData::Timestamp(){
  return &this->timestamp;
}

kittiLaserData::~kittiLaserData(){
  laserCloud.clear();
}
