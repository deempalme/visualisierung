#ifndef CONSOLE_H
#define CONSOLE_H

#include <QMainWindow>
#include <QDebug>
#include <QFont>
#include <QFontDatabase>
#include <QTimer>
#include <QFrame>
#include <QMatrix4x4>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QScrollArea>

#include "headers/definitions.h"
#include "headers/uielements.h"
#include "headers/databaseconnection.h"
#include "headers/installmaps.h"
#include "headers/optionbuttons.h"
#include "headers/textdraftsman.h"
#include "headers/coordinateconversor.h"

#ifdef ROS_DATA
  #include <ros/ros.h>
  #include <QCloseEvent>
#endif

namespace Ui {
  class Console;
}

class Console : public QMainWindow
{
  Q_OBJECT

public:
  explicit Console(QWidget *parent = 0);
  ~Console();


  /*    ___          ___          ___          ___          ___          ___          ___                            ___          ___
       /\  \        /\  \        /\__\        /\__\        /\  \        /\  \        /\__\                          /\  \        /\  \
      /::\  \      _\:\  \      /:/ _/_      /:/ _/_      /::\  \      |::\  \      /:/ _/_                 ___    /::\  \       \:\  \
     /:/\:\  \    /\ \:\  \    /:/ /\__\    /:/ /\  \    /:/\:\  \     |:|:\  \    /:/ /\__\               /|  |  /:/\:\  \       \:\  \
    /:/ /::\  \  _\:\ \:\  \  /:/ /:/ _/_  /:/ /::\  \  /:/  \:\  \  __|:|\:\  \  /:/ /:/ _/_             |:|  | /:/  \:\  \  ___  \:\  \
   /:/_/:/\:\__\/\ \:\ \:\__\/:/_/:/ /\__\/:/_/:/\:\__\/:/__/ \:\__\/::::|_\:\__\/:/_/:/ /\__\            |:|  |/:/__/ \:\__\/\  \  \:\__\
   \:\/:/  \/__/\:\ \:\/:/  /\:\/:/ /:/  /\:\/:/ /:/  /\:\  \ /:/  /\:\~~\  \/__/\:\/:/ /:/  /          __|:|__|\:\  \ /:/  /\:\  \ /:/  /
    \::/__/      \:\ \::/  /  \::/_/:/  /  \::/ /:/  /  \:\  /:/  /  \:\  \       \::/_/:/  /          /::::\  \ \:\  /:/  /  \:\  /:/  /
     \:\  \       \:\/:/  /    \:\/:/  /    \/_/:/  /    \:\/:/  /    \:\  \       \:\/:/  /           ~~~~\:\  \ \:\/:/  /    \:\/:/  /
      \:\__\       \::/  /      \::/  /       /:/  /      \::/  /      \:\__\       \::/  /                 \:\__\ \::/  /      \::/  /
       \/__/        \/__/        \/__/        \/__/        \/__/        \/__/        \/__/                   \/__/  \/__/        \/__/
        ___          ___          ___                   ___          ___          ___          ___
       /\  \        /\  \        /\__\                 /\  \        /\__\        /\  \        /\__\
      /::\  \      /::\  \      /:/ _/_                \:\  \      /:/ _/_      /::\  \      /:/ _/_
     /:/\:\  \    /:/\:\__\    /:/ /\__\                \:\  \    /:/ /\__\    /:/\:\__\    /:/ /\__\
    /:/ /::\  \  /:/ /:/  /   /:/ /:/ _/_           ___ /::\  \  /:/ /:/ _/_  /:/ /:/  /   /:/ /:/ _/_
   /:/_/:/\:\__\/:/_/:/__/___/:/_/:/ /\__\         /\  /:/\:\__\/:/_/:/ /\__\/:/_/:/__/___/:/_/:/ /\__\
   \:\/:/  \/__/\:\/:::::/  /\:\/:/ /:/  /         \:\/:/  \/__/\:\/:/ /:/  /\:\/:::::/  /\:\/:/ /:/  /
    \::/__/      \::/~~/~~~´  \::/_/:/  /           \::/__/      \::/_/:/  /  \::/~~/~~~´  \::/_/:/  /
     \:\  \       \:\~~\       \:\/:/  /             \:\  \       \:\/:/  /    \:\~~\       \:\/:/  /
      \:\__\       \:\__\       \::/  /               \:\__\       \::/  /      \:\__\       \::/  /
       \/__/        \/__/        \/__/                 \/__/        \/__/        \/__/        \/__/
  */

  // ------------------------------------------------------------------------------------ //
  // ------------- basic variables, do not modify unless you know your moves ------------ //
  // ------------------------------------------------------------------------------------ //
  //to select the company, possible values:
  //  PEM      or    0
  //  DHL      or    1
  //  NOBRAND  or    2
  int whichCompany;
  //shows the laser clouds
  bool onCloud;
  //colorize the laser clouds depending on intensity
  bool onColorize;
  //shows obstacles
  bool onMovables;
  //shows vehicle
  bool onVehicle;
  //shows streets
  bool onStreets;
  //shows signs
  bool onSignalization;
  //shows trajectories
  bool onTrajectory;
  //shows free space ground
  bool onGround;
  //shows background
  bool onSkybox;
  //shows chassis
  bool onChassis;
  //shows mechanical effects on the vehicle
  bool onChassisRebounding;
  //activates the vehicle axes rotation
  bool onAxesRotation;
  //shows solid texture in the vehicle axes
  bool onAxesTexture;
  //shows vehicle axes
  bool onAxes;
  //shows vehicle box
  bool onBox;
  //show the frame coordinates
  bool onFrameCoordinates;
  //keeps the camera rotating with the vehicle
  bool isStaticCamera;
  //keeps the camera static in the ground
  bool isCameraLocked;
  //shows the left menu
  bool onMenu;
  //shows the images from the camera
  bool onCamera;
  //show the text labels in scenary
  bool onLabels;
  //shows or hides graphic elements with exception of "Controls", "Compass", "Cameras" and "Clock"
  bool onGUI;
  // ------------------------------------------------------------------------------------ //
  // ------------------------------ end of basic variables ------------------------------ //
  // ------------------------------------------------------------------------------------ //

  //this is the object handler of the controls, it will allow us to control the animations from the outside;
  const NewControls *const Controls();
  //this will return the address to the camera with id = _ID
  const NewCamera *const Cameras(unsigned int _ID);
  //this is the transformation matrix of the vehicle
  const QMatrix4x4 *const VehicleTransformationMatrix();
  //this is the transformation matrix of the navigation frame
  const QMatrix4x4 *const NavigationFrameTransformationMatrix();
  //this is the transformation matrix of the vehicle frame with an extra 90 degrees at yaw
  const QMatrix4x4 *const Plus90VehicleFrameTransformationMatrix();
  //this is the transformation matrix of the fixed frame
  const QMatrix4x4 *const FixedFrameTransformationMatrix();
  //this is the transformation matrix of the fixed position
  const QMatrix4x4 *const FixedPositionTransformationMatrix();
  //changes the position of the camera, note that is only the location of
  //the camera, the point that the camera is aiming will not be modified
  void ChangeCameraPosition(const float _x = 0, const float _y = 0, const float _z = 12);
  //change the rotation of the camera, changes the upward side of the camera
  //you must introduce the coordinates of a vector, this vector represents
  //the camera orientation, by default is pointing up
  void ChangeCameraUpVector(const float _x = 0, const float _y = 1, const float _z = 0);
  //this specifies the point which the camera is looking
  void ChangeCameraLookingAt(const float _x = 0, const float _y = 0, const float _z = 0);
  //this changes the scenary rotation in euler angles, the angle is in RADIANS
  void ChangeScenaryRotation(const float _rotationX = -0.436332, const float _rotationY = 0, const float _rotationZ = 0);
  //this changes the scenary zoom, maximum = 0.05 : minimum = 15
  void ChangeScenaryZoom(const float _zoom = 1);
  //changes the view to "Top view"
  void setTopView();
  //changes the view to ~"Isometric view"
  void setStandardView();
  //returns the address of the QFrame hosting all QObjects
  QFrame *const innkeeper();

