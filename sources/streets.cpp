#include "headers/streets.h"

Streets::Streets(QOpenGLShaderProgram *_program, int *_scenaryRotationY) :
  isInstalling(nullptr),
  m_program(_program),
  lineW(8),
  indexBuffer(QOpenGLBuffer::IndexBuffer),
  indexSignsBuffer(QOpenGLBuffer::IndexBuffer),
  latitude(nullptr),
  longitude(nullptr),
  ltMin(-1000),
  ltMax(-1000),
  lnMin(-1000),
  lnMax(-1000),
  mapRangeW(150),
  mapRangeL(150),
  matrix(nullptr),
  yrot(_scenaryRotationY),
  yrot2(-10000)
{
  streetBuffer.create();
  lanesBuffer.create();
  signsBuffer.create();
  indexBuffer.create();
  indexSignsBuffer.create();

  i_position = m_program->attributeLocation("i_position");
  i_translation = m_program->attributeLocation("i_translation");
  i_texcoor = m_program->attributeLocation("i_texcoor");
  i_texture = m_program->attributeLocation("i_texture");
  u_model = m_program->uniformLocation("u_model");
  u_map = m_program->uniformLocation("u_map");
  u_rotation = m_program->uniformLocation("u_rotation");
  u_mapModel = m_program->uniformLocation("u_mapModel");

  //gentextures creates an unique ID to avoid conflicts when passing data to GLSL shaders
  glGenTextures(1, &texture1);
  glGenTextures(1, &texture2);
  glGenTextures(1, &texture3);
  glGenTextures(1, &texture4);
  m_program->setUniformValue("diffuseMap1", texture1);
  m_program->setUniformValue("diffuseMap2", texture2);
  m_program->setUniformValue("diffuseMap3", texture3);
  m_program->setUniformValue("diffuseMap4", texture4);

  // ------------------------------------------------------------------------------------ //
  // -------------------------------- initial variables --------------------------------- //
  // ------------------------------------------------------------------------------------ //
  pT[0] = QVector3D(-0.3, 3,-0.01);
  pT[1] = QVector3D( 0.3, 3,-0.01);
  pT[2] = QVector3D( 0.3, 2.4,-0.01);
  pT[3] = QVector3D(-0.3, 2.4,-0.01);

  pB[0] = QVector3D(-0.03, 2.5, 0);
  pB[1] = QVector3D( 0.03, 2.5, 0);
  pB[2] = QVector3D( 0.03, 0, 0);
  pB[3] = QVector3D(-0.03, 0, 0);

  tP[0] = QVector2D(0, 0);
  tP[1] = QVector2D(1, 0);
  tP[2] = QVector2D(1, 1);
  tP[3] = QVector2D(0, 1);

  oP[0] = 0;
  oP[1] = 0;
  oP[2] = 1;
  oP[3] = 3;
  oP[4] = 2;
  oP[5] = 2;

  // ------------------------------------------------------------------------------------ //
  // --------------------------------- Adding textures ---------------------------------- //
  // ------------------------------------------------------------------------------------ //

  initTextures(":/images/road1", true, 0);
  initTextures(":/images/laner", true, 1);
  initTextures(":/images/signals1", false, 2);
  initTextures(":/images/signals2", false, 3);
  initTextures(":/images/signal_bar", false, 4);
  initTextures(":/images/traffic_light", false, 5);

  mapping = new LoadMaps();
}

Streets::~Streets(){
  streetBuffer.destroy();
  lanesBuffer.destroy();
  signsBuffer.destroy();
  indexBuffer.destroy();
  indexSignsBuffer.destroy();

  for(int i = 0; i < 6; i++)
    delete texture[i];

  delete mapping;
}

bool Streets::setMapPosition(double *_latitude, double *_longitude){
  latitude = _latitude;
  longitude = _longitude;
  return forceUpdateMap();
}

bool Streets::setMapRange(unsigned int _width, unsigned int _length){
  mapRangeW = _width;
  mapRangeL = _length;
  return forceUpdateMap();
}

void Streets::setMapTransformationMatrix(QMatrix4x4 *_transformationMatrix){
  matrix = _transformationMatrix;
}

void Streets::translateMap(const float _x, const float _y, const float _z){
  mapMatrix.translate(-_y, _z, -_x);
}

