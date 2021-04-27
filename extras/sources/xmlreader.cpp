#include "xmlreader.h"

XMLReader::XMLReader() :
  filePath(),
  obstacles(0)
{}

void XMLReader::readXMLFile(const QString _filePath){

  filePath = _filePath;

  QDomDocument objects;
  QFile XMLfile(filePath);

  if(XMLfile.open(QIODevice::ReadOnly | QIODevice::Text)){
    if(!objects.setContent(&XMLfile)){
      qWarning() << "Failed to read file:" << filePath;
      return;
    }

    //object is the element <item> child of <tracklets>
    //  <tracklets>
    //    <item>  <-- this element
    //      <poses>
    //        <item>
    QDomNode object;
    object = objects.firstChildElement().firstChild().firstChildElement("item");

    //poses is:
    //  <tracklets>
    //    <item>
    //      <poses>  <-- this element
    //        <item>
    QDomNode poses;

    //item is:
    //  <tracklets>
    //    <item>
    //      <poses>
    //        <item>  <-- this element
    QDomNode item;
    int initialFrame, finalFrame;

    obstacles.clear();

    //read size, position, angles, frames and type of object for each detected movable object
    while(object.isElement()){
      //obstacle is each object that appear in the XML file, in this case is every element:
      //  <tracklets>
      //    <item>  <-- this element
      //      <poses>
      //        <item>
      Obstacle_item obstacle;

      //obstacle_frame is each <item> element inside the <poses> element
      //  <tracklets>
      //    <item>
      //      <poses>
      //        <item>  <-- this element
      Obstacle_frame obstacle_frame;

      //selecting <poses>
      poses = object.firstChildElement("poses");

      //this will give you the value inside <first_frame>
      initialFrame = object.firstChildElement("first_frame").toElement().text().toInt();

      //this will give you the number of total frames per object
      // poses.firstChildElement().text().toInt() will select the first element of <poses> which is <count> ang give you its value
      finalFrame = initialFrame + poses.firstChildElement().text().toInt();

      //selects the first element <item> inside <poses>
      item = poses.firstChildElement("item");

      //this will give you the value inside the element <objectType>
      obstacle.object_type = object.firstChildElement().text();

      //this will select all the child elements of <poses> including <count>, <item_version> and <item>
      QDomNodeList items = poses.childNodes();

      obstacle.height = object.firstChildElement("h").toElement().text().toDouble();
      obstacle.width = object.firstChildElement("w").toElement().text().toDouble();
      obstacle.length = object.firstChildElement("l").toElement().text().toDouble();

      //this determinates in which frame will this tracklet appear
      obstacle.initial_frame = initialFrame;
      obstacle.final_frame = finalFrame;

      //this will travel through all the elements <item> inside the element <poses>
      //with exception of the first two elements (<count> and <item_version>)
      for(uint i = 2; i < items.size(); i++){
        item = items.at(i);
        //places the position, remember the coordinate system of OpenGl es different from normal coordinate systems
        obstacle_frame.tx = item.firstChildElement("tx").toElement().text().toDouble();
        obstacle_frame.ty = item.firstChildElement("ty").toElement().text().toDouble();
        obstacle_frame.tz = item.firstChildElement("tz").toElement().text().toDouble();

        //obtain the euler angles from the xml file, values in radians
        obstacle_frame.rx = item.firstChildElement("rx").toElement().text().toDouble();
        obstacle_frame.ry = item.firstChildElement("ry").toElement().text().toDouble();
        obstacle_frame.rz = item.firstChildElement("rz").toElement().text().toDouble();

        obstacle_frame.state = item.firstChildElement("state").toElement().text().toDouble();

        obstacle_frame.occlusion = item.firstChildElement("occlusion").toElement().text().toDouble();
        obstacle_frame.occlusion_kf = item.firstChildElement("occlusion_kf").toElement().text().toDouble();

        obstacle_frame.truncation = item.firstChildElement("truncation").toElement().text().toDouble();

        obstacle_frame.amt_occlusion = item.firstChildElement("amt_occlusion").toElement().text().toDouble();
        obstacle_frame.amt_occlusion_kf = item.firstChildElement("amt_occlusion_kf").toElement().text().toDouble();

        obstacle_frame.amt_border_l = item.firstChildElement("amt_border_l").toElement().text().toDouble();
        obstacle_frame.amt_border_r = item.firstChildElement("amt_border_r").toElement().text().toDouble();
        obstacle_frame.amt_border_kf = item.firstChildElement("amt_border_kf").toElement().text().toDouble();

        obstacle.poses.push_back(obstacle_frame);
      }
      obstacles.push_back(obstacle);
      object = object.nextSibling();
    }

    XMLfile.close();
  }else
    qWarning() << "Failed to open file:" << filePath;
}

vector<Obstacle_item> *const XMLReader::getData(){
  return &obstacles;
}