  //-------------------------------- Laser handling --------------------------------------//

  //this will add a new laser and return the new Laser ID,
  //store the ID in a variable if you want to modify or delete this later,
  unsigned int addLaser(const vector<LaserCloudData> *const _laserCloud, const QString _name, const bool _visible = true,
                        const QMatrix4x4 *const _transformationMatrix = nullptr, const float _pointSize = 1, const bool _gradient = true,
                        const bool _colorized = true, const bool _isIntensityHigherThanOne = false);
  //this is if you have a transformation matrix and a solid color for this laser cloud
  unsigned int addLaser(const vector<LaserCloudData> *const _laserCloud, const QString _name, const bool _visible = true,
                        const QMatrix4x4 *const _transformationMatrix = nullptr, const float _pointSize = 1,
                        const float _R = 0, const float _G = 0, const float _B = 0, const bool _gradient = true,
                        const bool _colorized = true, const bool _isIntensityHigherThanOne = false);
  //this is if you have a transformation matrix and a color palette for this laser cloud
  //if you have a color palette for this laser cloud
  //by default are 4: {0.2,0.5,0.7}, {0,1,0}, {1,1,0}, {1,0,0}
  //                   grayish blue   green    yellow    red
  //the maximum size for the color palette must be 10
  unsigned int addLaser(const vector<LaserCloudData> *const _laserCloud, const QString _name, const bool _visible = true,
                        const QMatrix4x4 *const _transformationMatrix = nullptr, const float _pointSize = 1,
                        vector<array<float, 3> > _palette = {}, const bool _gradient = true,
                        const bool _colorized = true, const bool _isIntensityHigherThanOne = false);


  //this will add a new laser and return the new Laser ID using PCL point clouds,
  //store the ID in a variable if you want to modify or delete this later,
  unsigned int addLaser(const vector<pointRGB> *const _laserCloud, const QString _name, const bool _visible = true,
                        const QMatrix4x4 *const _transformationMatrix = nullptr, const float _pointSize = 1,
                        const bool _gradient = true, const bool _colorized = true,
                        const bool _isIntensityHigherThanOne = false);
  //this is if you have a transformation matrix and a solid color for this laser cloud
  unsigned int addLaser(const vector<pointRGB> *const _laserCloud, const QString _name, const bool _visible = true,
                        const QMatrix4x4 *const _transformationMatrix = nullptr, const float _pointSize = 1,
                        const float _R = 0, const float _G = 0, const float _B = 0, const bool _gradient = true,
                        const bool _colorized = true, const bool _isIntensityHigherThanOne = false);
  //this is if you have a transformation matrix and a color palette for this laser cloud
  //if you have a color palette for this laser cloud
  //by default are 4: {0.2,0.5,0.7}, {0,1,0}, {1,1,0}, {1,0,0}
  //                   grayish blue   green    yellow    red
  //the maximum size for the color palette must be 10
  unsigned int addLaser(const vector<pointRGB> *const _laserCloud, const QString _name, const bool _visible = true,
                        const QMatrix4x4 *const _transformationMatrix = nullptr, const float _pointSize = 1,
                        vector<array<float, 3> > _palette = {}, const bool _gradient = true,
                        const bool _colorized = true, const bool _isIntensityHigherThanOne = false);


  //this will add a new laser and return the new Laser ID,
  //store the ID in a variable if you want to modify or delete this later,
  unsigned int addLaser(const vector<pointDAI> *const _laserCloud, const QString _name, const float _pointSize = 1,
                        const float _angularResolutionDEGREES = 1/32, const float _verticalAngleBetweenLayersDEGREES = 0.8,
                        const bool _visible = true, const QMatrix4x4 *const _transformationMatrix = nullptr,
                        const bool _gradient = true, const bool _colorized = true, const bool _isIntensityHigherThanOne = false);
  //this is if you have a transformation matrix and a solid color for this laser cloud
  unsigned int addLaser(const vector<pointDAI> *const _laserCloud, const QString _name, const float _pointSize = 1,
                        const float _angularResolutionDEGREES = 1/32, const float _verticalAngleBetweenLayersDEGREES = 0.8,
                        const bool _visible = true, const QMatrix4x4 *const _transformationMatrix = nullptr,
                        const float _R = 0, const float _G = 0, const float _B = 0,
                        const bool _gradient = true, const bool _colorized = true,
                        const bool _isIntensityHigherThanOne = false);
  //this is if you have a transformation matrix and a color palette for this laser cloud
  //if you have a color palette for this laser cloud
  //by default are 4: {0.2,0.5,0.7}, {0,1,0}, {1,1,0}, {1,0,0}
  //                   grayish blue   green    yellow    red
  //the maximum size for the color palette must be 10
  unsigned int addLaser(const vector<pointDAI> *const _laserCloud, const QString _name, const float _pointSize = 1,
                        const float _angularResolutionDEGREES = 1/32, const float _verticalAngleBetweenLayersDEGREES = 0.8,
                        const bool _visible = true, const QMatrix4x4 *const _transformationMatrix = nullptr,
                        vector<array<float, 3> > _palette = {}, const bool _gradient = true,
                        const bool _colorized = true, const bool _isIntensityHigherThanOne = false);


  //this is if you want to change the color palette of the laser, this will override the previous color palette
  //returns false if the Laser cloud is not found
  bool setLaserColorPalette(unsigned int _id, vector<array<float, 3> > _palette, const bool _gradient = true);
  //this is if you want to change the solid color, this will override the previous color palette
  //returns false if the Laser cloud is not found
  bool setLaserSolidColor(unsigned int _id, const float _R = 0, const float _G = 0, const float _B = 0, const bool _gradient = true);
  //ignores the solid color defined before and uses the color palette
  bool ignoreSolidColor(unsigned int _id, const bool _ignore = true);
  //this is to modify the Laser options
  //returns false if the Laser cloud is not found
  bool setLaserOptions(unsigned int _id, const bool _gradient = true, const bool _colorized = true,
                       const bool _ignoreSolidColor = false, const bool _isIntensityHigherThanOne = false);
  //if the laser sensor is located outside the vehicle's origin then it is necessary
  //to define a transformation matrix. This will overwrite the existing transformation matrix
  //returns false if the Laser cloud is not found
  bool setLaserTransformationMatrix(unsigned int _id, const QMatrix4x4 *const _transformationMatrix);
  //this will translate all the laser points
  //requires the laser ID, it will return false if the laser objects was not found
  bool translateLaserMatrix(unsigned int _id, const float _translationX = 0, const float _translationY = 0, const float _translationZ = 0);
  //this will rotate all the laser points, input in RADIANS
  //requires the laser ID, it will return false if the laser objects was not found
  bool rotateLaserMatrix(unsigned int _id, const float _pitch = 0, const float _yaw = 0, const float _roll = 0);
  //this will update the laser cloud with id = _id
  //returns false if laser cloud not found
  bool updateCloud(unsigned int _id);
  //deletes the Laser, it requires the laser ID,
  //returns false if error deleting
  bool deleteLaser(unsigned int _id);
  //this will delete all laser clouds and free memory, if you create and delete laser clouds dinamically
  //call this method from time to time to increase the program performance,
  //the previous laser clouds' IDs will not work anymore
  void purgeLaserClouds();

  //------------------------------ Obstacle handling -------------------------------------//