void Streets::rotateMap(const float _pitch, const float _yaw, const float _roll){
  mapMatrix.rotate(QQuaternion::fromEulerAngles(_pitch * toDEGREES, _yaw * toDEGREES, _roll * toDEGREES));
}

QString Streets::seeFolderPath(){
  return mapping->getFolderPath();
}

float Angle(float x, float y){
  float ang = atan(y/x);
  if(x < 0)
    ang += _PI;
  else if(y < 0)
    ang += _2PI;
  return ang;
}

bool Streets::updateMap(){
  bool ok = false;

  if((latitude != nullptr) && (longitude != nullptr) && (isInstalling != nullptr)){
    if((*latitude > ltMax) || (*latitude < ltMin) || (*longitude > lnMax) || (*longitude < lnMin)){

      if(!mapping->occupied && !(*isInstalling)){
        //calculates the new range and converts the degrees into meters
        transformCoordinates();
        mapping->loadMap(*latitude, *longitude, dLt, dLn, mapRangeW, mapRangeL);
        emit reloadMapMatrix();
        ok = true;
      }
    }

    if(*yrot != yrot2){
      yrot2 = *yrot;
      signalMatrix.setToIdentity();
      signalMatrix.rotate(QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), *yrot/anglePrecision + 30));
    }
  }
  return ok;
}

bool Streets::forceUpdateMap(){
  ltMin = ltMax = lnMin = lnMax = -1000;
  mapping->stopMapLoading();
  return updateMap();
}

void Streets::restartData(){
  mapping->stopMapLoading();
  ltMin = ltMax = lnMin = lnMax = -1000;

  m_program->bind();
  streetPoints.clear();
  lanesPoints.clear();
  indicesStreets.clear();
  indicesStreetsSize = indicesStreets.size();
  indexBuffer.bind();
  indexBuffer.allocate(indicesStreets.data(), indicesStreetsSize * sizeof(unsigned int));

  streetBuffer.bind();
  streetBuffer.allocate(streetPoints.data(), streetPoints.size() * sizeof(streetData));

  lanesBuffer.bind();
  lanesBuffer.allocate(lanesPoints.data(), lanesPoints.size() * sizeof(streetData));

  indicesSignals.clear();

  signsPoints.clear();
  indicesSignalsSize = indicesSignals.size();
  indexSignsBuffer.bind();
  indexSignsBuffer.allocate(indicesSignals.data(), indicesSignalsSize * sizeof(unsigned int));

  signsBuffer.bind();
  signsBuffer.allocate(signsPoints.data(), signsPoints.size() * sizeof(signsData));
  m_program->release();
}

