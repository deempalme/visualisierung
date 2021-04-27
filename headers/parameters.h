#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QString>

// ----------------------------------------------------------------------------------------------------- //
// -------------------------------- database connection to collect maps -------------------------------- //
// ----------------------------------------------------------------------------------------------------- //

const QString E_SERVER        = "localhost";          //SQL server
const QString E_DATABASE      = "gps_map";            //SQL database
const QString E_USERNAME      = "mapping";            //SQL user, must have read, update and delete permissions
const QString E_PASSWORD      = "1234567";            //SQL password

#ifdef ROS_DATA
  #ifdef EGOIST
    const QString E_MODELS3D      = "/home/egoist/Dokumente/models3D/";       //this will define in which place is located the 3D models folder
    const QString E_MAPSFOLDER    = "/home/egoist/Dokumente/maps/";           //this is the path of the maps folder (which contains the .osm files to install)
  #else
    const QString E_MODELS3D      = "/home/franciscoramirez/Documents/Qt/models3D/";       //this will define in which place is located the 3D models folder
    const QString E_MAPSFOLDER    = "/home/franciscoramirez/Documents/Qt/maps/";           //this is the path of the maps folder (which contains the .osm files to install)
  #endif
  const QString E_KITTIFOLDER   = "/home/franciscoramirez/Documents/Qt/KittiData/raw/";  //this is the path to kitti data folder
#else
  const QString E_MODELS3D      = "../models3D_old/";       //this will define in which place is located the 3D models folder
  const QString E_MAPSFOLDER    = "../maps/";           //this is the path of the maps folder (which contains the .osm files to install)
  const QString E_KITTIFOLDER   = "../KittiData/raw/";  //this is the path to kitti data folder
#endif

#endif // MAPS_H