  //this will add a new obstacles vector and return the new obstacles ID,
  //store the ID in a variable if you want to delete this later,
  //if it also requires the transformation matrix AS A POINTER of the obstacles
  //this is very useful when you don't want to call setObstacleTransformationMatrix each time
  unsigned int addObstacles(const vector<Obstacle> *const _obstacles, const QString _name, const bool _visible = true,
                            const QMatrix4x4 *const _transformationMatrix = nullptr);
  //set the tranformation matrix AS A POINTER to update dinamically
  //returns false if Obstacle object does not exist
  bool setObstacleTransformationMatrix(unsigned int _id, QMatrix4x4 *_transformationMatrix = nullptr);
  //this will translate all the obstacle
  //requires the obstacle ID, it will return false if the objects was not found
  bool translateObstacleMatrix(unsigned int _id, const float _translationX = 0,
                               const float _translationY = 0, const float _translationZ = 0);
  //this will rotate all the obstacles, input in RADIANS
  //requires the obstacle ID, it will return false if the objects was not found
  bool rotateObstacleMatrix(unsigned int _id, const float _pitch = 0, const float _yaw = 0,
                            const float _roll = 0);
  //deletes the Obstacles vector, it requires the obstacles vector ID
  //returns false if error deleting
  bool deleteObstacles(unsigned int _id);
  //this will delete all Obstacels and free memory, if you create and delete Obstacles dinamically
  //call this method from time to time to increase the program performance,
  //the previous obstacles' IDs will not work anymore
  void purgeObstacles();

  //----------------------------- Cylinders handling ------------------------------------//

  //this will add a new cylinders vector and return the new cylinders ID,
  //store the ID in a variable if you want to delete this later,
  //the lineWidth is in pixels
  //if it also requires the transformation matrix AS A POINTER of the cylinders
  //this is very useful when you don't want to call setCylinderTransformationMatrix each time
  unsigned int addCylinder(const vector<Obstacle> *const _cylinders, const QString _name, const bool _visible = true,
                           const QMatrix4x4 *const _transformationMatrix = nullptr);
  //set the tranformation matrix AS A POINTER to update dinamically
  //returns false if Cylinder object does not exist
  bool setCylinderTransformationMatrix(unsigned int _id, QMatrix4x4 *_transformationMatrix = nullptr);
  //this will translate all the cylinders
  //requires the cylinder ID, it will return false if the cylinder was not found
  bool translateCylinderMatrix(unsigned int _id, const float _translationX = 0,
                               const float _translationY = 0, const float _translationZ = 0);
  //this will rotate all the cylinders, input in RADIANS
  //requires the cylinder ID, it will return false if the cylinder was not found
  bool rotateCylinderMatrix(unsigned int _id, const float _pitch = 0, const float _yaw = 0,
                            const float _roll = 0);
  //deletes the cylinder vector, it requires the cylinder vector ID
  //returns false if error deleting
  bool deleteCylinder(unsigned int _id);
  //this will delete all Cylinders and free memory, if you create and delete Cylinders dinamically
  //call this method from time to time to increase the program performance,
  //the previous cylinders' IDs will not work anymore
  void purgeCylinders();

  //------------------------------ Trajectory handling -----------------------------------//

  //this will add a new trajectory vector and return the new trajectory ID,
  //store the ID in a variable if you want to delete this later,
  //it is not necessary to have a transformation matrix
  unsigned int addTrajectory(const vector<vector<trajectoryData> > *const _trajectory, const QString _name,
                             const bool _visible = true, const QMatrix4x4 *const _transformationMatrix = nullptr);
  //set the tranformation matrix AS A POINTER to update dinamically
  //returns false if trajectory object does not exist
  bool setTrajectoryTransformationMatrix(unsigned int _id,
                                         const QMatrix4x4 *const _transformationMatrix = nullptr);
  //this will translate the trajectories inside the vector
  //requires the trajectory ID, it will return false if the trajectory objects was not found
  bool translateTrajectoryMatrix(unsigned int _id, const float _translationX = 0,
                                 const float _translationY = 0, const float _translationZ = 0);
  //this will rotate all the trajectories inside the vector, input in RADIANS
  //requires the trajectory ID, it will return false if the trajectory objects was not found
  bool rotateTrajectoryMatrix(unsigned int _id, const float _pitch = 0, const float _yaw = 0,
                              const float _roll = 0);
  //this will update the trajectory with id = _id
  //returns false if id was not found
//  bool updateTrajectory(unsigned int _id);
  //this will update all the added trajectories, if the data from some trajectories are not changed, then this will
  //be waste of calculations. Returns false if there is no trajectory elements
  bool updateAllTrajectories();
  //deletes the trajectory with id = _id vector, it requires the trajectory vector ID
  //returns false if error deleting
  bool deleteTrajectory(unsigned int _id);
  //this will delete all trajectories and free memory, if you create and delete trajectories dinamically
  //call this method from time to time to increase the program performance,
  //the previous trajectories' IDs will not work anymore
  void purgeTrajectories();

  //-------------------------------- Ground handling -------------------------------------//

  //the height value will make the squares to be tranformed into cubes,
  //with value 0 they will be squares, and with value 1 they will have the maximum height
  //defined in setGroundMaximumHeight(default = 2.55 meters)
  //the cube color is in RGB, the input is from the range 0 -> 255
  unsigned int addGround(const vector<Boxes> *const _boxes, const QString _name, const bool _visible = true,
                         const bool _drawLines = true, const QMatrix4x4 *const _transformationMatrix = nullptr);
  bool setGroundTransformationMatrix(unsigned int _id,
                                     const QMatrix4x4 *const _transformationMatrix = nullptr);
  //this will draw lines for the ground with id = _id
  //returns false if id not found
  bool setGroundWithLines(unsigned int _id, const bool _drawLines = true);
  //this will translate all the components in the ground
  //returns false if id was not found
  bool translateGround(unsigned int  _id, const float _translationX = 0, const float _translationY = 0,
                       const float _translationZ = 0);
  //this will rotate all the components in the ground, input is in RADIANS
  //returns false if id was not found
  bool rotateGround(unsigned int  _id, const float _pitch = 0, const float _yaw = 0, const float _roll = 0);
  //call this to update the data in the ground
  //returns false if setting is not complete (if *_squareValue is not defined yet)
  bool updateGround(unsigned int _id);
  //this will update all the added grounds, if the data from some grounds are not changed, then this will
  //be waste of calculations. Returns false if there is no ground elements
  bool updateAllGround();
  //deletes the ground with id = _id, it requires the ground ID
  //returns false if error deleting because id not found
  bool deleteGround(unsigned int _id);
  //this will delete all grounds and free memory, if you create and delete grounds dinamically
  //call this method from time to time to increase the program performance,
  //the previous grounds' IDs will not work anymore
  void purgeGround();

  //----------------------------------- Text labels --------------------------------------//

  //this will add text labels in the screen, XYZ are 3D space coordinates
  //returns the id of the label
  unsigned int addLabels(const vector<XYZLabel> *const _labels, const QString _name, const bool _visible = true);
  //if the label's origin is located outside the vehicle's origin then it is necessary
  //to define a transformation matrix. This will overwrite the existing transformation matrix
  //returns false if the Label is not found
  bool setLabelTransformationMatrix(unsigned int _id, const QMatrix4x4 *const _transformationMatrix);
  //this will translate all the label's positions
  //requires the label ID, it will return false if the label object was not found
  bool translateLabelMatrix(unsigned int _id, const float _translationX = 0, const float _translationY = 0, const float _translationZ = 0);
  //this will rotate all the label's positions, input in RADIANS
  //requires the label ID, it will return false if the label object was not found
  bool rotateLabelMatrix(unsigned int _id, const float _pitch = 0, const float _yaw = 0, const float _roll = 0);
  //this will update the data inside the label with id = _id
  //returns false if _id does not exist
  bool updateLabel(unsigned int _id);
  //this will delete the label with id = _id
  //returns false if _id does not exist
  bool deleteLabel(unsigned int _id);
  //if you dinamically add and delete labels in a very constant manner it is neccesary
  //to call this function to free memory, all labels will be deleted
  void purgeLabels();

  //---------------------------------- Maps handling -------------------------------------//

