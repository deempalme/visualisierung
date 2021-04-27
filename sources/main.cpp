#include <QApplication>
#include <QObject>

//define this for non-runnnig-examples application
//#define NO_EXAMPLES
//define this to non-GUI-graphic-examples application
//#define NO_GUI_EXAMPLES
//define to indicate ROS data
//#define ROS_DATA

//this include is for GUI : #important
#include "headers/parameters.h"
#include "headers/console.h"
#include "headers/coordinateconversor.h"
#include "headers/navigationTrajectory.h"

#ifdef ROS_DATA
  #include <ros/ros.h>

  //introduce here the position of the laser sensor
  #define VELODYNE_POSITION_X         0
  #define VELODYNE_POSITION_Y         0
  #define VELODYNE_POSITION_Z         0

  #include "extras/headers/rosdata.h"

  #define ROS_CYCLE_TIME_MILLISECONDS 20
#else
  //these are for data obtainment
  #include "extras/headers/kittiDataIMU.h"
  #include "extras/headers/kittiLaserData.h"
  #include "extras/headers/kittiObstacles.h"
  #include "extras/headers/kittiTrajectory.h"
  #include "extras/headers/kittiTextLabels.h"

  #define INITIAL_DATASET       5
  #define INITIAL_FRAME         0
  #define INITIAL_CAMERA        2

  #ifndef NO_EXAMPLES
    #define VELODYNE_POSITION_X   1.6
  #else
    #define VELODYNE_POSITION_X   0.76
  #endif
  #define VELODYNE_POSITION_Y   0
  #define VELODYNE_POSITION_Z   1.46
#endif

#define TIRE_AXIS_TO_GROUND  -0.33

