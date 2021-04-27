#include "headers/ibeoLaserReader.h"

//ibeoLaserData::ibeoLaserData(const float _angularResolutionDEGREES, const int _numberOfLayers,
//                             const float _verticalAngleBetweenLayersDEGREES) :
//  dataCloud(0),
//  angularResolution(_angularResolutionDEGREES),
//  verticalAngle(_verticalAngleBetweenLayersDEGREES),
//  layerNumber(_numberOfLayers)
//{}

//bool ibeoLaserData::onData(const ScanLux* const scan){
//  logInfo;
//  pointDAI temporal;

//  const std::vector<ScanPointLux>& scanpoints = scan->getScanPoints();
//  dataCloud.clear();

//  for(unsigned int i = 0; i < scanpoints.size(); ++i){
//    temporal.distance = (float)scanpoints.at(i).getDistance();
//    temporal.angle = (float)scanpoints.at(i).getHorizontalAngle();
//    temporal.intensity = (float)scanpoints.at(i).getEchoPulseWidth();
//    temporal.layer = (float)scanpoints.at(i).getLayer();
//    dataCloud.push_back(temporal);
//  }
//}

//ibeoLaserData::~kittiPCLData(){
//  dataCloud.clear();
//}