  //this set the position of the map using POINTERS, if the value of the pointer changes then
  //in the next update the map's position will change too,
  //returns false if nothig was found
  bool setMapPosition(double *_latitude, double *_longitude);
  //this will set the range of the GPS map, all the streets to be drawn will be inside this
  //square, the distance is in meters, returns false if nothing was found
  bool setMapRange(unsigned int _width = 150, unsigned int _length = 150);
  //if you want to make transformation matrix to the map you need to define
  //a transformation matrix. This will overwrite the existing transformation matrix
  void setMapTransformationMatrix(QMatrix4x4 *_transformationMatrix = nullptr);
  //this will translate the transformation matrix
  void translateMap(const float _x = 0, const float _y = 0, const float _z = 0);
  //this will rotate the transformation matrix using euler angles (RADIANS)
  void rotateMap(const float _pitch = 0, const float _yaw = 0, const float _roll = 0);
  //this will force the update of the map
  bool forceUpdateMap();
  //if you want to install a new map in format OSM then call this function (the .osm file must be
  //placed inside a folder declared in E_MAPSFOLDER inside headers/parameters.h)
  //see the options panel to see the results
  void checkForNewMapsFiles();
  //this will return the "maps" folder path, that is the folder where you must put the .osm files to install
  QString seeMapFolderPath();

  // --------------------------------------- Vehicle --------------------------------------- //

  //adds only one vehicle
  //returns false if the vehicle was already created
  bool addVehicle(const unsigned int _model = CHASSIS);
  //this is to introduce the vehicle orientation in RADIANS, #important to define the vehicle transformation matrix
  void addVehicleRotation(double *_pitch, double *_yaw, double *_roll);
  //this is to define the vehicle's position in meters the origin point is the origin in the static frame,
  //#important to define the vehicle transformation matrix
  void addVehiclePosition(double *_positionX, double *_positionY, double *_positionZ,
                          double *_latitude, double *_longitude);
  //this is for graphic effects for the vehicle displayed in the scene, if not defined the
  //there will not be any rebounding
  void addVehicleAccelerations(double *_forwardAcceleration, double *_leftwardAcceleration, double *_upwardAcceleration);
  //this is for the wheel and axes rotation, if not defined the tires and axes will be static
  void addVehicleSpeed(double *_speed);
  //this will make the steering wheel to rotate, if not specified will use the _leftwardAcceleration.
  //if also leftwardAcceleration not defined then will be static
  void addVehicleSteeringWheelAngle(double *_steeringAngle);
  //changes the lateral exes length in meters
  void setLateralAxesLength(const float _length = 1.5);
  //changes the tire size
  void setTireSize(const float _diameter = 0.54, const float _width = 0.18);
  //see drawing below for details in the dimension's letter
  void setVehicleDimensions(const float _A = 0.20, const float _B = 0.65, const float _C = 1.15, const float _D = 1.15,
                            const float _E = 0.20, const float _F = 1.50, const float _height = 1.7);
  //this will change the position of all the vehicle parts with exception of the vehicle's coordinate system
  void changeChassisPosition(const float _x = 1.475, const float _y = 0, const float _z = 0);
  //if the vehicle's origin is located inside another 3D object then the 3D model of the coordinates
  //system will be hidden behind that 3D object
  //this will change the position of the 3D model of the coordinate system
  //(it will NOT change the vehicle's origin)
  void moveVehicleOriginsCoordinateSystem(const float _x = 0, const float _y = 0, const float _z = 0.05);

  //               Vehicle chassis:
  //                                                  // DISTANCES:
  //                                       |–––|·E    // end of rear chassis' wheel suspension section to end of rear bumper
  //                        |–––––––––|·D             // chassis' origin to beginning of frontal chassis' wheel suspension section
  //            |–––––––––––|·C                       // end of frontal chassis' wheel suspension section to chassis' origin
  //       |––––|·B                   |––––|·B        // frontal and rear chassis' wheel suspension section
  //   |–––|·A                                        // frontal bumper to chassis' frontal wheel suspension section
  //         _ _                        _ _                     –
  //     // (_·_) ____________________ (_·_) \\      –          |
  //    //____|__/ _____  _____  ____ \__|____\\     |          |
  //    ||__  __  /     ||_____||____\  __  __||     |          |
  //    ||  ||  ||_____/        \\    ||  ||  ||  Lateral
  //    ||  ||  ||_____     *    ))   ||  ||  ||   axes         F  =  chassis width
  //    ||__||__||     \  _____ //____||__||__||  length
  //    ||_______ \_____||_____||____/ _______||     |          |
  //    \\   _|_ \____________________/ _|_   //     |          |
  //     \\ (_·_)           |          (_·_) //      –          |
  //                 chassis origin      |                      –
  //                                tire origin
  //        |–––| <-tire diameter
  //
  //          |–––––– longitudinal ––––––| = B + C + D       1. the "BAR" origin is located at its center
  //                  axis length
  //
  //    |––––––––––– chassis length –––––––––––| = A + 2*B + C + D + E
  //

  // --------------------------------- Coordinate system ---------------------------------- //

  //this will a Coordinate System on screen and does not have any effect in calculations
  //returns the _id of the fixed frame vector
  unsigned int addFixedFrame(const float _x = 0, const float _y = 0, const float _z = 0,
                             const float _pitch = 0, const float _yaw = 0, const float _roll = 0);
  //this will delete the coordinate frame with id = _id
  //return false if not found
  bool deleteFixedFrame(const unsigned int _id);
  //this will delete all added fixed frames, use this from time to time when you add fixed frames dinamically to free memory
  void purgeFixedFrames();

  //the origin will be the first map coordinate that you introduce.
  void setFixedFrameOrigin(double *_latitude, double *_longitude, double *_altitude,
                           double *_pitch = nullptr, double *_yaw = nullptr, double *_roll = nullptr);
  //updates the fixed frame transformation matrix
  void updateFixedFrameTransformationMatrix();
  //updates the vehicle transformation matrix
  void updateVehicleTransformationMatrix();

  // ------------------------------------------------------------------------------------ //
  // ----------------------------------- GUI Handling ----------------------------------- //
  // ------------------------------------------------------------------------------------ //

  /*
  Alignments to choose for any element:
    LEFT
    RIGTH
    TOP
    BOTTOM
    CENTER (for vertical or horizontal)


  FOR ALL GRAPHIC ELEMENTS:
    GE = Graphic element

    _name       : indicates the name to show at the top of the GE

    _width      : indicates the width of the GE
                  The values can be positives bigger than one for absoulte size, _width = 200 indicates 200 pixel in size.
                  Bigger than 0 and inferior or equals to 1 to percentual size, _width = 0.6 indicates 60% window's size.
                  Inferior than 0 to indicate window's size minus _width

    _height     : indicates the height of the GE
                  The values can be positives bigger than one to absoulte size, _height = 200 indicates 200 pixel in size.
                  Bigger than 0 and inferior or equals to 1 to percentual size, _height = 0.6 indicates 60% window's size.
                  Inferior than 0 to indicate window's size minus _height

    _positionX  : indicates the position relative to the horizontal alignment, if _horizontalAlignment = LEFT and
                  _positionX = 100, then the GE will be separated 100 pixels from the LEFT.
    _positionY  : indicates the position relative to the vertical alignment, if _horizontalAlignment = BOTTOM and
                  _positionY = 50, then the GE will be separated 50 pixels from the BOTTOM
    _horizontalAlignment  : indicates the horizontal aligment, see possible values at the start of "GUI Handling"
    _verticalAlignment    : indicates the vertical aligment, see possible values at the start of "GUI Handling"

  ------------------------------------ Vertical Bar ------------------------------------//

    _decimals   : indicates the number of decimals to show (not all elements have this)
    _max        : indicates the maximum value, if the GE is a bar then the level's size will be (input - min)/(max - min)
    _min        : indicates the minimum value
    _input      : indicates the input values, if _max = 1, _min = 0 and _input = 0.25 then the level is at 25%
  */

