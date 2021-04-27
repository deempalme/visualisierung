#include "extras/headers/kittiDataIMU.h"

kittiDataIMU::kittiDataIMU() :
  vehicle(),
  folderPath(),
  frame(0),
  total_frames(0),
  frameLatitude(0),
  frameLongitude(0),
  frameAltitude(0),
  isFrameSet(false),
  conversor(nullptr)
{
}

bool kittiDataIMU::setDataSet(const unsigned int _datasetNumber){
  bool result = false;

  folderPath = qApp->applicationDirPath() + "/" + QString("../KittiData/raw/%1_sync/oxts/data/")
      .arg(QString::number(_datasetNumber).rightJustified(4, '0'));

  frame = 0;
  total_frames = 0;

  //opening the folder
  QDir cDir(folderPath);
  if(cDir.exists()){
    isFrameSet = false;
    //viewing all the files inside the folder
    cDir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    //counting all the files
    total_frames = cDir.count();

    result = true;
  }
  return result;
}

bool kittiDataIMU::gotoFrame(const unsigned int _frameNumber){
  bool result = false;

  if(_frameNumber < total_frames){
    frame = _frameNumber;
    QString fName = QString::number(frame).rightJustified(10, '0') + ".txt";
    vector<double> temporal;

    //open the txt file of the oxt folder
    QFile file(folderPath + fName);
    if(file.open(QFile::ReadOnly)){
      while(!file.atEnd()){
        //read each line
        QString vehDatum = file.readLine();
        //separates the line in each space
        QStringList vehData = vehDatum.split(" ");

        //places the values as doubles in the "temporal" vector
        //see the data format located in the oxt folder,
        //example: temporal[2] is equal to altitude
        for(int i = 0; i < vehData.size(); i++)
          temporal.push_back(QString(vehData[i]).toDouble());

        //now we are creating our values,
        //some of them like RPM, Brake, Gas, etc. are not in the IMU file
        //so we are creating fake data
        //0 : latitude
        vehicle.latitude = QString(vehData[0]).toDouble();
//        vehicle.latitude = 50.78241;
        //1 : longitude
        vehicle.longitude = QString(vehData[1]).toDouble();
//        vehicle.longitude = 6.04677;
        //2 : altitude
        vehicle.altitude = QString(vehData[2]).toDouble();
        //3 : roll
        vehicle.roll = QString(vehData[3]).toDouble();
        //4 : pitch
        vehicle.pitch = QString(vehData[4]).toDouble();
        //5 : yaw
        vehicle.yaw = QString(vehData[5]).toDouble();
        //6 : speed
        vehicle.velocity = qSqrt(qPow(QString(vehData[8]).toDouble(), 2) + qPow(QString(vehData[9]).toDouble(), 2) + qPow(QString(vehData[10]).toDouble(), 2)) * toKMPH;
        //7 : rpm
        vehicle.rpm = vehicle.velocity * 120;
        //8 : forward acceleration
        vehicle.accelerationX = QString(vehData[14]).toDouble();
        //9 : upward acceleration
        vehicle.accelerationZ = QString(vehData[16]).toDouble();
        //10 : gas
        vehicle.gas = (vehicle.accelerationX > 0)? qAbs(vehicle.accelerationX/5.0f) : 0;
        //11 : brakes
        vehicle.brakes = (vehicle.accelerationX < 0)? qAbs(vehicle.accelerationX/5.0f) : 0;
        //12 : steering
        vehicle.steeringAngle = -0.9 * QString(vehData[15]).toDouble();
        //13 : battery
        vehicle.battery = vehicle.velocity * 2.32;
        //14 : gearbox
        double gearbox;
        if(vehicle.velocity < 0)
          gearbox = 4;
        else if(vehicle.velocity < 15)
          gearbox = -2;
        else if(vehicle.velocity < 25)
          gearbox = -1;
        else if(vehicle.velocity < 35)
          gearbox = 0;
        else if(vehicle.velocity < 45)
          gearbox = 1;
        else if(vehicle.velocity < 60)
          gearbox = 2;
        else if(vehicle.velocity < 75)
          gearbox = 3;
        vehicle.gearbox = gearbox;

        if(!isFrameSet){
          frameLatitude = vehicle.latitude;
          frameLongitude = vehicle.longitude;
          frameAltitude = vehicle.altitude;
          framePitch = vehicle.pitch;
          frameYaw = vehicle.yaw;
          frameRoll = vehicle.roll;
          isFrameSet = true;
        }

        if(conversor != nullptr){
          QVector2D converted(conversor->GPStoXY(vehicle.latitude, vehicle.longitude));

          double x = converted.x();
          double y = converted.y();
          double z = vehicle.altitude - frameAltitude;
          double distance = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
          double angle = atan(y/x);
          if(x == 0)
            if(y == 0)
              angle = 0;
            else if(y > 0)
              angle = _PI2;
            else if(y < 0)
              angle = _3PI2;
          if(x < 0 && y >= 0)
            angle += _PI;
          else if(x < 0 && y < 0)
            angle += _PI;
          else if(x > 0 && y < 0)
            angle += _2PI;
          angle -= frameYaw;
          x = distance * cos(angle);
          y = distance * sin(angle);

          //15 : vehicle position X in meters with origin in the static frame origin
          vehicle.x = x;
          //16 : vehicle position Y in meters with origin in the static frame origin
          vehicle.y = y;
        }else{
          //15 : vehicle position X in meters with origin in the static frame origin
          vehicle.x = 0;
          //16 : vehicle position Y in meters with origin in the static frame origin
          vehicle.y = 0;
        }
        //17 : vehicle position Z in meters with origin in the static frame origin
        vehicle.z = vehicle.altitude - frameAltitude;
        //18 : vehicle forward velocity
        vehicle.velocityX = QString(vehData[8]).toDouble();
        //19 : vehicle leftward velocity
        vehicle.velocityY = QString(vehData[9]).toDouble();
        //20 : vehicle upward velocity
        vehicle.velocityZ = QString(vehData[10]).toDouble();
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

unsigned int kittiDataIMU::actualFrame(){
  return frame;
}

unsigned int kittiDataIMU::totalFrames(){
  return total_frames;
}

void kittiDataIMU::setStaticFrameOrigin(const double _latitude, const double _longitude, const double _altitude,
                                        const double _pitch, const double _yaw, const double _roll){
  frameLatitude = _latitude;
  frameLongitude = _longitude;
  frameAltitude = _altitude;
  framePitch = _pitch;
  frameYaw = _yaw;
  frameRoll = _roll;
  isFrameSet = true;
}

void kittiDataIMU::setConversor(CoordinateConversor *_conversor){
  conversor = _conversor;
}
