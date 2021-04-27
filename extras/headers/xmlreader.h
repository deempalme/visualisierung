#ifndef XMLREADER_H
#define XMLREADER_H

//there are some disvantages about this code
//it requires the next packages in CMakeList.txt

/*

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)

#in target libraries:
target_link_libraries( project_name
  Qt5::Widgets
  Qt5::Xml
)

*/
#include <QFile>
#include <QtXml>
#include <QString>
#include <QDebug>
#include <vector>

using namespace std;

struct Obstacle_frame{
  //positions relative to the position of the laser sensor
  double tx;
  double ty;
  double tz;

  //orientation in euler angles
  double rx;
  double ry;
  double rz;

  double state;
  double occlusion;
  double occlusion_kf;
  double truncation;

  double amt_occlusion;
  double amt_occlusion_kf;
  double amt_border_l;
  double amt_border_r;
  double amt_border_kf;
};

struct Obstacle_item{
  //contains the object type in a string variable
  QString object_type;

  double height;
  double width;
  double length;

  //contains all the <item> elements that appear inside <poses> for each object
  vector<Obstacle_frame> poses;

  //first frame in which this object appears
  int initial_frame;
  //last frame in which this object appears
  int final_frame;
};

class XMLReader
{
public:
  XMLReader();

  void readXMLFile(const QString _filePath);

  vector<Obstacle_item> *const getData();

private:
  //contains the path to the file to read
  QString filePath;
  //contains all the objects in the XML file
  vector<Obstacle_item> obstacles;
};

#endif // XMLREADER_H