  //returns the vertical Bar ID, use this ID to modify or delete elements
  unsigned int addVerticalBar(const QString _name = "", const int _decimals = 0, const double _max = 1, const double _min = 0,
                              double *_input = nullptr, const float _positionX = 0, const float _positionY = 0,
                              const float _horizontalAlignment = LEFT, const float _verticalAlignment = BOTTOM);
  //returns false if element does not exist
  bool changeVerticalBarName(const unsigned int _ID, const QString _name);
  //returns false if element does not exist
  bool changeVerticalBarInput(const unsigned int _ID, double *_input);
  //returns false if element does not exist
  bool changeVerticalBarPosition(const unsigned int _ID, const float _positionX = UNCHANGED, const float _positionY = UNCHANGED);
  //returns false if element does not exist
  bool changeVerticalBarAlignment(const unsigned int _ID, const float _horizontalAlignment = UNCHANGED,
                                  const float _verticalAlignment = UNCHANGED);
  //returns false if element does not exist
  bool changeVerticalBarMaxMin(const unsigned int _ID, const double _max = UNCHANGED, const double _min = UNCHANGED);
  //returns false if element does not exist
  bool updateVerticalBar(const unsigned int _ID);
  //returns false if element does not exist
  bool deleteVerticalBar(const unsigned int _ID);

  //----------------------------------- Steering wheel ------------------------------------//

  //  _decimals   : indicates the number of decimals to show (not all elements have this)
  //  _input      : indicates the input angle in RADIANS (they will converted into degrees)

  //returns the steering wheel ID, use this ID to modify or delete elements
  unsigned int addSteering(const QString _name = "", const int _decimals = 0, double *_input = nullptr,
                           const float _positionX = 0, const float _positionY = 0, const float _horizontalAlignment = LEFT,
                           const float _verticalAlignment = BOTTOM);
  //returns false if element does not exist
  bool changeSteeringName(const unsigned int _ID, const QString _name);
  //returns false if element does not exist
  bool changeSteeringInput(const unsigned int _ID, double *_input);
  //returns false if element does not exist
  bool changeSteeringPosition(const unsigned int _ID, const float _positionX = UNCHANGED, const float _positionY = UNCHANGED);
  //returns false if element does not exist
  bool changeSteeringAlignment(const unsigned int _ID, const float _horizontalAlignment = UNCHANGED,
                               const float _verticalAlignment = UNCHANGED);
  //returns false if element does not exist
  bool changeSteeringMaxMin(const unsigned int _ID, const double _max = UNCHANGED, const double _min = UNCHANGED);
  //returns false if element does not exist
  bool updateSteering(const unsigned int _ID);
  //returns false if element does not exist
  bool deleteSteering(const unsigned int _ID);

  //-------------------------------------- Battery --------------------------------------//

  //  _decimals   : indicates the number of decimals to show (not all elements have this)
  //  _max        : indicates the maximum value
  //  _min        : indicates the minimum value, if the GE is a battery then the level's size will be (input - min)/(max - min)
  //  _input      : indicates the input values, if _max = 400, _min = 100 and _input = 250 then the level is at 50%

  //returns the battery ID, use this ID to modify or delete elements
  unsigned int addBattery(const QString _name = "", const int _decimals = 0, const double _max = 1, const double _min = 0,
                           double *_input = nullptr, const float _positionX = 0, const float _positionY = 0,
                           const float _horizontalAlignment = LEFT, const float _verticalAlignment = BOTTOM);
  //returns false if element does not exist
  bool changeBatteryName(const unsigned int _ID, const QString _name);
  //returns false if element does not exist
  bool changeBatteryInput(const unsigned int _ID, double *_input);
  //returns false if element does not exist
  bool changeBatteryPosition(const unsigned int _ID, const float _positionX = UNCHANGED, const float _positionY = UNCHANGED);
  //returns false if element does not exist
  bool changeBatteryAlignment(const unsigned int _ID, const float _horizontalAlignment = UNCHANGED,
                               const float _verticalAlignment = UNCHANGED);
  //returns false if element does not exist
  bool changeBatteryMaxMin(const unsigned int _ID, const double _max = UNCHANGED, const double _min = UNCHANGED);
  //returns false if element does not exist
  bool updateBattery(const unsigned int _ID);
  //returns false if element does not exist
  bool deleteBattery(const unsigned int _ID);

  //------------------------------------ Speedometer ------------------------------------//

  //  _maxSpeed   : indicates the maximum speed
  //  _minSpeed   : indicates the minimum speed
  //  _maxRPM     : indicates the maximum RPM
  //  _minRPM     : indicates the minimum RPM
  //  _inputSpeed : indicates the input speed, if _maxSpeed = 150, _minSpeed = 0 and _inputSpeed = 80 then the needle is at 53%
  //  _inputRPM   : indicates the input RPM, if _maxRPM = 7000, _minRPM = 0 and _inputRPM = 1200 then the needle is at 17%

  //returns the speedometer ID, use this ID to modify or delete elements
  unsigned int addSpeedometer(const QString _name = "", const double _maxSpeed = 100, const double _minSpeed = 0,
                              const double _maxRPM = 6000, const double _minRPM = 0, double *_inputSpeed = nullptr,
                              double *_inputRPM = nullptr, const float _positionX = 0, const float _positionY = 0,
                              const float _horizontalAlignment = LEFT, const float _verticalAlignment = BOTTOM);
  //returns false if element does not exist
  bool changeSpeedometerName(const unsigned int _ID, const QString _name);
  //returns false if element does not exist
  bool changeSpeedometerInput(const unsigned int _ID, double *_inputSpeed = nullptr, double *_inputRPM = nullptr);
  //returns false if element does not exist
  bool changeSpeedometerPosition(const unsigned int _ID, const float _positionX = UNCHANGED, const float _positionY = UNCHANGED);
  //returns false if element does not exist
  bool changeSpeedometerAlignment(const unsigned int _ID, const float _horizontalAlignment = UNCHANGED,
                               const float _verticalAlignment = UNCHANGED);
  //returns false if element does not exist
  bool changeSpeedometerMaxMin(const unsigned int _ID, const double _maxSpeed = UNCHANGED, const double _minSpeed = UNCHANGED,
                               const double _maxRPM = UNCHANGED, const double _minRPM = UNCHANGED);
  //returns false if element does not exist
  bool updateSpeedometer(const unsigned int _ID);
  //returns false if element does not exist
  bool deleteSpeedometer(const unsigned int _ID);

  //------------------------------------- Gearbox -------------------------------------//
  // _type to choose:
  //      AUTOMATIC (indicates automatic transmission)
  //      MANUAL (indicates manual transmission)

  //  _labels     : indicates the labels to show for each speed
  //  _values     : indicates the values for each speed
  //  _input      : indicates the input
  //                if _label = {"R", "1", "2", "3", "4"}, _values = {1, 2, 3, 4, 5} and _input = 3
  //                then the pivot will be placed at the third label Position: "2"

  //returns the gearbox ID, use this ID to modify or delete elements
  unsigned int addGearbox(const int _type = AUTOMATIC, const QString _name = "", vector<nameANDvalue> *_values = nullptr,
                          double *_input = NULL, const float _positionX = 0, const float _positionY = 0,
                          const float _horizontalAlignment = LEFT, const float _verticalAlignment = BOTTOM);
  //returns false if element does not exist
  bool changeGearboxType(const unsigned int _ID, const int _type = AUTOMATIC);
  //returns false if element does not exist
  bool changeGearboxName(const unsigned int _ID, const QString _name);
  //returns false if element does not exist
  bool changeGearboxInput(const unsigned int _ID, double *_input = nullptr);
  //returns false if element does not exist
  bool changeGearboxPosition(const unsigned int _ID, const float _positionX = UNCHANGED,
                             const float _positionY = UNCHANGED);
  //returns false if element does not exist
  bool changeGearboxAlignment(const unsigned int _ID, const float _horizontalAlignment = UNCHANGED,
                               const float _verticalAlignment = UNCHANGED);
  //returns false if element does not exist
  bool changeGearboxLabels(const unsigned int _ID, vector<nameANDvalue> *_values = nullptr);
  //returns false if element does not exist
  bool updateGearbox(const unsigned int _ID);
  //returns false if element does not exist
  bool deleteGearbox(const unsigned int _ID);

