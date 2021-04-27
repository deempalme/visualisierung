#ifndef OBTAINIBEODATA_H
#define OBTAINIBEODATA_H

#include <QDebug>
#include <QObject>

#include "headers/definitions.h"

//ideo libraries
#include <ibeosdk/lux.hpp>
#include <ibeosdk/IpHelper.hpp>

#include <ibeosdk/datablocks/commands/CommandLuxReset.hpp>
#include <ibeosdk/datablocks/commands/CommandLuxGetStatus.hpp>
#include <ibeosdk/datablocks/commands/CommandLuxGetParameter.hpp>
#include <ibeosdk/datablocks/commands/CommandLuxSetParameter.hpp>
#include <ibeosdk/datablocks/commands/EmptyCommandReply.hpp>
#include <ibeosdk/datablocks/commands/ReplyLuxGetStatus.hpp>
#include <ibeosdk/datablocks/commands/ReplyLuxGetParameter.hpp>
#include <ibeosdk/datablocks/commands/CommandLuxSetNtpTimestampSync.hpp>
//end ibeo libraries

#include <iostream>
#include <cstdlib>

//======================================================================

using namespace ibeosdk;

//======================================================================

const ibeosdk::Version::MajorVersion majorVersion(5);
const ibeosdk::Version::MinorVersion minorVersion(0);
const ibeosdk::Version::Revision revision(4);
const ibeosdk::Version::PatchLevel patchLevel;
const ibeosdk::Version::Build build;
const std::string info = "IbeoSdkLuxLiveDemo";

ibeosdk::Version appVersion(majorVersion, minorVersion, revision, patchLevel, build, info);
IbeoSDK ibeoSDK;

//======================================================================

TimeConversion tc;

//======================================================================

class ibeoLaserData : public QObject,
    public ibeosdk::DataListener<ScanLux>,
    public ibeosdk::DataListener<ObjectListLux>,
    public ibeosdk::DataListener<VehicleStateBasicLux>,
    public ibeosdk::DataListener<LogMessageError>,
    public ibeosdk::DataListener<LogMessageDebug>,
    public ibeosdk::DataListener<LogMessageNote>,
    public ibeosdk::DataListener<LogMessageWarning>
{
  Q_OBJECT
public:
  ibeoLaserData(const float _angularResolutionDEGREES = 1/32, const float _verticalAngleBetweenLayersDEGREES = 0.8) :
    dataCloud(0),
    angularResolution(_angularResolutionDEGREES),
    verticalAngle(_verticalAngleBetweenLayersDEGREES)
  {}
  ~ibeoLaserData(){ dataCloud.clear(); }

  const vector<pointDAI> *const LaserCloud(){ return &dataCloud; }
  const float getAngularResolution(){ return angularResolution; }
  const float getVerticalAngle(){ return verticalAngle; }

public slots:
  //========================================
  bool onData(const ScanLux* const scan){
    logInfo;
    pointDAI temporal;

    const std::vector<ScanPointLux>& scanpoints = scan->getScanPoints();
    dataCloud.clear();

    for(unsigned int i = 0; i < scanpoints.size(); ++i){
      temporal.distance = (float)scanpoints.at(i).getDistance();
      temporal.angle = (float)scanpoints.at(i).getHorizontalAngle();
      temporal.intensity = (float)scanpoints.at(i).getEchoPulseWidth();
      temporal.layer = (float)scanpoints.at(i).getLayer();
      dataCloud.push_back(temporal);
    }
  }
  //========================================
  void onData(const ObjectListLux* const pObj){
    logInfo << "Objects received: # " << pObj->getNumberOfObjects() << std::endl;
  }

  //========================================
  void onData(const VehicleStateBasicLux* const vsb){
    logInfo << "VSB for Scan #: " << vsb->getScanNumber() << "  "
            << tc.toString(vsb->getTimestamp().toPtime()) << std::endl;
  }

  //========================================
  virtual void onData(const LogMessageError* const logMsg){
    logInfo << std::setw(5) << logMsg->getSerializedSize() << " Bytes  "
            << "LogMessage (Error) received: time: " << logMsg->getTraceLevel() << ": " << logMsg->getMessage() << std::endl;
  }

  //========================================
  virtual void onData(const LogMessageWarning* const logMsg){
    logInfo << std::setw(5) << logMsg->getSerializedSize() << " Bytes  "
            << "LogMessage (Warning) received: time: " << logMsg->getTraceLevel() << ": " << logMsg->getMessage() << std::endl;
  }

  //========================================
  virtual void onData(const LogMessageNote* const logMsg){

    logInfo << std::setw(5) << logMsg->getSerializedSize() << " Bytes  "
            << "LogMessage (Note) received: time: " << logMsg->getTraceLevel() << ": " << logMsg->getMessage() << std::endl;
  }

  //========================================
  virtual void onData(const LogMessageDebug* const logMsg){
    logInfo << std::setw(5) << logMsg->getSerializedSize() << " Bytes  "
            << "LogMessage (Debug) received: time: " << logMsg->getTraceLevel() << ": " << logMsg->getMessage() << std::endl;
  }

private:
  vector<pointDAI> dataCloud;
  float angularResolution, verticalAngle;
};

#endif // OBTAINIBEODATA_H
