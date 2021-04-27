#cmake version
cmake_minimum_required(VERSION 2.8.12)

#project name
project(visualization_node)
set(PROJECT_BINARY_NAME visualization_node)

# use the next line if you are using ROS  <----------------------------------------------------- ROS
#add_definitions(-DROS_DATA)
#set(ROS_DATA true)

if(DEFINED ROS_DATA)
  find_package(catkin REQUIRED COMPONENTS
    roscpp
    rospy
    std_msgs
    message_filters
    message_generation
  )

  add_message_files(
    FILES
    object.msg
    object_vec.msg
  )

  generate_messages(DEPENDENCIES std_msgs)

  catkin_package(
  #  INCLUDE_DIRS include
  #  LIBRARIES anavs
    CATKIN_DEPENDS roscpp rospy std_msgs message_runtime
  #  DEPENDS system_lib
  )
endif(DEFINED ROS_DATA)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#compatibility with C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Widgets finds its own dependencies
#find_package(Qt5 COMPONENTS Widgets OpenGL Xml Sql REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(OpenGL REQUIRED)


# checks if ROS_DATA is defined
if(DEFINED ROS_DATA)
  add_definitions(-DROS_DATA)
  set(DATA_H extras/headers/rosdata.h)
  set(DATA_S extras/sources/rosdata.cpp)

  #include_directories(${catkin_INCLUDE_DIRS} $ENV{${Qt5_INCLUDE_DIRS}})
  include_directories(${catkin_INCLUDE_DIRS})

  set(ros_LIBRARIES ${catkin_LIBRARIES})
endif(DEFINED ROS_DATA)

#source files
set(CPP_FILES
  sources/console.cpp
  sources/ground.cpp
  sources/main.cpp
  sources/loadmap.cpp
  sources/installmaps.cpp
  sources/movingobjects.cpp
  sources/cylindricalobjects.cpp
  sources/scene.cpp
  sources/skybox.cpp
  sources/streets.cpp
  sources/threedimentionalmodelloaderparallax.cpp
  sources/threedimentionalmodelloadernormals.cpp
  sources/threedimentionalmodelloadersimple.cpp
  sources/uielements.cpp
  sources/vehicle.cpp
  sources/laserclouds.cpp
  sources/trajectory.cpp
  sources/databaseconnection.cpp
  sources/optionbuttons.cpp
  sources/textdraftsman.cpp
  sources/coordinateconversor.cpp
  #to obtain kitti data
  extras/sources/kittiDataIMU.cpp
  extras/sources/kittiLaserData.cpp
  extras/sources/kittiPCLData.cpp
  extras/sources/kittiObstacles.cpp
  extras/sources/kittiTrajectory.cpp
  extras/sources/kittiCustomObstacles.cpp
  extras/sources/kittiTextLabels.cpp
  extras/sources/kittiGroundExtractor.cpp
  sources/navigationTrajectory.cpp
  ${DATA_S}
#  extras/sources/xmlreader.cpp
)

#header files
set(HPP_FILES
  headers/console.h
  headers/ground.h
  headers/loadmap.h
  headers/installmaps.h
  headers/movingobjects.h
  headers/cylindricalobjects.h
  headers/scene.h
  headers/skybox.h
  headers/streets.h
  headers/threedimentionalmodelloaderparallax.h
  headers/threedimentionalmodelloadernormals.h
  headers/threedimentionalmodelloadersimple.h
  headers/uielements.h
  headers/vehicle.h
  headers/laserclouds.h
  headers/trajectory.h
  headers/parameters.h
  headers/databaseconnection.h
  headers/optionbuttons.h
  headers/definitions.h
  headers/textdraftsman.h
  headers/coordinateconversor.h
  #to obtain kitti data
  extras/headers/kittiDataIMU.h
  extras/headers/kittiLaserData.h
  extras/headers/kittiPCLData.h
  extras/headers/kittiObstacles.h
  extras/headers/kittiTrajectory.h
  extras/headers/kittiCustomObstacles.h
  extras/headers/kittiTextLabels.h
  extras/headers/kittiGroundExtractor.h
  headers/navigationTrajectory.h
  ${DATA_H}
  #to obtain ibeo lidar data
  #extras/headers/ibeoLaserReader.h
#  extras/headers/xmlreader.h
)

#this creates MOC files for classes that include the Q_OBJECT macro, if 'vtable' error appears then you should include the headers in here
set(WRAP_CPP_FILES
  headers/console.h
  headers/uielements.h
  headers/scene.h
  headers/streets.h
  headers/loadmap.h
  headers/installmaps.h
  headers/optionbuttons.h
  headers/textdraftsman.h
  #to obtain kitti data
  extras/headers/kittiLaserData.h
  extras/headers/kittiPCLData.h
  extras/headers/kittiDataIMU.h
  extras/headers/kittiObstacles.h
  extras/headers/kittiTrajectory.h
  extras/headers/kittiCustomObstacles.h
  extras/headers/kittiTextLabels.h
  extras/headers/kittiGroundExtractor.h
  headers/navigationTrajectory.h
  ${DATA_H}
  #to obtain ibeo lidar data
  #extras/headers/ibeoLaserReader.h
)
#including the GUI forms
set(WRAP_UI_FILES forms/console.ui)
#including the resource files
set(WRAP_RS_FILES
  resources/fonts.qrc
  resources/images.qrc
  resources/shaders.qrc
)

#qt5 requires wrapping of several parts, in this case are: MOC files, forms and resources
qt5_wrap_cpp(PROJECT_HEADERS_MOC ${WRAP_CPP_FILES})
qt5_wrap_ui(PROJECT_FORMS_HEADERS ${WRAP_UI_FILES})
qt5_add_resources(PROJECT_RESOURCES ${WRAP_RS_FILES})

#add every file to the executable
add_executable(${PROJECT_BINARY_NAME}
  ${CPP_FILES}
  ${HPP_FILES}
  ${PROJECT_HEADERS_MOC}
  ${PROJECT_FORMS_HEADERS}
  ${PROJECT_RESOURCES}
)

if(DEFINED ROS_DATA)
  add_dependencies(${PROJECT_BINARY_NAME} ${PROJECT_NAME}_generate_messages_cpp ${catkin_EXPORTED_TARGETS})
endif(DEFINED ROS_DATA)

#indicates which libraries to use in the executable
target_link_libraries(${PROJECT_BINARY_NAME}
  Qt5::Widgets
  Qt5::OpenGL
  Qt5::Xml
  Qt5::Sql
  ${ros_LIBRARIES}
  ${OPENGL_LIBRARIES}
)