  //------------------------------------- Text List -------------------------------------//

  //  _labels         : indicates the labels to show for each _input
  //  _inputs         : indicates the inputs
  //  _inputDecimals  : indicates the quantity of decimal for each input
  //  _inputTerminations : indicates the string terminations for each input
  //                       if _inputTerminations[0] = " meters" then the first input will have " meters" at the end
  //  the size must be the same for the 4 vectors

  //returns the text list ID, use this ID to modify or delete elements
  unsigned int addTextList(const QString _name = "", vector<nameANDdata> *_values = NULL,
                           const float _positionX = 0, const float _positionY = 0, const float _width = 210,
                          const float _height = 150, const float _horizontalAlignment = LEFT,
                          const float _verticalAlignment = BOTTOM);
  //returns false if element does not exist
  bool changeTextListName(const unsigned int _ID, const QString _name);
  //returns false if element does not exist
  bool changeTextListInputs(const unsigned int _ID, vector<nameANDdata> *values = NULL);
  //returns false if element does not exist
  bool changeTextListPosition(const unsigned int _ID, const float _positionX = UNCHANGED,
                              const float _positionY = UNCHANGED);
  //returns false if element does not exist
  bool changeTextListAlignment(const unsigned int _ID, const float _horizontalAlignment = UNCHANGED,
                               const float _verticalAlignment = UNCHANGED);
  //returns false if element does not exist
  bool changeTextListSize(const unsigned int _ID, const float _width = UNCHANGED,
                          const float _height = UNCHANGED);
  //returns false if element does not exist
  bool updateTextList(const unsigned int _ID);
  //returns false if element does not exist
  bool deleteTextList(const unsigned int _ID);

  //------------------------------------- Icon List -------------------------------------//

  //  _labels  : indicates the labels to show at the icon list
  //  _inputs  : indicates the types of icon, true = green icon, false = red icon

  //returns the icon list ID, use this ID to modify or delete elements
  unsigned int addIconList(const QString _name = "", vector<nameANDboolen> *_values = NULL,
                           const float _positionX = 0, const float _positionY = 0,
                           const float _width = 135, const float _height = 150,
                           const float _horizontalAlignment = LEFT, const float _verticalAlignment = BOTTOM);
  //returns false if element does not exist
  bool changeIconListName(const unsigned int _ID, const QString _name);
  //returns false if element does not exist
  bool changeIconListInputs(const unsigned int _ID, vector<nameANDboolen> *_values = NULL);
  //returns false if element does not exist
  bool changeIconListPosition(const unsigned int _ID, const float _positionX = UNCHANGED,
                              const float _positionY = UNCHANGED);
  //returns false if element does not exist
  bool changeIconListAlignment(const unsigned int _ID, const float _horizontalAlignment = UNCHANGED,
                               const float _verticalAlignment = UNCHANGED);
  //returns false if element does not exist
  bool changeIconListSize(const unsigned int _ID, const float _width = UNCHANGED,
                          const float _height = UNCHANGED);
  //returns false if element does not exist
  bool updateIconList(const unsigned int _ID);
  //returns false if element does not exist
  bool deleteIconList(const unsigned int _ID);

  //------------------------------------- Message List -------------------------------------//
  // _type to choose:
  //      WARNING_M
  //      ERROR_M
  //      INFO_M
  //      PROPER_M

  //returns the message list ID, use this ID to modify or delete elements
  unsigned int addMessageList(const QString _name = "", const float _positionX = 0, const float _positionY = 0,
                              const float _width = -500, const float _height = 150, const float _horizontalAlignment = LEFT,
                              const float _verticalAlignment = BOTTOM);
  //returns false if element does not exist
  bool changeMessageListName(const unsigned int _ID, const QString _name);
  //returns false if element does not exist
  bool changeMessageListPosition(const unsigned int _ID, const float _positionX = UNCHANGED, const float _positionY = UNCHANGED);
  //returns false if element does not exist
  bool changeMessageListAlignment(const unsigned int _ID, const float _horizontalAlignment = UNCHANGED,
                               const float _verticalAlignment = UNCHANGED);
  //returns false if element does not exist
  bool changeMessageListSize(const unsigned int _ID, const float _width = UNCHANGED, const float _height = UNCHANGED);
  //returns false if element does not exist
  bool addMessageToList(const unsigned int _ID, const QString _message = "", const int _type = WARNING_M);
  //returns false if element does not exist
  bool ClearMessagesAtList(const unsigned int _ID);
  //returns false if element does not exist
  bool deleteMessageList(const unsigned int _ID);

  //------------------------------------- Signaling -------------------------------------//
  //for traffic light choose:
  //    SEMAPHORE         (all lights off)
  //    RED               (red light on)
  //    YELLOW            (yellow light on)
  //    YELLOW_RED        (yellow and red lights on)
  //    YELLOW_GREEN      (yellow and green lights on)
  //    GREEN             (green light on)

  //  for any other kind of signaling use the 3 distinctive numbers from the German code, example:
  //  speed limit signs = DE_274_SPEED_LIMIT then you should use 274 and in _numericalValue place the speed limit

  //returns the signaling ID, use this ID to modify or delete elements
  //check the height! if you place this element above another with a scroll then scrolling
  //will not work if the mouse is inside this element
  unsigned int addSignaling(const float _positionX = 10, const float _positionY = 110, const float _width = 50,
                            const float _height = -250, const float _horizontalAlignment = RIGHT,
                            const float _verticalAlignment = TOP);
  //returns false if element does not exist
  bool changeSignalingPosition(const unsigned int _ID, const float _positionX = UNCHANGED, const float _positionY = UNCHANGED);
  //returns false if element does not exist
  bool changeSignalingAlignment(const unsigned int _ID, const float _horizontalAlignment = UNCHANGED,
                               const float _verticalAlignment = UNCHANGED);
  //returns false if element does not exist
  bool changeSignalingSize(const unsigned int _ID, const float _width = UNCHANGED, const float _height = UNCHANGED);
  //returns false if element does not exist
  bool addSign(const unsigned int _ID, const int _type = SEMAPHORE, const int _numericalValue = 0);
  //returns false if element does not exist
  bool ClearSigns(const unsigned int _ID);
  //returns false if element does not exist
  bool deleteSignaling(const unsigned int _ID);

  //---------------------------------------- Clock ----------------------------------------//

  //  _timestamp  : indicates the time to show at the clock located in the left top corner
  //                format = 0000-12-31 24:60:60.999    // year-month-day hours:minutes:seconds.miliseconds

  //returns the clock ID, use this ID to modify or delete elements
  unsigned int addClock(const QString *const _timestamp = nullptr, const float _positionX = 10, const float _positionY = 10,
                        const float _width = 250, const float _height = 70, const float _horizontalAlignment = LEFT,
                        const float _verticalAlignment = TOP);
  //returns false if element does not exist
  bool changeClockPosition(const unsigned int _ID, const float _positionX = UNCHANGED, const float _positionY = UNCHANGED);
  //returns false if element does not exist
  bool changeClockAlignment(const unsigned int _ID, const float _horizontalAlignment = UNCHANGED,
                               const float _verticalAlignment = UNCHANGED);
  //returns false if element does not exist
  bool changeClockSize(const unsigned int _ID, const float _width = UNCHANGED, const float _height = UNCHANGED);
  //returns false if element does not exist
  bool updateClock(const unsigned int _ID);
  //returns false if element does not exist
  bool deleteClock(const unsigned int _ID);

  //--------------------------------------- Compass ---------------------------------------//

  //returns the clock ID, use this ID to modify or delete elements
  unsigned int addCompass(const float _positionX = 0, const float _positionY = 10, const float _width = 0.7,
                          const float _height = 40, const float _horizontalAlignment = CENTER, const float _verticalAlignment = TOP);
  //returns false if element does not exist
  bool changeCompassPosition(const unsigned int _ID, const float _positionX = UNCHANGED, const float _positionY = UNCHANGED);
  //returns false if element does not exist
  bool changeCompassAlignment(const unsigned int _ID, const float _horizontalAlignment = UNCHANGED,
                               const float _verticalAlignment = UNCHANGED);
  //returns false if element does not exist
  bool changeCompassSize(const unsigned int _ID, const float _width = UNCHANGED, const float _height = UNCHANGED);
  //returns false if element does not exist
  bool deleteCompass(const unsigned int _ID);