void Streets::loadMap(){
  streetPoints.clear();
  lanesPoints.clear();
  indicesStreets.clear();

  // ------------------------------------------------------------------------------------ //
  // ----------------------------- Creating the streets --------------------------------- //
  // ------------------------------------------------------------------------------------ //
  if(mapping->occupied) return;
  int sWays = mapping->streetList.size(); //obtaining the quantity of streets
  int pos = 0;
  bool thighway = false, oneway = false;
  float lanes = 2;
  float wline = lineW;
  float wlane = 0.1;
  float zheight = 0.05;
  QString wtags;
  QStringList tags, tag;
  Nodes *tempNode;

  for(int n = 0; n < sWays; n++){
    lanes = 0;
    wline = lineW;
    //zheight += 0.0005;
    //zheight += 0.1;
    if(mapping->occupied) return;
    int sPoints = mapping->streetList[n].size();//obtaining the quantity of data points for each street
    if(mapping->occupied) return;
    wtags = mapping->streetTags[n];
    tags = wtags.split("+", QString::SkipEmptyParts);
    for(int i = 0; i < tags.size(); i++){
      if((tags.value(i) == "highway=residential") || (tags.value(i) == "highway=tertiary") || (tags.value(i) == "highway=secondary") || (tags.value(i) == "highway=primary"))
        thighway = true;
      else if((tags.value(i) == "highway=service") || (tags.value(i) == "highway=track") || (tags.value(i) == "highway=path"))
        thighway = false;
      if(tags.value(i) == "oneway=yes")
        oneway = true;
      tag = tags.value(i).split("=");
      if(tag.value(0) == "width")
        wline = QString(tag.value(1)).toFloat();
      if(tag.value(0) == "lanes")
        lanes = QString(tag.value(1)).toInt();
    }
    if(oneway && (lanes == 0))
      lanes = 1;
    if(thighway && (lanes == 0))
      lanes = 2;
    else if(lanes == 0)
      lanes = 1;

    wline = lineW;

    QVector2D PP;//first point
    streetData temPoint;

    indicesStreets.append(pos);
    QVector2D P0, P1, P2, P0a, P2a;
    float ang0 = 0, ang1 = 0, ang2 = 0, dPP = 0, dPP2 = 0, ptxD = 0, ptxD2 = 0, ttxD = 0;

    for(int i = 0; i < sPoints; i++){
      if(mapping->occupied) return;
      tempNode = &mapping->streetList[n][i];
      int i1;
      P1 = QVector2D(-tempNode->longitude, tempNode->latitude);
      P0 = (i == 0)? P1 : PP;
      PP = P1;
      if(i < (sPoints - 1)){
        i1 = i + 1;
        if(mapping->occupied) return;
        tempNode = &mapping->streetList[n][i1];
        P2 = QVector2D(-tempNode->longitude, tempNode->latitude);
        P2 -= P1;
        ttxD = P2.length();
        P1 -= P0;
        ang1 = Angle(P1[0], P1[1]);
        ang2 = Angle(P2[0], P2[1]);
        ang1 = (ang1 != ang1)? ang2 : ang1;
        ang0 = ((ang1 + ang2)/2) + _PI2;
        dPP = (wline/2)/(qSin(ang1 + _PI - ang0));
        dPP2 = (wlane/2)/(qSin(ang1 + _PI - ang0));
        //qWarning("P0(%.2f, %.2f) P1(%.2f, %.2f) P2(%.2f, %.2f) ang0:%.2f h:%f", P0[0], P0[1], P1[0], P1[1], P2[0], P2[1], ang0*180/3.1416, dPP);
        ang1 = ang0 + _PI;
        P1 += P0;
        P0 = P1 + QVector2D(dPP * qCos(ang0), dPP * qSin(ang0));
        P2 = P1 + QVector2D(dPP * qCos(ang1), dPP * qSin(ang1));
        P0a = P1 + QVector2D(dPP2 * qCos(ang0), dPP2 * qSin(ang0));
        P2a = P1 + QVector2D(dPP2 * qCos(ang1), dPP2 * qSin(ang1));
        //qWarning("P0(%.2f, %.2f) P1(%.2f, %.2f) P2(%.2f, %.2f)", P0[0], P0[1], P1[0], P1[1], P2[0], P2[1]);
      }else{
        P0 = P1 + QVector2D(dPP * qCos(ang0), dPP * qSin(ang0));
        P2 = P1 + QVector2D(dPP * qCos(ang1), dPP * qSin(ang1));
        P0a = P1 + QVector2D(dPP2 * qCos(ang0), dPP2 * qSin(ang0));
        P2a = P1 + QVector2D(dPP2 * qCos(ang1), dPP2 * qSin(ang1));
      }
      temPoint.texcoor = QVector2D(lanes, ptxD);
      //temPoint.position = QVector3D(P0[0], -1.73 + zheight, P0[1]);
      temPoint.position = QVector3D(P0[0], - zheight, P0[1]);
      streetPoints.append(temPoint);
      temPoint.texcoor = QVector2D(1, ptxD2);
      temPoint.position = QVector3D(P0a[0], 0, P0a[1]);
      lanesPoints.append(temPoint);
      indicesStreets.append(pos);
      pos++;
      temPoint.texcoor = QVector2D(0, ptxD);
      //temPoint.position = QVector3D(P2[0], -1.73 + zheight, P2[1]);
      temPoint.position = QVector3D(P2[0], - zheight, P2[1]);
      streetPoints.append(temPoint);
      temPoint.texcoor = QVector2D(0, ptxD2);
      temPoint.position = QVector3D(P2a[0], 0, P2a[1]);
      lanesPoints.append(temPoint);
      indicesStreets.append(pos);
      pos++;
      ptxD += ttxD/wline;
      ptxD2 += ttxD/wlane * 0.3;
    }
    indicesStreets.append(pos - 1);
  }

  indicesSignals.clear();
  signsPoints.clear();

  if(mapping->occupied) return;
  int sSigns = mapping->signsList.size();

  if(sSigns > 0){
    signsData tSign;
    Signals *tempSign;
    int mlt = 0;
    float txtS = 0;

    for(int i = 0; i < sSigns; i++){
      if(mapping->occupied) return;
      tempSign = &mapping->signsList[i];
      txtS = 0;
      if(tempSign->type == 267)
        mlt = 0;
      else if(tempSign->type == 205)
        mlt = 1;
      else if(tempSign->type == 306)
        mlt = 2;
      else if(tempSign->type == 215)
        mlt = 3;
      else if(tempSign->type == 350)
        mlt = 4;
      else if(tempSign->type == 220)
        mlt = 5;
      else if(tempSign->type == 108)
        mlt = 6;
      if(tempSign->type == 274){
        txtS = 1;
        if(tempSign->value == 10)
          mlt = 0;
        if(tempSign->value == 20)
          mlt = 1;
        if(tempSign->value == 30)
          mlt = 2;
        if(tempSign->value == 40)
          mlt = 3;
        if(tempSign->value == 50)
          mlt = 4;
        if(tempSign->value == 60)
          mlt = 5;
        if(tempSign->value == 70)
          mlt = 6;
      }
      if(tempSign->type == 2000)
        txtS = 3;

      tSign.translation = QVector2D(tempSign->longitude, tempSign->latitude);
      for(int e = 0; e < 8; e++){
        if(e >= 4){
          if(txtS == 3){
            tSign.texcoor = QVector2D(tP[e - 4][0], tP[e - 4][1]);
          }else{
            tSign.texcoor = QVector2D(tP[e - 4][0] * 0.142857 + mlt * 0.142857, tP[e - 4][1]);
          }
          tSign.position = QVector3D(pT[e - 4][0], pT[e - 4][1], pT[e - 4][2]);
          tSign.texture = txtS;
        }else{
          tSign.position = QVector3D(pB[e][0], pB[e][1], pB[e][2]);
          tSign.texcoor = tP[e];
          tSign.texture = 2;
        }
        signsPoints.append(tSign);
      }
      for(int e = 0; e < 12; e++){
        if(e < 6){
          indicesSignals.append(oP[e] + i * 8);
        }else{
          indicesSignals.append(oP[e - 6] + i * 8 + 4);
        }
      }
    }
  }


  m_program->bind();
  // Transfer index data to VBO 1
  indicesStreetsSize = indicesStreets.size();
  indexBuffer.bind();
  indexBuffer.allocate(indicesStreets.data(), indicesStreetsSize * sizeof(unsigned int));

  streetBuffer.bind();
  streetBuffer.allocate(streetPoints.data(), streetPoints.size() * sizeof(streetData));

  lanesBuffer.bind();
  lanesBuffer.allocate(lanesPoints.data(), lanesPoints.size() * sizeof(streetData));

  indicesSignalsSize = indicesSignals.size();
  indexSignsBuffer.bind();
  indexSignsBuffer.allocate(indicesSignals.data(), indicesSignalsSize * sizeof(unsigned int));

  signsBuffer.bind();
  signsBuffer.allocate(signsPoints.data(), signsPoints.size() * sizeof(signsData));
  m_program->release();
}

