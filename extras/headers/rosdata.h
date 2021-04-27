#ifndef ROSDATA_H
#define ROSDATA_H

#include <QObject>
#include <QQuaternion>
#include <QDateTime>
#include <math.h>
#include <float.h>

#include "headers/definitions.h"
#include "headers/console.h"
#include "headers/coordinateconversor.h"

#include <ros/ros.h>

#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/point_cloud2_iterator.h>

#include <sensor_msgs/NavSatFix.h>
#include <sensor_msgs/Imu.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/TwistWithCovarianceStamped.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <geometry_msgs/AccelWithCovarianceStamped.h>

#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>

#include "visualization_node/object.h"
#include "visualization_node/object_vec.h"
#include "mabx/LowLevelInfo_vis.h"

#include "navigation/nav_alternative.h"

using namespace message_filters;
using namespace sensor_msgs;
using namespace geometry_msgs;

typedef sync_policies::ApproximateTime<sensor_msgs::NavSatFix,
                                       sensor_msgs::Imu,
//                                       geometry_msgs::AccelWithCovarianceStamped,
//                                       geometry_msgs::PoseWithCovarianceStamped,
                                       geometry_msgs::TwistWithCovarianceStamped> MySyncPolicy;
class ROSData : public QObject
{
  Q_OBJECT
public:
  ROSData();
  ~ROSData(){}

  void points(const sensor_msgs::PointCloud2::ConstPtr &cloud);
  void setObjects(const visualization_node::object_vec::ConstPtr &obj);
  void setParameters(const mabx::LowLevelInfo_vis::ConstPtr &info);
  void setGPS(const sensor_msgs::NavSatFix::ConstPtr &gps,
              const sensor_msgs::Imu::ConstPtr &pose,
              const geometry_msgs::TwistWithCovarianceStamped::ConstPtr &vel);
  void setAlternative(const navigation::nav_alternative::ConstPtr &alt);

  FrameParameters *const getFrameValues();
  VehicleParameters *const getVehicleValues();
  const vector<LaserCloudData> *const getCloudData();
  const vector<Obstacle> *const getDynamicBoxes();
  const vector<Obstacle> *const getStaticBoxes();
  const vector<XYZLabel> *const getDynamicTexts();
  const vector<XYZLabel> *const getStaticTexts();
  double *const getMaximum();

  void setStaticFrameOrigin(const double _latitude, const double _longitude, const double _altitude,
                            const double _pitch = 0, const double _yaw = 0, const double _roll = 0);

  void setConversor(CoordinateConversor *_conversor);

  bool isins, ispos, isvel, isalt, istat;
  ros::NodeHandle nh;

private:
  ros::Subscriber input_cloud, input_object, input_parameters, input_alternative; //, input_dynamic_box, input_dynamic_text;
//  ros::Subscriber input_dynamic_vec, input_static_box, input_static_text;

  message_filters::Subscriber<sensor_msgs::NavSatFix> *sub_gps;
  message_filters::Subscriber<sensor_msgs::Imu> *sub_imu;
  message_filters::Subscriber<geometry_msgs::TwistWithCovarianceStamped> *sub_vel;
//  message_filters::Subscriber<geometry_msgs::AccelWithCovarianceStamped> *sub_acc;
//  message_filters::Subscriber<geometry_msgs::PoseWithCovarianceStamped> *sub_pose;
//  TimeSynchronizer<sensor_msgs::NavSatFix, sensor_msgs::Imu, geometry_msgs::TwistWithCovarianceStamped> *timesync;
  Synchronizer<MySyncPolicy> *sync;

  vector<LaserCloudData> laserCloud;
  vector<Obstacle> dynamic_boxes, static_boxes;
  vector<XYZLabel> dynamic_texts, static_texts;
  FrameParameters frame;
  VehicleParameters vehicle;

  bool isFrameSet;

  CoordinateConversor *conversor;

  double maximum;

signals:
  void alternativeChanged(bool _alternative);
};

#endif // ROSDATA_H