  //--------------------------------------- Controls ---------------------------------------//

  //  _initialDataset : indicates the folder numerical number, if you choose _initialDataset = 11 then
  //                        the folder will be 0011_sync, the folder must be placed in the following way:

  //  ∟– KittiData
  //      ∟– raw
  //          ∟– 0001_sync             <- here is the folder (note that the dates and "_drive_" was removed)
  //  ∟– yourExecutableFolder

  //  _initialFrame : indicates the frame number, the size will depend in the number of *.bin files inside:

  //  ∟– KittiData
  //      ∟– raw
  //          ∟– 0001_sync
  //              ∟– velodyne_points
  //                  ∟– data          <- inside here

  //  _initialDelayment : indicates the time delayment in milisecond to advance each frame


  //returns false if the control was already created
  bool addControl(const unsigned int _initialDataset = 0, const unsigned int _initialFrame = 0, const unsigned int _initialDelayment = 70);
  //returns false if _frame is out of range
  bool gotoFrame(const unsigned int _frame = 0);
  //returns false if _dataset does not exist
  bool gotoDataset(const unsigned int _dataset = 0);
  //this will set the frame in the first position
  void firstFrame();
  //this will advance one frame
  void nextFrame();
  //this will go back one frame
  void previousFrame();
  //this will set the frame at the last position
  void lastFrame();
  //set the delayment in miliseconds, this does not play/pause
  void setDelayment(const unsigned int _delay = 70);
  //this start/stop the animation timer
  void play();
  //this will pause the animation timer
  void pause();
  //returns false if element does not exist
  bool deleteControl();

  //--------------------------------------- Camera ---------------------------------------//

  //  _initialDataset : indicates the folder numerical number, if you choose _initialDataset = 11 then
  //                    the folder will be 0011_sync, the folder must be placed in the following way:

  //  ∟– KittiData
  //      ∟– raw
  //          ∟– 0001_sync             <- here is the folder (note that the dates and "_drive_" was removed)
  //  ∟– yourExecutableFolder

  //  _cameraSet : indicates which folder will take the information, available options:
  //      0 : for folder image_00 "CAMERA LEFT - GRAYSCALE"
  //      1 : for folder image_01 "CAMERA RIGHT - GRAYSCALE"
  //      2 : for folder image_02 "CAMERA LEFT - COLOR"
  //      3 : for folder image_03 "CAMERA RIGHT - COLOR"

  //  _initialFrame : indicates the frame number, the size will depend in the number of *.bin files inside:

  //  ∟– KittiData
  //      ∟– raw
  //          ∟– 0001_sync
  //              ∟– image_0 + _cameraSet
  //                  ∟– data          <- inside here

  //returns the ID of the new Camera
  unsigned int addCamera(const QString _name = "Camera 02", const unsigned int _initialDataset = 0,
                         const unsigned int _initialFrame = 0, const unsigned int _cameraSet = 2,
                         const float _width = 1242, const float _height = 375, const float _positionX = 0,
                         const float _positionY = 0, const float _horizontalAlignment = CENTER,
                         const float _verticalAlignment = BOTTOM);
  //changes the name to display in the Camera frame
  //returns false if ID does not exist
  bool changeCameraName(unsigned int _ID, const QString _name);
  //changes the size of the Camera frame
  //returns false if ID does not exist
  bool changeCameraSize(unsigned int _ID, const float _width = 1242, const float _height = 375);
  //changes the position of the Camera frame
  //returns false if ID does not exist
  bool changeCameraPosition(unsigned int _ID, const float _positionX = UNCHANGED, const float _positionY = UNCHANGED);
  //changes the Alignment of the Camera frame
  //returns false if ID does not exist
  bool changeCameraAlignment(unsigned int _ID, const float _horizontalAlignment = UNCHANGED, const float _verticalAlignment = UNCHANGED);
  //updates the image in the camera frame
  //returns false if ID does not exist
  bool updateCamera(unsigned int _ID);
  //deletes the camera frame
  //returns false if ID does not exist
  bool deleteCamera(unsigned int _ID);

  void setCoordinateConversor(CoordinateConversor *_conversor);

  // ------------------------------------------------------------------------------------ //
  // -------------------------------- ROS frame spinner --------------------------------- //
  // ------------------------------------------------------------------------------------ //
#ifdef ROS_DATA
  void setROSspinner(int _ROS_cycle_time_milliseconds = 16);
#endif

public slots:
  //this will force a reading in all the data and show it in scene
  void updateData();
  //this will update the data in the trajectory, it is necessary to have the ID
  bool updateTrajectory(unsigned int _id);
  //this if fro ros nodes
  void updateROSdata();
  void setAlternative(bool _alternative);

private:
  //this function triggers itself when a window resize appends
  void resizeEvent(QResizeEvent *event);
  //resize all the elements in the GUI
  void setInterface();
  //This GUI is using external typographies loaded from a resource file,
  //all the fonts, sizes and styles are defined here
  void setFonts();
  //set the initial configuration of the options menu
  void setOptions();
  //check if the state of a button has changed
  void checkOptions();
  //hides or shows the images from all the cameras
  void hideShowCameras();
  //hide or shows GUI elements
  void hideShowGUIElements();
  //this will raise the options and the menu layers one level
  void raiseF();
  //this will create new buttons in the option's menu for each new laser
  void createNewLaserButton(const unsigned int _id, const QString _name, const bool _visible);
  //this will delete a laser's button with id = _id
  void deleteLaserButton(const unsigned int _id);
  //this will delete all laser buttons
  void purgeLaserButtons();
  //this will create new buttons in the option's menu for each new laser
  void createNewObstacleButton(const unsigned int _id, const QString _name, const bool _visible);
  //this will delete a laser's button with id = _id
  void deleteObstacleButton(const unsigned int _id);
  //this will delete all laser buttons
  void purgeObstacleButtons();
  //this will create new buttons in the option's menu for each new cylinder
  void createNewCylinderButton(const unsigned int _id, const QString _name, const bool _visible);
  //this will delete a cylinder's button with id = _id
  void deleteCylinderButton(const unsigned int _id);
  //this will delete all cylinder buttons
  void purgeCylinderButtons();
  //this will create new buttons in the option's menu for each new cylinder
  void createNewLabelsButton(const unsigned int _id, const QString _name, const bool _visible);
  //this will delete a cylinder's button with id = _id
  void deleteLabelsButton(const unsigned int _id);
  //this will delete all cylinder buttons
  void purgeLabelsButtons();
  //this will create new buttons in the option's menu for each new ground element
  void createNewGroundButton(const unsigned int _id, const QString _name, const bool _visible);
  //this will delete a ground's button with id = _id
  void deleteGroundButton(const unsigned int _id);
  //this will delete all grounds buttons
  void purgeGroundButtons();
  //this will create new buttons in the option's menu for each new trajectory element
  void createNewTrajectoryButton(const unsigned int _id, const QString _name, const bool _visible);
  //this will delete a trajectory's button with id = _id
  void deleteTrajectoryButton(const unsigned int _id);
  //this will delete all trajectory buttons
  void purgeTrajectoryButtons();
  void rearrangeButtons();

  // ------------------------------------------------------------------------------------ //
  // ------------- basic variables, do not modify unless you know your moves ------------ //
  // ------------------------------------------------------------------------------------ //
  Ui::Console *ui;
  //typographies id
  int iQuantico, iUnica, iMontserrat, iRoboto;
  //css styles for options' menu buttons
  QString onCss, btnCss, insCss;
  //timer for the visual fade effect
  QTimer *fader;
  //timer for the visual blink effect
  QTimer *blinker;

