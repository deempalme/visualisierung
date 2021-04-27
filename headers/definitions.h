#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <vector>
#include <array>
#include <QString>
#include <QVector2D>
#include <QVector3D>

using std::vector;
using std::array;

struct LaserCloudData{
  float x;          //positionX in meters
  float y;          //positionY in meters
  float z;          //positionZ in meters
  float intensity;  //intensity values from 0 to 255
};

struct Obstacle{
  float width = 1;              //in meters
  float length = 1;             //in meters
  float height = 1;             //in meters
  float positionX = 0;          //in meters LOCATED in the CENTER BOTTOM fo the cube
  float positionY = 0;          //in meters LOCATED in the CENTER BOTTOM fo the cube
  float positionZ = 0;          //in meters LOCATED in the CENTER BOTTOM fo the cube
  float pitch = 0;              //in euler angles (RADIANS)
  float yaw = 0;                //in euler angles (RADIANS)
  float roll = 0;               //in euler angles (RADIANS)
  float r = 0;                  //RED color: range 0->255
  float g = 0;                  //GREEN color: range 0->255
  float b = 0;                  //BLUE color: range 0->255
  float lineWidth = 0.1;        //in meters (recommended : 0.1)
  float arrow_pitch = 0;        //arrow pitch orientation (RADIANS)
  float arrow_yaw = 0;          //arrow yaw orientation (RADIANS)
  float arrow_roll = 0;         //arrow roll orientation (RADIANS)
  bool arrow = true;            //true to draw arrow inside box
};

struct trajectoryData{
  float positionX = 0;          //in meters
  float positionY = 0;          //in meters
  float positionZ = 0;          //in meters
  float r = 0;                  //RED color: range 0->255
  float g = 0;                  //GREEN color: range 0->255
  float b = 0;                  //BLUE color: range 0->255
  float lineWidth = 1;        //in meters (recommended : 0.2)
  float lineType = 0;           //type of texture for the trajectory
};

struct Boxes{
  float width = 1;              //set the box width (in meters)
  float length = 1;             //set the box length (in meters)
  float height = 0;             //set the box height (in meters)
  float positionX = 0;          //in meters
  float positionY = 0;          //in meters
  float positionZ = 0;          //in meters
  float r = 0;                  //RED color: range 0->255
  float g = 0;                  //GREEN color: range 0->255
  float b = 0;                  //BLUE color: range 0->255
};

struct pointRGB{
  float xyz[3];   //xyz coordinates
  float rgb[3];   //rgb color
  float intty;    //intensity
};

struct pointDAI{
  float distance;               //point distance from the sensor
  float angle;                  //point angle position
  float intensity;              //point intensity
  float layer;                  //point layer position
};

struct nameANDvalue{
  QString label;   //label name
  double value;
};

struct nameANDdata{
  QString label;               //name of each value
  double *input;               //address to variable containing the values
  QString label_termination;   //indicates the type of value in string (if value is in meters then you should place "m")
  int decimal_places;          //indicates the quantity of decimal places to show
};

struct nameANDboolen{
  QString label;   //name of each value
  bool *input;     //address to the boolean variable
};

struct XYZLabel{
  float positionX = 0;    //position X of the text label
  float positionY = 0;    //position Y of the text label
  float positionZ = 0;    //position Z of the text label
  float r = 255;          //red color in RGB, range 0->255
  float g = 255;          //green color in RGB, range 0->255
  float b = 255;          //blue color in RGB, range 0->255
  QString label;          //label to display
};

struct ModelDataComplex{
  QVector3D position;
  QVector2D uv;
  QVector3D normal;
  QVector3D tangent;
  QVector3D bitangent;
};

struct ModelDataSimple{
  QVector3D position;
  QVector2D uv;
  QVector3D normal;
};

struct VehicleParameters{
  double latitude        = 0; //value in degrees
  double longitude       = 0; //value in degrees
  double altitude        = 0; //value in meters
  double x               = 0; //value in meters
  double y               = 0; //value in meters
  double z               = 0; //value in meters
  double pitch           = 0; //value in radians
  double yaw             = 0; //value in radians
  double roll            = 0; //value in radians
  double velocityX       = 0; //value in meters per second
  double velocityY       = 0; //value in meters per second
  double velocityZ       = 0; //value in meters per second
  double velocity        = 0; //value in meters per second
  double accelerationX   = 0; //value in meters per square second
  double accelerationY   = 0; //value in meters per square second
  double accelerationZ   = 0; //value in meters per square second
  double acceleration    = 0; //value in meters per square second
  double rpm             = 0; //value in revolutions pre minute
  double gas             = 0; //value from 0 -> 1 (0:not pressed, 1:full pressed)
  double brakes          = 0; //value from 0 -> 1 (0:not pressed, 1:full pressed)
  double clutch          = 0; //value from 0 -> 1 (0:not pressed, 1:full pressed)
  double steeringAngle   = 0; //value in radians
  double battery         = 70; //value from 0 -> 1 (0:emty battery, 1:full batery)
  double gearbox         = 0; //gear position (first gear, second gear, ..., reverse)
  QString timestamp      = "1989-01-13 20:32:12.596"; // format: yyyy-MM-dd HH:mm:ss.zzz
};

struct vector2d{
  double x = 0;
  double y = 0;
};

struct FrameParameters{
  double latitude;
  double longitude;
  double altitude;
  double pitch;
  double yaw;
  double roll;
};

#define _PI                 3.14159265359
#define _2PI                6.28318530718
#define _PI2                1.57079632679
#define _PI4                0.78539816339744830962
#define _3PI2               4.71238898038
#define toDEGREES           57.29577951308
#define toRADIANS           0.01745329252
#define toKMPH              3.6
#define GRAVITY             9.81

#define anglePrecision      16.0f
#define steeringOriginX    -0.7f
#define steeringOriginY    -0.38f
#define steeringOriginZ     0.57f

#define BOTTOM              0
#define TOP                 1
#define LEFT                2
#define RIGHT               3
#define CENTER              4

#define UNCHANGED           0.0f/0.0f

#define BATTERY             0
#define VERTICALBAR         1

#define MANUAL              0
#define AUTOMATIC           1

#define WARNING_M           0
#define ERROR_M             1
#define PROPER_M            2
#define INFO_M              3

#define SEMAPHORE           0
#define GREEN               1
#define YELLOW              2
#define RED                 3
#define YELLOW_RED          4
#define YELLOW_GREEN        5

#define PEM                 0
#define DHL                 1
#define NOBRAND             2

#define TIRE_LEFT           0
#define TIRE_RIGHT          1
#define STEERING            2
#define COORDINATES         3
#define BAR                 4
#define CHASSIS             5
#define DB5                 6
#define WINDOWS             7
#define SHUTTLE             8
#define SHUTTLE_CHASSIS     9
#define SHUTTLE_WINDOWS     10
#define STREET_SCOOTER      11
#define CAMPUS_BOULEVAR     12

#define FRONTAL_LEFT_TIRE   0
#define FRONTAL_RIGHT_TIRE  1
#define REAR_LEFT_TIRE      4
#define REAR_RIGHT_TIRE     8
#define CENTRAL_AXIS        9
#define FRONTAL_AXIS        10
#define REAR_AXIS           11

#endif // DEFINITIONS_H
