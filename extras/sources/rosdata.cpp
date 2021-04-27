#include "headers/rosdata.h"

ROSData::ROSData() :
  isins(true),
  ispos(true),
  isvel(true),
  isalt(true),
  istat(true),
  laserCloud(0),
  dynamic_boxes(0),
  static_boxes(0),
  dynamic_texts(0),
  static_texts(0),
  frame(),
  vehicle(),
  isFrameSet(false),
  conversor(nullptr),
  maximum(-DBL_MAX)
{
  input_cloud = nh.subscribe("point_cloud_env", 2, &ROSData::points, this);
  input_object = nh.subscribe("object", 2, &ROSData::setObjects, this);
  input_parameters = nh.subscribe("mabx/LowLevelInfo_vis", 2, &ROSData::setParameters, this);
  input_alternative = nh.subscribe("navigation/nav_alternative", 2, &ROSData::setAlternative, this);

  sub_gps = new message_filters::Subscriber<sensor_msgs::NavSatFix>(nh, "gps/fix", 10);
  sub_imu = new message_filters::Subscriber<sensor_msgs::Imu>(nh, "imu/data", 10);
//  sub_acc = new message_filters::Subscriber<geometry_msgs::AccelWithCovarianceStamped>(nh, "linear_acc", 10);
//  sub_pose = new message_filters::Subscriber<geometry_msgs::PoseWithCovarianceStamped>(nh, "pose", 10);
  sub_vel = new message_filters::Subscriber<geometry_msgs::TwistWithCovarianceStamped>(nh, "gps/vel", 10);

  sync = new Synchronizer<MySyncPolicy> (MySyncPolicy(500), *sub_gps, *sub_imu, *sub_vel);
  sync->registerCallback(boost::bind(&ROSData::setGPS, this, _1, _2, _3));

//  sub_fix = new message_filters::Subscriber<sensor_msgs::NavSatFix>(nh, "gps/fix", 10);
//  sub_imu = new message_filters::Subscriber<sensor_msgs::Imu>(nh, "imu/data", 10);
//  sub_vel = new message_filters::Subscriber<geometry_msgs::TwistWithCovarianceStamped>(nh, "gps/vel", 10);
//  timesync = new message_filters::TimeSynchronizer<sensor_msgs::NavSatFix, sensor_msgs::Imu, geometry_msgs::TwistWithCovarianceStamped>(*sub_fix, *sub_imu, *sub_vel, 10);
//  timesync->registerCallback(boost::bind(&ROSData::setGPS, this, _1, _2, _3));


//  typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::NavSatFix, sensor_msgs::Imu, geometry_msgs::TwistWithCovarianceStamped> MySyncPolicy;

  vehicle.x = -3.5;
}

void ROSData::points(const sensor_msgs::PointCloud2::ConstPtr &cloud){
 LaserCloudData datum;
 laserCloud.clear();

 for(sensor_msgs::PointCloud2ConstIterator<float>
     X(*cloud, "x"), Y(*cloud, "y"), Z(*cloud, "z"), iter_i(*cloud, "intensity");
     X != X.end();
     ++X, ++Y, ++Z, ++iter_i){
   datum.x = *X;
   datum.y = *Y;
   datum.z = *Z;
   datum.intensity = *iter_i / 4.84f;

   if(datum.intensity > maximum)
    maximum = datum.intensity;

   laserCloud.push_back(datum);
 }
 vehicle.timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
}