void Streets::transformCoordinates(){
  //obtain the distance per grade (°) of the current GPS coordinates
  //converting degrees to radians:
  //lt = latitide, ln = longitude
  float iLt = qRound(*latitude) * 0.01745329252;
  //distance in meters of latitude:
  dLt = 111132.92 - 559.82 * qCos(2 * iLt) + 1.175 * qCos(4 * iLt) - 0.0023 * qCos(6 * iLt);
  //longitude:
  dLn = 111412.84 * qCos(iLt) - 93.5 * qCos(3 * iLt) - 0.118 * qCos(5 * iLt);

  //this will obtain the range in GPS coordinates
  double rLt = (mapRangeW/2.0f)/dLt;
  double rLn = (mapRangeL/2.0f)/dLn;
  ltMin = *latitude - rLt * 0.7;
  ltMax = *latitude + rLt * 0.7;
  lnMin = *longitude - rLn * 0.7;
  lnMax = *longitude + rLn * 0.7;
}

void Streets::drawStreets(bool _wtd){
  if((latitude != nullptr) && (longitude != nullptr)){
    quintptr offset = 0;

    if(matrix != nullptr)
      m_program->setUniformValue(u_model, *matrix);
    else
      m_program->setUniformValue(u_model, emptyMatrix);
    m_program->setUniformValue(u_mapModel, mapMatrix);

    if(_wtd){
      glDisable(GL_DEPTH_TEST);
      indexBuffer.bind();
      streetBuffer.bind();
      texture[0]->bind(texture1);
      m_program->setUniformValue(u_map, true);
      m_program->setUniformValue(u_rotation, emptyMatrix);

      // Offset for position
      offset = 0;

      //Tell OpenGL programmable pipeline where find vertex positions
      m_program->setAttributeBuffer(i_position, GL_FLOAT, offset, 3, sizeof(streetData));

      offset += sizeof(QVector3D);
      m_program->setAttributeBuffer(i_texcoor, GL_FLOAT, offset, 2, sizeof(streetData));

      m_program->enableAttributeArray(i_position);
      m_program->enableAttributeArray(i_texcoor);

      //Draw cube geometry using indices from VBO
      glDrawElements(GL_TRIANGLE_STRIP, indicesStreetsSize, GL_UNSIGNED_INT, NULL);

      m_program->disableAttributeArray(i_texcoor);
      m_program->disableAttributeArray(i_position);

      //------------------------------------ Drawing blue lanes -----------------------------------------
      texture[1]->bind(texture1);
      indexBuffer.bind();
      lanesBuffer.bind();

      offset = 0;
      m_program->setAttributeBuffer(i_position, GL_FLOAT, offset, 3, sizeof(streetData));
      offset += sizeof(QVector3D);
      m_program->setAttributeBuffer(i_texcoor, GL_FLOAT, offset, 2, sizeof(streetData));

      m_program->enableAttributeArray(i_position);
      m_program->enableAttributeArray(i_texcoor);

      glDrawElements(GL_TRIANGLE_STRIP, indicesStreetsSize, GL_UNSIGNED_INT, NULL);

      m_program->disableAttributeArray(i_texcoor);
      m_program->disableAttributeArray(i_position);

      glEnable(GL_DEPTH_TEST);
    }else{
      //----------------------------------- Drawing traffic signs ---------------------------------------
      texture[2]->bind(texture1);
      texture[3]->bind(texture2);
      texture[4]->bind(texture3);
      texture[5]->bind(texture4);
      signsBuffer.bind();
      indexSignsBuffer.bind();
      m_program->setUniformValue(u_map, false);
      m_program->setUniformValue(u_rotation, signalMatrix);

      offset = 0;
      m_program->setAttributeBuffer(i_position, GL_FLOAT, offset, 3, sizeof(signsData));
      offset += sizeof(QVector3D);
      m_program->setAttributeBuffer(i_translation, GL_FLOAT, offset, 2, sizeof(signsData));
      offset += sizeof(QVector2D);
      m_program->setAttributeBuffer(i_texcoor, GL_FLOAT, offset, 2, sizeof(signsData));
      offset += sizeof(QVector2D);
      m_program->setAttributeBuffer(i_texture, GL_FLOAT, offset, 1, sizeof(signsData));

      m_program->enableAttributeArray(i_position);
      m_program->enableAttributeArray(i_translation);
      m_program->enableAttributeArray(i_texcoor);
      m_program->enableAttributeArray(i_texture);

      glDrawElements(GL_TRIANGLE_STRIP, indicesSignalsSize, GL_UNSIGNED_INT, NULL);

      m_program->disableAttributeArray(i_texture);
      m_program->disableAttributeArray(i_texcoor);
      m_program->disableAttributeArray(i_translation);
      m_program->disableAttributeArray(i_position);
    }
  }
}

void Streets::initTextures(QString image, bool mrd, int inx){
  // Load normal mapping image
  QImage tImg(image);
  if(mrd)
    texture[inx] = new QOpenGLTexture(tImg.mirrored());
  else
    texture[inx] = new QOpenGLTexture(tImg);

  // Set nearest filtering mode for texture minification
  texture[inx]->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);

  // Set bilinear filtering mode for texture magnification
  texture[inx]->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
  texture[inx]->setWrapMode(QOpenGLTexture::Repeat);
  texture[inx]->setAutoMipMapGenerationEnabled(true);
  texture[inx]->setMaximumAnisotropy(16);
}