  //this contain the frame where all GUI elements must be placed
  QFrame *host;
  //vector to almacenate the external typographies
  QVector<QFont*> fonts;
  int *scenaryRotationY;
  //displaying the options' menu
  bool onOptions;
  //if the controls have been created
  bool createdControl;
  //indicats if the map installation is connected
  bool connected;
  bool previousOnGUI;

  //this is the object handler of the controls, it will allow us to control the animations from the outside;
  NewControls *controls;
  //this is the transformation matrix of the vehicle
  QMatrix4x4 vehicleTransformationMatrix;
  //this is the vehicle's translation matrix
  QMatrix4x4 vehicleTranslationMatrix;
  //this is the transformation matrix of the fixed frame
  QMatrix4x4 fixedFrameTransformationMatrix;
  QMatrix4x4 fixedFrameModel;
  //this matrix has a fixed position in the same spot than the fixed frame but only has a yaw rotation
  QMatrix4x4 fixedPositionTransformationMatrix;
  //this matrix moves together with the vehicle but does not rotates, it points to east
  QMatrix4x4 navigationTransformationMatrix;
  //this matrix moves together with the vehicle but does not rotates, and has plus 90 degrees (is pointing north)
  //created for free space detection visualization
  QMatrix4x4 plus90VehicleTransformationMatrix;

  QVector<NewSimpleElement*> simpleElements;
  QVector<NewSpeedo*> speedos;
  QVector<NewGearbox*> gearboxes;
  QVector<NewTextList*> textlists;
  QVector<NewIconList*> iconlists;
  QVector<NewMessages*> messagelists;
  QVector<NewSignaling*> signs;
  QVector<NewClock*> clocks;
  QVector<NewCompass*> compasses;
  QVector<NewCamera*> cameras;
  QVector<TextDraftsman*> labels;

  double *vehiclePositionX, *vehiclePositionY, *vehiclePositionZ, *vehicle_latitude, *vehicle_longitude;
  double *vehiclePitch, *vehicleYaw, *vehicleRoll;
  double *vehicleSpeed, *vehicleSteeringAngle;
  double *vehicleForwardAcceleration, *vehicleLeftwardAcceleration, *vehicleUpwardAcceleration;

  double *frameLatitude, *frameLongitude, *frameAltitude, previousFrameLatitude, previousFrameLongitude;
  double *framePitch, *frameYaw, *frameRoll;
  double lastFixedPitch, lastFixedYaw, lastFixedRoll;

  QPixmap bckPer, bckCamera, bckIcon, bckIcon2;
  unsigned int iconAngle, previousDataset;
  QPainter ptr;
  QLinearGradient gradient;

  InstallMaps *installator;
  databaseConnection *database;

  QMap<uint, OptionButtons*> laserButtons, objectButtons, cylinderButtons, labelButtons, groundButtons,
  trajectoryButtons;

  QMap<uint, bool> laserActives, objectActives, cylinderActives, labelActives, groundActives,
  trajectoryActives;

  QScrollArea *s_extra;
  QFrame *f_extra;
  // ------------------------------------------------------------------------------------ //
  // ------------------------------ end of basic variables ------------------------------ //
  // ------------------------------------------------------------------------------------ //
  QTimer *ROStimer;
  bool *show_alternative, previous_alternative;

#ifdef ROS_DATA
protected:
  void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
#endif

private slots:
  //options' menu button released event
  void on_b_op_released();
  //top view button released event
  void on_b_tv_released();
  //standard view button released event
  void on_b_sv_released();
  //show laser clouds button released event
  void on_b_lsc_released();
  //colorize laser button released event
  void on_b_ls_color_released();
  //show obstacles button released event
  void on_b_movables_released();
  //show vehicle button released event
  void on_b_vehicle_released();
  //show streets button released event
  void on_b_streets_released();
  //show traffic signals button released event
  void on_b_signals_released();
  //show trajectory button released event
  void on_b_trajectory_released();
  //show free space button released event
  void on_b_ground_released();
  //show skybox button released event
  void on_b_skybox_released();
  //show chassis button released event
  void on_b_chassis_released();
  //rebounding vehicle button released event
  void on_b_rebound_released();
  //show axes button released event
  void on_b_axes_released();
  //rotate axes button released event
  void on_b_rotation_released();
  //solid texture button released event
  void on_b_texture_released();
  //show vehicle box button released event
  void on_b_box_released();
  //lock the camera to the vehicle
  void on_b_svw_released();
  //fix the camera to the ground
  void on_b_fvw_released();
  //hide the menu
  void on_b_hop_released();
  //shows frame coordinates
  void on_b_frame_released();
  //install the osm files in the path defined in E_MAPSFOLDER
  void on_b_install_released();
  //shows the text of found OSM files in the option's menu
  void osmFilesFoundText(uint _quantity);
  //actions to take after all OSM files installed
  void allFilesInstalledActuator();
  //update the OSM files percentage in the option's menu
  void updatePercentage(uint _percentage);
  //shows a message in the option's menu if error reading OSM files happened
  void osmFilesErrorFound(QString _fileName, QString _errorText);
  //rotates the installing icon in the options panel to add a working feeling
  void rotateInstallingImage();
  //shows the images from the cameras
  void on_b_cameras_released();
  //shows the GUI elements
  void on_b_gui_released();
  //this is an event handler when a laser button is released
  void laserButtonReleased(unsigned int _id);
  //this is an event handler when a obstacle button is released
  void obstacleButtonReleased(unsigned int _id);
  //this is an event handler when a cylinder button is released
  void cylinderButtonReleased(unsigned int _id);
  //this is an event handler when a label button is released
  void labelButtonReleased(unsigned int _id);
  //this is an event handler when a ground button is released
  void groundButtonReleased(unsigned int _id);
  //this is an event handler when a trajectory button is released
  void trajectoryButtonReleased(unsigned int _id);
  //shows the textual labels
  void on_b_tags_released();
  //zoom in button
  void on_b_zoomin_released();
  //zoom out button
  void on_b_zoomout_released();

signals:
  void options_trigger();
  void frameChanged();
};


  //         /\ \       /\ \        / /\                /\ \     _    /\ \            /\ \      / /\         /\ \           /\ \   /_/\   / /\
  //        /  \ \     /  \ \      / /  \              /  \ \   /\_\ /  \ \           \ \ \    / /  \       /  \ \         /  \ \  \_\ \ / /  \
  //       / /\ \ \   / /\ \ \    / / /\ \            / /\ \ \_/ / // /\ \ \          /\ \_\  / / /\ \__   / /\ \ \       / /\ \ \ /\_\// / /\ \__
  //      / / /\ \_\ / / /\ \_\  / / /\ \ \          / / /\ \___/ // / /\ \ \        / /\/_/ / / /\ \___\ / / /\ \ \     / / /\ \ \\/_// / /\ \___\
  //     / /_/_ \/_// / /_/ / / / / /  \ \ \        / / /  \/____// / /  \ \_\      / / /    \ \ \ \/___// / /  \ \_\   / / /  \ \_\   \ \ \ \/___/
  //    / /____/\  / / /__\/ / / / /___/ /\ \      / / /    / / // / /    \/_/     / / /      \ \ \     / / /    \/_/  / / /   / / /    \ \ \
  //   / /\____\/ / / /_____/ / / /_____/ /\ \    / / /    / / // / /             / / /   _    \ \ \   / / /          / / /   / / / _    \ \ \
  //  / / /      / / /\ \ \  / /_________/\ \ \  / / /    / / // / /________  ___/ / /__ /_/\__/ / /  / / /________  / / /___/ / / /_/\__/ / /
  // / / /      / / /  \ \ \/ / /_       __\ \_\/ / /    / / // / /_________\/\__\/_/___\\ \/___/ /  / / /_________\/ / /____\/ /  \ \/___/ /
  // \/_/       \/_/    \_\/\_\___\     /____/_/\/_/     \/_/ \/____________/\/_________/ \_____\/   \/____________/\/_________/    \_____\/ bitch

#endif // CONSOLE_H