void ROSData::setObjects(const visualization_node::object_vec::ConstPtr &obj){
  dynamic_boxes.clear();
  static_boxes.clear();

  dynamic_texts.clear();
  static_texts.clear();

  Obstacle box;
  Obstacle arrow;
  XYZLabel label;
  bool isDynamic(false);
  bool isChanging(false);
  float speed(0);

  box.lineWidth = 0.07f;
  box.positionZ = 0;
  box.height = 0.3f;
  box.arrow = false;

  label.positionZ = 0.3f;

  arrow.positionZ = -0.5f;
  arrow.height = 1.0f;
  arrow.width = 1.0f;
  arrow.r = 0.0f;
  arrow.g = 255.0f;
  arrow.b = 0.0f;
  arrow.lineWidth = 0.0f;

  for(uint i = 0; i < obj->object_vector.size(); i++){

    const visualization_node::object &data = obj->object_vector[i];

    isDynamic = (data.object_flags == 384)? true : false;
    isChanging = (data.object_flags == 448)? true : false;

    box.width = data.sizeY;
    box.length = data.sizeX;

    box.positionX = arrow.positionX = label.positionX = data.centerX;
    box.positionY = arrow.positionY = label.positionY = data.centerY;

    box.yaw = data.object_orient;

    if(isDynamic){
      arrow.yaw = arrow.arrow_yaw = box.yaw + _PI2;
      box.r = label.r = arrow.r;
      box.g = label.g = arrow.g;
      box.b = label.b = arrow.b;

      speed = sqrt(pow(data.absvelX,2) + pow(data.absvelY, 2));
      //change here the speed-length relation:
      arrow.length = speed;
    }else if(isChanging){
      box.r = 0.0f;
      box.g = 0.0f;
      box.b = 255.0f;
      label.r = 40.0f;
      label.g = 80.0f;
      label.b = 255.0f;
    }else{
      box.r = 255.0f;
      box.g = 20.0f;
      box.b = 20.0f;
      label.r = 255.0f;
      label.g = 80.0f;
      label.b = 40.0f;

    }


    if(isDynamic){
      label.label = QString("i:%1 C:%2 A:%3\nQ:%4 V:%5 m/s").arg(data.object_id)
                    .arg(data.object_class).arg(data.object_age).arg(data.object_quali)
                    .arg(speed, 0, 'f', 2, '0');

      dynamic_boxes.push_back(box);
      dynamic_boxes.push_back(arrow);
      dynamic_texts.push_back(label);
    }else{
      label.label = QString("i:%1 C:%2\nA:%3 Q:%4").arg(data.object_id).arg(data.object_class)
                    .arg(data.object_age).arg(data.object_quali);

      static_boxes.push_back(box);
      static_texts.push_back(label);
    }
  }
}

void ROSData::setParameters(const mabx::LowLevelInfo_vis::ConstPtr &info){
  vehicle.gas = info->throttle;
  vehicle.brakes = info->brake;
//  vehicle.battery = info->battery_soc;
  vehicle.rpm = info->engine_rpm;
  vehicle.steeringAngle = info->steering_angle;
  // values missing:
//  vehicle.gearbox;
//  vehicle.timestamp;
//  vehicle.clutch;
}