int main(int argc, char *argv[]){


  /*   ___          ___          ___                   ___          ___          ___          ___          ___          ___   ___          ___
      /\  \        /\  \        /\  \                 /\  \        /\  \        /\__\        /\  \        /\  \        /\__\ /\  \         /\__\
     /::\  \      /::\  \      /::\  \               /::\  \      /::\  \      /::|  |      /::\  \      /::\  \      /:/  //::\  \       /:/  /
    /:/\ \  \    /:/\:\  \    /:/\:\  \             /:/\:\  \    /:/\:\  \    /:|:|  |     /:/\ \  \    /:/\:\  \    /:/  //:/\:\  \     /:/__/
   _\:\~\ \  \  /::\~\:\  \  /::\~\:\  \           /:/  \:\  \  /:/  \:\  \  /:/|:|  |__  _\:\~\ \  \  /:/  \:\  \  /:/  //::\~\:\  \   /::\  \ ___
  /\ \:\ \ \__\/:/\:\ \:\__\/:/\:\ \:\__\         /:/__/ \:\__\/:/__/ \:\__\/:/ |:| /\__\/\ \:\ \ \__\/:/__/ \:\__\/:/__//:/\:\ \:\__\ /:/\:\  /\__\
  \:\ \:\ \/__/\:\~\:\ \/__/\:\~\:\ \/__/         \:\  \  \/__/\:\  \ /:/  /\/__|:|/:/  /\:\ \:\ \/__/\:\  \ /:/  /\:\  \\:\~\:\ \/__/ \/__\:\/:/  /
   \:\ \:\__\   \:\ \:\__\   \:\ \:\__\            \:\  \       \:\  /:/  /     |:/:/  /  \:\ \:\__\   \:\  /:/  /  \:\  \\:\ \:\__\        \::/  /
    \:\/:/  /    \:\ \/__/    \:\ \/__/             \:\  \       \:\/:/  /      |::/  /    \:\/:/  /    \:\/:/  /    \:\  \\:\ \/__/  __    /:/  /
     \::/  /      \:\__\       \:\__\                \:\__\       \::/  /       /:/  /      \::/  /      \::/  /      \:\__\\:\__\   /\_\  /:/  /
      \/__/        \/__/        \/__/                 \/__/        \/__/        \/__/        \/__/        \/__/        \/__/ \/__/   \/_/  \/__/
       ___          ___          ___                   ___          ___          ___          ___                              ___          ___          ___
      /\  \        /\  \        /\  \                 /\__\        /\  \        /\  \        /\  \                  ___       /\__\        /\  \        /\  \
     /::\  \      /::\  \      /::\  \               /::|  |      /::\  \      /::\  \      /::\  \                /\  \     /::|  |      /::\  \      /::\  \
    /:/\:\  \    /:/\:\  \    /:/\:\  \             /:|:|  |     /:/\:\  \    /:/\:\  \    /:/\:\  \               \:\  \   /:|:|  |     /:/\:\  \    /:/\:\  \
   /::\~\:\  \  /:/  \:\  \  /::\~\:\  \           /:/|:|__|__  /:/  \:\  \  /::\~\:\  \  /::\~\:\  \              /::\__\ /:/|:|  |__  /::\~\:\  \  /:/  \:\  \
  /:/\:\ \:\__\/:/__/ \:\__\/:/\:\ \:\__\         /:/ |::::\__\/:/__/ \:\__\/:/\:\ \:\__\/:/\:\ \:\__\          __/:/\/__//:/ |:| /\__\/:/\:\ \:\__\/:/__/ \:\__\
  \/__\:\ \/__/\:\  \ /:/  /\/_|::\/:/  /         \/__/~~/:/  /\:\  \ /:/  /\/_|::\/:/  /\:\~\:\ \/__/         /\/:/  /   \/__|:|/:/  /\/__\:\ \/__/\:\  \ /:/  /
       \:\__\   \:\  /:/  /    |:|::/  /                /:/  /  \:\  /:/  /    |:|::/  /  \:\ \:\__\           \::/__/        |:/:/  /      \:\__\   \:\  /:/  /
        \/__/    \:\/:/  /     |:|\/__/                /:/  /    \:\/:/  /     |:|\/__/    \:\ \/__/            \:\__\        |::/  /        \/__/    \:\/:/  /
                  \::/  /      |:|  |                 /:/  /      \::/  /      |:|  |       \:\__\               \/__/        /:/  /                   \::/  /
                   \/__/        \|__|                 \/__/        \/__/        \|__|        \/__/                            \/__/                     \/__/
       ___          ___          ___                   ___          ___                   ___          ___          ___
      /\  \        /\__\        /\  \                 /\  \        /\  \                 /\  \        /\  \        /\  \
     /::\  \      /::|  |      /::\  \               /::\  \      /::\  \               /::\  \      /::\  \      /::\  \
    /:/\:\  \    /:|:|  |     /:/\:\  \             /:/\:\  \    /:/\:\  \             /:/\:\  \    /:/\:\  \    /:/\:\  \
   /::\~\:\  \  /:/|:|  |__  /:/  \:\__\           /:/  \:\  \  /:/  \:\  \           /::\~\:\  \  /::\~\:\  \  /:/  \:\  \
  /:/\:\ \:\__\/:/ |:| /\__\/:/__/ \:|__|         /:/__/_\:\__\/:/__/ \:\__\         /:/\:\ \:\__\/:/\:\ \:\__\/:/__/ \:\__\
  \/__\:\/:/  /\/__|:|/:/  /\:\  \ /:/  /         \:\  /\ \/__/\:\  \ /:/  /         \/__\:\/:/  /\/_|::\/:/  /\:\  \ /:/  /
       \::/  /     |:/:/  /  \:\  /:/  /           \:\ \:\__\   \:\  /:/  /               \::/  /    |:|::/  /  \:\  /:/  /
       /:/  /      |::/  /    \:\/:/  /             \:\/:/  /    \:\/:/  /                 \/__/     |:|\/__/    \:\/:/  /
      /:/  /       /:/  /      \::/__/               \::/  /      \::/  /                            |:|  |       \::/  /
      \/__/        \/__/        ~~                    \/__/        \/__/                              \|__|        \/__/
  */

  //now you have the OPTION to hide the GUI elements from the option PANEL or press "P"
  //const bool GUIShowMenu = false;

  //-------------------------------------------------------------------------------------------------------//
  //                                 OpenGL configuration and GUI creation                                 //
  //-------------------------------------------------------------------------------------------------------//

  #ifdef ROS_DATA
    ros::init(argc, argv, "francis");
  #endif

  //this creates the application
  QApplication app(argc, argv);
  app.setAttribute(Qt::AA_UseDesktopOpenGL);
  //this creates our GUI, our GUI object is called now "GUI"
  Console GUI;
  //maximized window state
  //GUI.showMaximized();
  //regular window size
  GUI.show();

  //-------------------------------------------------------------------------------------------------------//
  //                                            obtaining the data                                         //
  //-------------------------------------------------------------------------------------------------------//

  #ifndef ROS_DATA
    //creating an object to read the IMU data
    kittiDataIMU oxts;

    CoordinateConversor conversor(&oxts.frameLatitude, &oxts.frameLongitude);
    oxts.setConversor(&conversor);
    GUI.setCoordinateConversor(&conversor);

    #ifdef EGOIST
      NavigationTrajectory trajectory("/home/egoist/Dokumente/trajectories/", &conversor);
    #else
      //NavigationTrajectory trajectory("/home/franciscoramirez/Documents/trajectories/", &conversor);
    #endif

    //creating the object to obtain Laser data
    kittiLaserData cloudy1;
    //adding the laserCloud to the scenary, the id will serve later to
    //modify or delete this cloud
    unsigned int id_laser1 = GUI.addLaser(cloudy1.LaserCloud(), "LASER 1", true, GUI.VehicleTransformationMatrix(), 1, true);
    GUI.translateLaserMatrix(id_laser1, VELODYNE_POSITION_X, VELODYNE_POSITION_Y, VELODYNE_POSITION_Z);

    //creating a new object to read the obstacels from
    //the tracklets file in kitti data
    kittiObstacles obstacle(&conversor, true, &oxts.vehicle.latitude, &oxts.vehicle.longitude,
                            &oxts.vehicle.altitude, &oxts.vehicle.pitch, &oxts.vehicle.yaw, &oxts.vehicle.roll);
    unsigned int id_obstacle1 = GUI.addObstacles(obstacle.getObstacles(), "KITTI OBSTACLES", true, GUI.VehicleTransformationMatrix());
    GUI.translateObstacleMatrix(id_obstacle1, VELODYNE_POSITION_X, VELODYNE_POSITION_Y, VELODYNE_POSITION_Z);

    kittiTrajectoryLoader kTrajectoryLoader("../KittiData/");
    unsigned int id_traject1 = GUI.addTrajectory(kTrajectoryLoader.GetTrajectories(), "MAGNUS TRAJECTORY", true);
//    unsigned int id_traject1 = GUI.addTrajectory(trajectory.GetTrajectories(), "TESTING TRAJECTORY", true, GUI.FixedFrameTransformationMatrix());
    kTrajectoryLoader.setGUIId(id_traject1);
    GUI.translateTrajectoryMatrix(id_traject1, 0, 0, TIRE_AXIS_TO_GROUND);

    kittiTextLabels labels("labels");
    unsigned int id_labels = GUI.addLabels(labels.textLabels(), "KITTI LABELS", true);
    GUI.translateLabelMatrix(id_labels, VELODYNE_POSITION_X, VELODYNE_POSITION_Y, VELODYNE_POSITION_Z);

    //-------------------------------------------------------------------------------------------------------//
    //                                        Creating graphic elements                                      //
    //-------------------------------------------------------------------------------------------------------//

    #ifndef NO_EXAMPLES
      #ifndef NO_GUI_EXAMPLES
        //camera creation
        unsigned int id_camera1 = GUI.addCamera("CAMERA 02 - LEFT", INITIAL_DATASET, INITIAL_FRAME, INITIAL_CAMERA, 1242, 375, 0, 120, CENTER);
      #else
        //camera creation
        unsigned int id_camera1 = GUI.addCamera("CAMERA 02 - LEFT", INITIAL_DATASET, INITIAL_FRAME, INITIAL_CAMERA, 1242, 375, 0, 30, CENTER);
      #endif
      //this is to hide/show the camera
      GUI.onCamera = false;
    #else
      //camera creation
      unsigned int id_camera1 = GUI.addCamera("CAMERA 02 - LEFT", INITIAL_DATASET, INITIAL_FRAME, INITIAL_CAMERA, 1242, 375, 0, 30, CENTER);
    #endif
    //it is possible to scale, but is not recommended for high performance applications
    GUI.changeCameraSize(id_camera1, 800, 242);

    #ifndef NO_GUI_EXAMPLES
      //this is for a manual transmission, see structure definitions in "headers/definitions.h"
      vector<nameANDvalue> gearbox_values(6);
      gearbox_values[0] = { "1", -2 };
      gearbox_values[1] = { "2", -1 };
      gearbox_values[2] = { "3",  0 };
      gearbox_values[3] = { "4",  1 };
      gearbox_values[4] = { "5",  2 };
      gearbox_values[5] = { "6",  3 };
      gearbox_values.push_back({ "R",  4 });

      //this is for the General status graphic element, see structure definitions in "headers/definitions.h"
      vector<nameANDdata> textList_values(3);
      textList_values[0] = { "LAT", &oxts.vehicle.latitude, "°", 10 };
      textList_values[1] = { "LON", &oxts.vehicle.longitude, "°", 10 };
      textList_values[2] = { "ALT", &oxts.vehicle.altitude, " m", 4 };

      //this is for the Sensor status
      //variables for the sensor status
      bool isins = false, ispos = true, isvel = true, isalt = true, istat = false;
      vector<nameANDboolen> iconStatus_values(5);
      iconStatus_values[0] = { "INS", &isins };
      iconStatus_values[1] = { "GPS POS", &ispos };
      iconStatus_values[2] = { "GPS VEL", &isvel };
      iconStatus_values[3] = { "GPS ALT", &isalt };
      iconStatus_values[4] = { "STATUS", &istat };

      unsigned int id_brakes   = GUI.addVerticalBar("BRAKES", 1, 1, 0, &oxts.vehicle.brakes, 385, 0, LEFT, BOTTOM);
      unsigned int id_gas      = GUI.addVerticalBar("GAS", 1, 1, 0, &oxts.vehicle.gas, 455, 0, LEFT, BOTTOM);
      unsigned int id_steering = GUI.addSteering("STEERING", 1, &oxts.vehicle.steeringAngle, 0, 0, LEFT, BOTTOM);
      unsigned int id_battery  = GUI.addBattery("BATTERY", 0, 100, 0, &oxts.vehicle.battery, 180, 0, LEFT);
      unsigned int id_speedo   = GUI.addSpeedometer("SPEED AND RPM", 150, 0, 6000, 0, &oxts.vehicle.velocity, &oxts.vehicle.rpm, 250, 0, LEFT, BOTTOM);
      unsigned int id_gearbox  = GUI.addGearbox(MANUAL, "GEARBOX", &gearbox_values, &oxts.vehicle.gearbox, 110, 0, LEFT, BOTTOM);
      unsigned int id_status   = GUI.addTextList("GENERAL STATUS", &textList_values, 530, 0, 210, 150, LEFT, BOTTOM);
      unsigned int id_sensors  = GUI.addIconList("SENSOR STATUS", &iconStatus_values, 760, 0, 135, 150, LEFT, BOTTOM);

      unsigned int id_messages = GUI.addMessageList("GENERAL MESSAGES", 915, 0, -915, 150, LEFT, BOTTOM);
      GUI.addMessageToList(id_messages, "Test message number 1", WARNING_M);
      GUI.addMessageToList(id_messages, "Test message number 2", ERROR_M);
      GUI.addMessageToList(id_messages, "Test message number 3", INFO_M);
      GUI.addMessageToList(id_messages, "Test message number 4", PROPER_M);

      unsigned int id_signaling = GUI.addSignaling();
      GUI.addSign(id_signaling, GREEN);      //semaphore's green light
      GUI.addSign(id_signaling, 274, 40);    //274 is the european code for speed signalization
      GUI.addSign(id_signaling, 306);        //306 is the european code for priority road
    #endif

    unsigned int id_clock   = GUI.addClock(cloudy1.Timestamp());
    unsigned int id_compass = GUI.addCompass(0, 5, 0.6);
    //this is very important if you dont introduce this values then the vehicle's
    //transformation matrix will always be a identity matrix
    GUI.addVehicleRotation(&oxts.vehicle.pitch, &oxts.vehicle.yaw, &oxts.vehicle.roll);

    //you must establish the vehicle position in meters with origin in the static frame origin
    //to generate the proper transformation matrix
    GUI.addVehiclePosition(&oxts.vehicle.x, &oxts.vehicle.y, &oxts.vehicle.z,
                           &oxts.vehicle.latitude, &oxts.vehicle.longitude);

    //this is just to vehicle mechanical movements in scene:
    GUI.addVehicleAccelerations(&oxts.vehicle.accelerationX, &oxts.vehicle.accelerationY, &oxts.vehicle.accelerationZ);

    GUI.addVehicleSpeed(&oxts.vehicle.velocity);

    //there is not steering angle so it is introduced the leftward acceleration, this is not necessary
    //because if no steering angle is defined it will use automatically the leftward acceleration
    //previously defined, the next line was introduced just has an example.
    GUI.addVehicleSteeringWheelAngle(&oxts.vehicle.steeringAngle);

    //adding the vehicle into scene
    GUI.addVehicle(SHUTTLE);
    GUI.setFixedFrameOrigin(&oxts.frameLatitude, &oxts.frameLongitude, &oxts.frameAltitude,
                            &oxts.framePitch, &oxts.frameYaw, &oxts.frameRoll);

    //GUI.setMapPosition(&oxts.values_IMU[0], &oxts.values_IMU[1]);
    //GUI.translateMap(0, 0, TIRE_AXIS_TO_GROUND);

    //this will create the controls and set the 0011 kitti dataset
    GUI.addControl(INITIAL_DATASET, INITIAL_FRAME);
    //this will play the animation
//    GUI.play();

    // ----------------------------------------------------------- //
    // ------------------ THE COORDINATE SYSTEM ------------------ //
    // ----------------------------------------------------------- //
    //                                                             //
    //                               Z                             //
    //                               |     X                       //
    //                               |    /                        //
    //                               |   /                         //
    //                               |  /                          //
    //                               | /                           //
    //                               |/                            //
    //                 Y ––––––––––– o –––––––––––– -Y             //
    //                              /|                             //
    //                             / |                             //
    //                            /  |                             //
    //                           /   |                             //
    //                          /    |                             //
    //                        -X     |                             //
    //                              -Z                             //
    //                                                             //
    // ----------------------------------------------------------- //
    // ------------------ THE COORDINATE SYSTEM ------------------ //
    // ----------------------------------------------------------- //

    //this is very important, it will connect the controls with the kitti data
    QObject::connect(GUI.Controls(), SIGNAL(updateData()), &GUI, SLOT(updateData()));
    //Laser clouds:
    QObject::connect(GUI.Controls(), SIGNAL(updateFrame(uint)), &cloudy1, SLOT(gotoFrame(uint)));
    QObject::connect(GUI.Controls(), SIGNAL(updateDataset(uint)), &cloudy1, SLOT(setDataSet(uint)));
    //IMU data:
    QObject::connect(GUI.Controls(), SIGNAL(updateFrame(uint)), &oxts, SLOT(gotoFrame(uint)));
    QObject::connect(GUI.Controls(), SIGNAL(updateDataset(uint)), &oxts, SLOT(setDataSet(uint)));
    //obstacles:
    QObject::connect(GUI.Controls(), SIGNAL(updateFrame(uint)), &obstacle, SLOT(gotoFrame(uint)));
    QObject::connect(GUI.Controls(), SIGNAL(updateDataset(uint)), &obstacle, SLOT(setDataSet(uint)));
    //cameras:
    QObject::connect(GUI.Controls(), SIGNAL(updateFrame(uint)), GUI.Cameras(id_camera1), SLOT(gotoFrame(uint)));
    QObject::connect(GUI.Controls(), SIGNAL(updateDataset(uint)), GUI.Cameras(id_camera1), SLOT(setDataSet(uint)));
    //trajectory:
//    QObject::connect(GUI.Controls(), SIGNAL(updateDataset(uint)), &kTrajectoryLoader, SLOT(setDataSet(uint)));
//    QObject::connect(&kTrajectoryLoader, SIGNAL(updateTrajectory(uint)), &GUI, SLOT(updateTrajectory(uint)));
    //Text labels
    QObject::connect(GUI.Controls(), SIGNAL(updateFrame(uint)), &labels, SLOT(gotoFrame(uint)));
    QObject::connect(GUI.Controls(), SIGNAL(updateDataset(uint)), &labels, SLOT(setDataSet(uint)));

    //QObject::connect(&GUI, SIGNAL(frameChanged()), &trajectory, SLOT(calculate()));

    #ifdef NO_EXAMPLES
      QObject::connect(GUI.Controls(), SIGNAL(updateFrame(uint)), &kCustomObstacleLoader, SLOT(gotoFrame(uint)));
      QObject::connect(GUI.Controls(), SIGNAL(updateDataset(uint)), &kCustomObstacleLoader, SLOT(setDataSet(uint)));
    #endif

    //example to update Trajectory:
    //QObject::connect(&yourClass, SIGNAL(updateTrajectory(unsigned int)), &GUI, SLOT(updateTrajectory(unsigned int)));
    //
    // ----------------------------------------
    //
    // inside "yourClass" you need to create a SIGNAL in the headers this way:
    //
    //signals:
    //  bool updateTrajectoy(unsigned int _id);
    //
    // ----------------------------------------
    //
    // And you can call that signal by using the following inside any method in "yourClass":
    //
    //  emit updateTrajectory(idTrajectory); //idTrajectory contains the ID of the trajectory
    //

    //conections means to call an outside class method without any need for inheritance
    //you just need to know where the class are stored, there is why we used two pointers, example:
    //GUI.Controls() is the pointer for the controls' class and &cloudy is the pointer for the laser
    //class

    //To connect two classes it is necessary to have both of them as QObjects; QWidget elements are
    //automatically asigned as objects, but if you create your own class, you need to include the
    //follow code lines in the class' header file AND to compile the headers as MOC files with CMake:

    //  #include <QObject>                      //#important
    //  ...
    //  Class yourClassName : public QObject {  //#important your class is a : public QObject type
    //    Q_OBJECT                              //#important
    //  ...
    //  }

    //example of MOC file compilation with CMake:

    //  #cmake version
    //  cmake_minimum_required(VERSION 2.8.12)
    //  ...
    //  #in case you are using Qt5
    //  qt5_wrap_cpp(PROJECT_HEADERS_MOC yourClassHeaderFile.h)
    //  ...
    //  add_executable(yourProjectName
    //    ...
    //    ${PROJECT_HEADERS_MOC}
    //    ...
    //  )

    //Make sure you don't compile the same file twice (especially with MOC files)
    //because it will resullt in CMake errors (I prefer to have AUTOMOC_FILES deactivated)


    //if you define NO_EXAMPLES before this, this should not be executed
    #ifndef NO_EXAMPLES
      //example of ground creation
      vector<Boxes> grounding;
      Boxes groundTest;
      groundTest.r = 127;
      groundTest.g = 127;
      groundTest.b = 127;
      groundTest.width = 0.5f;
      groundTest.length = 0.5f;
      groundTest.height = 1.27f;
      groundTest.positionX = 0.25f;
      groundTest.positionY = 0.25f;
//      groundTest.positionX = 0.5f;
//      groundTest.positionY = 0.5f;
      groundTest.positionZ = 0.0f;
      grounding.push_back(groundTest);
      groundTest.r = 255;
      groundTest.g = 0;
      groundTest.b = 0;
      groundTest.width = 0.5f;
      groundTest.length = 0.5f;
      groundTest.height = 0.0f;
      groundTest.positionX = 0.25f;
      groundTest.positionY = -0.25f;
//      groundTest.positionX = 0.5f;
//      groundTest.positionY = -0.5f;
      groundTest.positionZ = 0.0f;
      grounding.push_back(groundTest);
      groundTest.r = 0;
      groundTest.g = 255;
      groundTest.b = 0;
      groundTest.width = 0.5f;
      groundTest.length = 0.5f;
      groundTest.height = 0.61f;
      groundTest.positionX = -0.25f;
      groundTest.positionY = 0.25f;
//      groundTest.positionX = -0.5f;
//      groundTest.positionY = 0.5f;
      groundTest.positionZ = 0.0f;
      grounding.push_back(groundTest);
      groundTest.r = 0;
      groundTest.g = 0;
      groundTest.b = 255;
      groundTest.width = 0.5f;
      groundTest.length = 0.5f;
      groundTest.height = -0.25f;
      groundTest.positionX = -0.25f;
      groundTest.positionY = -0.25f;
//      groundTest.positionX = -0.5f;
//      groundTest.positionY = -0.5f;
      groundTest.positionZ = 0.0f;
      grounding.push_back(groundTest);

//      for(int i = 0; i < 9996; i++){
//        grounding.push_back(groundTest);
//      }

      //GUI.onGround = true;
//      unsigned int id_ground = GUI.addGround(&grounding, "TESTING BOXES", true, true, GUI.NavigationFrameTransformationMatrix());
//      GUI.translateGround(id_ground, 0, 0, TIRE_AXIS_TO_GROUND);

      //example of trajectory creation
      vector<vector<trajectoryData> > trajector;
      vector<trajectoryData> tractor;
      trajectoryData trajectoryTest;
      trajectoryTest.lineType = 1;
      trajectoryTest.lineWidth = 0.3;
      trajectoryTest.positionX = 0;
      trajectoryTest.positionY = 0;
      trajectoryTest.positionZ = 0;
      trajectoryTest.r = 255;
      trajectoryTest.g = 200;
      trajectoryTest.b = 127;
//      trajectoryTest.r = 9;
//      trajectoryTest.g = 163;
//      trajectoryTest.b = 230;
      tractor.push_back(trajectoryTest);
      trajectoryTest.positionX = 2;
      trajectoryTest.positionY = 0;
      trajectoryTest.positionZ = 0;
      tractor.push_back(trajectoryTest);
      trajectoryTest.positionX = 5;
      trajectoryTest.positionY = 1;
      trajectoryTest.positionZ = 0;
      tractor.push_back(trajectoryTest);
      trajectoryTest.positionX = 10;
      trajectoryTest.positionY = -3;
      trajectoryTest.positionZ = 0;
      tractor.push_back(trajectoryTest);
      trajectoryTest.positionX = 10;
      trajectoryTest.positionY = -5;
      trajectoryTest.positionZ = 0;
      tractor.push_back(trajectoryTest);
      trajectoryTest.positionX = 13;
      trajectoryTest.positionY = -2;
      trajectoryTest.positionZ = 0;
      tractor.push_back(trajectoryTest);
      trajectoryTest.positionX = 15;
      trajectoryTest.positionY = -2;
      trajectoryTest.positionZ = 0;
      tractor.push_back(trajectoryTest);
      trajectoryTest.positionX = 20;
      trajectoryTest.positionY = 4;
      trajectoryTest.positionZ = 0;
      tractor.push_back(trajectoryTest);

      for(int i = 0; i < 992; i++){
        tractor.push_back(trajectoryTest);
      }

      trajector.push_back(tractor);

//      unsigned int traject1 = GUI.addTrajectory(&trajector, "TEST TRAJECTORY", true, GUI.VehicleTransformationMatrix());
//      //this is for fixed frame transformation matrix
////      unsigned int traject1 = GUI.addTrajectory(&trajector, "TEST TRAJECTORY");
//      //or you can use this:
////      unsigned int traject1 = GUI.addTrajectory(&trajector, "TEST TRAJECTORY", &GUI.fixedFrameTransformationMatrix);
//      GUI.translateTrajectoryMatrix(traject1, 0, 0, TIRE_AXIS_TO_GROUND);
    #endif

    //-------------------------------------------------------------------------------------------------------//
    //                                      End creating graphic elements                                    //
    //-------------------------------------------------------------------------------------------------------//

    return app.exec();
  #else
    //ros data reader
    ROSData rosy;

    //create a new latitude and longitude conversor
    CoordinateConversor conversor(&rosy.getFrameValues()->latitude, &rosy.getFrameValues()->longitude);

    //sets the rosy conversor
    rosy.setConversor(&conversor);
    GUI.setCoordinateConversor(&conversor);

    #ifdef EGOIST
      NavigationTrajectory trajectory("/home/egoist/Dokumente/trajectories/", &conversor);
    #else
      NavigationTrajectory trajectory("/home/franciscoramirez/Documents/trajectories/", &conversor);
    #endif

    QObject::connect(&rosy, SIGNAL(alternativeChanged(bool)), &trajectory, SLOT(recalculate(bool)));
    QObject::connect(&rosy, SIGNAL(alternativeChanged(bool)), &GUI, SLOT(setAlternative(bool)));

    //adding the laserCloud to the scenary, the id will serve later to
    //modify or delete this cloud
    unsigned int id_laser = GUI.addLaser(rosy.getCloudData(), "LASER CLOUD", true, GUI.VehicleTransformationMatrix(), 2, true);
//    GUI.translateLaserMatrix(id_laser, VELODYNE_POSITION_X, VELODYNE_POSITION_Y, VELODYNE_POSITION_Z);

    unsigned int id_dynamic_boxes = GUI.addObstacles(rosy.getDynamicBoxes(), "DYNAMIC BOXES", true, GUI.VehicleTransformationMatrix());
//    GUI.translateObstacleMatrix(id_dynamic_boxes, VELODYNE_POSITION_X, VELODYNE_POSITION_Y, VELODYNE_POSITION_Z);

    unsigned int id_static_boxes = GUI.addObstacles(rosy.getStaticBoxes(), "STATIC BOXES", false, GUI.VehicleTransformationMatrix());
//    GUI.translateObstacleMatrix(id_static_boxes, VELODYNE_POSITION_X, VELODYNE_POSITION_Y, VELODYNE_POSITION_Z);

    unsigned int id_dynamic_labels = GUI.addLabels(rosy.getDynamicTexts(), "DYNAMIC LABELS", true);
//    GUI.translateLabelMatrix(id_dynamic_labels, VELODYNE_POSITION_X, VELODYNE_POSITION_Y, VELODYNE_POSITION_Z);

    unsigned int id_static_labels = GUI.addLabels(rosy.getStaticTexts(), "STATIC LABELS", false);
//    GUI.translateLabelMatrix(id_static_labels, VELODYNE_POSITION_X, VELODYNE_POSITION_Y, VELODYNE_POSITION_Z);

    unsigned int id_traject1 = GUI.addTrajectory(trajectory.GetTrajectories(), "TESTING TRAJECTORY", true, GUI.FixedPositionTransformationMatrix());
    GUI.translateTrajectoryMatrix(id_traject1, 0, 0, TIRE_AXIS_TO_GROUND);
    QObject::connect(&GUI, SIGNAL(frameChanged()), &trajectory, SLOT(calculate()));

//    GUI.ChangeCameraLookingAt(3.5, 0, 0);
//    GUI.ChangeScenaryZoom(0.3);

    //this is for a manual transmission, see structure definitions in "headers/definitions.h"
    vector<nameANDvalue> gearbox_values(6);
    gearbox_values[0] = { "1", -2 };
    gearbox_values[1] = { "2", -1 };
    gearbox_values[2] = { "3",  0 };
    gearbox_values[3] = { "4",  1 };
    gearbox_values[4] = { "5",  2 };
    gearbox_values[5] = { "6",  3 };
    gearbox_values.push_back({ "R",  4 });

    //this is for the General status graphic element, see structure definitions in "headers/definitions.h"
    vector<nameANDdata> textList_values(3);
    textList_values[0] = { "LAT", &rosy.getVehicleValues()->latitude, "°", 10 };
    textList_values[1] = { "LON", &rosy.getVehicleValues()->longitude, "°", 10 };
    textList_values[2] = { "ALT", &rosy.getVehicleValues()->altitude, " m", 4 };
//    textList_values[3] = { "MAXIMUM", rosy.getMaximum(), "", 2 };

    //this is for the Sensor status
    vector<nameANDboolen> iconStatus_values(5);
    iconStatus_values[0] = { "INS", &rosy.isins };
    iconStatus_values[1] = { "GPS POS", &rosy.ispos };
    iconStatus_values[2] = { "GPS VEL", &rosy.isvel };
    iconStatus_values[3] = { "GPS ALT", &rosy.isalt };
    iconStatus_values[4] = { "STATUS", &rosy.istat };

//    GUI.addVerticalBar("BRAKES", 1, 1, 0, &rosy.getVehicleValues()->brakes, 315, 0, LEFT, BOTTOM);
//    GUI.addVerticalBar("GAS", 1, 1, 0, &rosy.getVehicleValues()->gas, 385, 0, LEFT, BOTTOM);
    GUI.addSteering("STEERING", 1, &rosy.getVehicleValues()->steeringAngle, 0, 0, LEFT, BOTTOM);
    GUI.addBattery("BATTERY", 0, 100, 0, &rosy.getVehicleValues()->battery, 110, 0, LEFT);
    GUI.addSpeedometer("SPEED AND RPM", 150, 0, 6000, 0, &rosy.getVehicleValues()->velocity, &rosy.getVehicleValues()->rpm, 180, 0, LEFT, BOTTOM);
//    GUI.addGearbox(MANUAL, "GEARBOX", &gearbox_values, &rosy.getVehicleValues()->gearbox, 110, 0, LEFT, BOTTOM);
    GUI.addTextList("GENERAL STATUS", &textList_values, 330, 0, 270, 150, LEFT, BOTTOM);
    GUI.addIconList("SENSOR STATUS", &iconStatus_values, 625, 0, 200, 150, LEFT, BOTTOM);

    unsigned int id_messages = GUI.addMessageList("GENERAL MESSAGES", 845, 0, -845, 150, LEFT, BOTTOM);
    GUI.addMessageToList(id_messages, "Succesful connection to ROS", PROPER_M);

    unsigned int id_signaling = GUI.addSignaling(20, 30);
    GUI.addSign(id_signaling, 274, 15);    //274 is the european code for speed signalization

//    GUI.onGUI = false;
//    GUI.onMenu = false;
    GUI.addClock(&rosy.getVehicleValues()->timestamp);

    GUI.addCompass(0, 5, 0.6);

    //this is very important if you dont introduce this values then the vehicle's
    //transformation matrix will always be a identity matrix
    GUI.addVehicleRotation(&rosy.getVehicleValues()->pitch, &rosy.getVehicleValues()->yaw, &rosy.getVehicleValues()->roll);

    //you must establish the vehicle position in meters with origin in the static frame origin
    //to generate the proper transformation matrix
    GUI.addVehiclePosition(&rosy.getVehicleValues()->x, &rosy.getVehicleValues()->y, &rosy.getVehicleValues()->z,
                           &rosy.getVehicleValues()->latitude, &rosy.getVehicleValues()->longitude);

    //this is just to vehicle mechanical movements in scene:
    GUI.addVehicleAccelerations(&rosy.getVehicleValues()->accelerationX, &rosy.getVehicleValues()->accelerationY, &rosy.getVehicleValues()->accelerationZ);

    GUI.addVehicleSpeed(&rosy.getVehicleValues()->velocity);

    //there is not steering angle so it is introduced the leftward acceleration, this is not necessary
    //because if no steering angle is defined it will use automatically the leftward acceleration
    //previously defined, the next line was introduced just has an example.
    GUI.addVehicleSteeringWheelAngle(&rosy.getVehicleValues()->steeringAngle);

    //adding the vehicle into scene
    GUI.addVehicle(SHUTTLE);

    GUI.setFixedFrameOrigin(&rosy.getFrameValues()->latitude, &rosy.getFrameValues()->longitude, &rosy.getFrameValues()->altitude,
                            &rosy.getFrameValues()->pitch, &rosy.getFrameValues()->yaw, &rosy.getFrameValues()->roll);

//    GUI.setMapPosition(&rosy.getVehicleValues()->latitude, &rosy.getVehicleValues()->longitude);
//    GUI.translateMap(0, 0, TIRE_AXIS_TO_GROUND);

    vector<Boxes> grounding;

    GUI.onGround = true;
    unsigned int id_ground = GUI.addGround(&grounding, "GROUND", true, true, GUI.FixedPositionTransformationMatrix());
    GUI.translateGround(id_ground, 0, 0, TIRE_AXIS_TO_GROUND);

    GUI.setROSspinner(ROS_CYCLE_TIME_MILLISECONDS);

    return app.exec();
  #endif
}