void ROSData::setGPS(const sensor_msgs::NavSatFix::ConstPtr &gps,
                     const sensor_msgs::Imu::ConstPtr &pose,
                     const geometry_msgs::TwistWithCovarianceStamped::ConstPtr &vel){

  QQuaternion temp(pose->orientation.w, -pose->orientation.y, pose->orientation.z, -pose->orientation.x);
  QVector3D ori = temp.toEulerAngles();

  // latitude
  vehicle.latitude = gps->latitude;
  // longitude
  vehicle.longitude = gps->longitude;
  // altitude
  vehicle.altitude = gps->altitude;
  // pitch
  vehicle.pitch = ori.x() * toRADIANS;
  // yaw
  vehicle.yaw = ori.y() * toRADIANS - _PI2;
  // roll
//  vehicle.roll = -ori.z() * toRADIANS;
//  vehicle.roll = ori.z() * toRADIANS;
  vehicle.roll = 0;
  // velocity x
  vehicle.velocityX = vel->twist.twist.linear.x * toKMPH;
  // velocity y
  vehicle.velocityY = vel->twist.twist.linear.y * toKMPH;
//  qWarning("%.2f %.2f %.2f", vehicle.velocityX, vehicle.velocityY, vehicle.velocityZ);
//  qWarning("%.2f %.2f %.2f", vel->twist.twist.angular.x, vel->twist.twist.angular.y, vel->twist.twist.angular.z);
  // velocity z
  vehicle.velocityZ = vel->twist.twist.linear.z * toKMPH;
  // velocity
  vehicle.velocity = sqrt(pow(vehicle.velocityX, 2) + pow(vehicle.velocityY, 2) + pow(vehicle.velocityZ, 2));
  // acceleration x
  vehicle.accelerationX = 0;
//  vehicle.accelerationX = acc->accel.accel.linear.x;
  // acceleration y
  vehicle.accelerationY = 0;
//  vehicle.accelerationY = acc->accel.accel.linear.y;
  // acceleration z
  vehicle.accelerationZ = 0;
//  vehicle.accelerationZ = acc->accel.accel.linear.z;

  vehicle.timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");

  vehicle.z = vehicle.altitude - frame.altitude;

  QVector2D position(conversor->GPStoXY(vehicle.latitude, vehicle.longitude));

  double x = position.x();
  double y = position.y();
  double z = vehicle.z;
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
  angle -= frame.yaw;
  x = distance * cos(angle);
  y = distance * sin(angle);

  vehicle.x = -x;
  vehicle.y = -y;

//  QVector3D pos(-position.x(), 0, -position.y());
//  QMatrix4x4 rotation;
//  rotation.rotate(QQuaternion::fromEulerAngles((-vehicle.pitch + frame.pitch) * toDEGREES, (vehicle.yaw - frame.yaw) * toDEGREES, (-vehicle.roll + frame.roll) * toDEGREES));
//  pos = rotation * pos;
//  vehicle.x = pos.z();
//  vehicle.y = -pos.x();
//  vehicle.z = vehicle.altitude - frame.altitude - pos.y();
//  vehicle.x = -position.x();
//  vehicle.y = -position.y();
//  vehicle.z = vehicle.altitude - frame.altitude;

  //  qWarning() << vehicle.latitude << vehicle.longitude << vehicle.x << vehicle.y << vehicle.z << vehicle.pitch * toDEGREES << vehicle.yaw * toDEGREES << vehicle.roll * toDEGREES;
//  qWarning() << vehicle.x << vehicle.y << vehicle.z;

  if(!isFrameSet){
    setStaticFrameOrigin(vehicle.latitude, vehicle.longitude, vehicle.altitude, vehicle.pitch, vehicle.yaw, vehicle.roll);
    isFrameSet = true;
//    qWarning() << vehicle.latitude << vehicle.longitude << vehicle.x << vehicle.y << vehicle.z << vehicle.pitch * toDEGREES << vehicle.yaw * toDEGREES << vehicle.roll * toDEGREES;
  }
}

void ROSData::setAlternative(const navigation::nav_alternative::ConstPtr &alt){
  emit alternativeChanged(alt->show);
}

FrameParameters *const ROSData::getFrameValues(){
  return &frame;
}

VehicleParameters *const ROSData::getVehicleValues(){
  return &vehicle;
}

const vector<LaserCloudData> *const ROSData::getCloudData(){
  return &laserCloud;
}

const vector<Obstacle> *const ROSData::getDynamicBoxes(){
  return &dynamic_boxes;
}

const vector<Obstacle> *const ROSData::getStaticBoxes(){
  return &static_boxes;
}

const vector<XYZLabel> *const ROSData::getDynamicTexts(){
  return &dynamic_texts;
}

const vector<XYZLabel> *const ROSData::getStaticTexts(){
  return &static_texts;
}

double *const ROSData::getMaximum(){
  return &maximum;
}

void ROSData::setStaticFrameOrigin(const double _latitude, const double _longitude, const double _altitude, const double _pitch, const double _yaw, const double _roll){
  frame.latitude = _latitude;
  frame.longitude = _longitude;
  frame.altitude = _altitude;
//  frame.pitch = _pitch;
//  frame.yaw = _yaw;
//  frame.roll = _roll;
  frame.pitch = 0;
  frame.yaw = 0;
  frame.roll = 0;
  isFrameSet = true;
}

void ROSData::setConversor(CoordinateConversor *_conversor){
  conversor = _conversor;
}
