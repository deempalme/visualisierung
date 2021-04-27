#include "headers/laserclouds.h"

LaserClouds::LaserClouds(QOpenGLShaderProgram *_program, const vector<LaserCloudData> *const _inputCloud,
                         const float _pointSize, const bool _gradient, const bool _colorized,
                         const bool _isIntensityHigherThanOne) :
  m_program(_program),
  laserCloud(_inputCloud),
  laserCloudPCL(nullptr),
  laserCloudIBEO(nullptr),
  modelVehicle(nullptr),
  modelCloud(),
  emptyMatrix(),
  colorized(_colorized),
  isIntensityInterger(_isIntensityHigherThanOne),
  solid(!_gradient),
  isRGB(false),
  ignoreSolid(false),
  isPointDAI(false),
  pointSize(_pointSize)
{
  setInitValues();
}

LaserClouds::LaserClouds(QOpenGLShaderProgram *_program, const vector<pointRGB> *const _inputCloud,
                         const float _pointSize, const bool _gradient, const bool _colorized,
                         const bool _isIntensityHigherThanOne) :
  m_program(_program),
  laserCloud(nullptr),
  laserCloudPCL(_inputCloud),
  laserCloudIBEO(nullptr),
  modelVehicle(nullptr),
  modelCloud(),
  emptyMatrix(),
  colorized(_colorized),
  isIntensityInterger(_isIntensityHigherThanOne),
  solid(!_gradient),
  isRGB(true),
  ignoreSolid(false),
  isPointDAI(false),
  pointSize(_pointSize)
{
  setInitValues();
}

LaserClouds::LaserClouds(QOpenGLShaderProgram *_program, const vector<pointDAI> *const _inputCloud,
                         const float _pointSize, const float _angularResolutionDEGREES,
                         const float _verticalAngleBetweenLayersDEGREES, const bool _gradient,
                         const bool _colorized, const bool _isIntensityHigherThanOne) :
  m_program(_program),
  laserCloud(nullptr),
  laserCloudPCL(nullptr),
  laserCloudIBEO(_inputCloud),
  modelVehicle(nullptr),
  modelCloud(),
  emptyMatrix(),
  colorized(_colorized),
  isIntensityInterger(_isIntensityHigherThanOne),
  solid(!_gradient),
  isRGB(false),
  ignoreSolid(false),
  angularResolution(_angularResolutionDEGREES),
  verticalAngle(_verticalAngleBetweenLayersDEGREES),
  isPointDAI(true),
  pointSize(_pointSize)
{
  setInitValues();
}

LaserClouds::~LaserClouds(){
  cloud.destroy();
}

void LaserClouds::ChangeInput(const vector<LaserCloudData> *const _newInputCloud){
  laserCloud = _newInputCloud;
  laserCloudPCL = nullptr;
  updateCloud();
}

void LaserClouds::ChangeInput(const vector<pointRGB> *const _newInputCloud){
  laserCloudPCL = _newInputCloud;
  laserCloud = nullptr;
  updateCloud();
}

void LaserClouds::setTransformationMatrix(const QMatrix4x4 *const _transformationMatrix){
  modelVehicle = _transformationMatrix;
}

void LaserClouds::setOptions(const bool _gradient, const bool _colorized, const bool _ignoreSolidColor, const bool _isIntensityHigherThanOne){
  solid = !_gradient;
  colorized = _colorized;
  isIntensityInterger = _isIntensityHigherThanOne;
  ignoreSolid = _ignoreSolidColor;
}

void LaserClouds::setSolidColor(const float _SolidColor[3], const bool _gradient){
  for(int i = 1; i < 10; i++)
    palette[i] = QVector3D();
  palette[0][0] = _SolidColor[0]/255.0f;
  palette[0][1] = _SolidColor[1]/255.0f;
  palette[0][2] = _SolidColor[2]/255.0f;

  num_colors = 1;
  solid = !_gradient;
}

bool LaserClouds::ignoreSolidColor(const bool _ignore){
  ignoreSolid = _ignore;
}

void LaserClouds::setLaserColorPalette(vector<array<float, 3> > _palette, const bool _gradient){
  num_colors = (_palette.size() < 10)? _palette.size() : 10;
  solid = !_gradient;

  for(int i = 0; i < 10; i++){
    if(i < num_colors)
      palette[i] = QVector3D(_palette[i][0], _palette[i][1], _palette[i][2])/255.0f;
    else
      palette[i] = QVector3D();
  }
}

void LaserClouds::translateLaserMatrix(const float _x, const float _y, const float _z){
  modelCloud.translate(-_y, _z, -_x);
}

void LaserClouds::rotateLaserMatrix(const float _pitch, const float _yaw, const float _roll){
  modelCloud.rotate(QQuaternion::fromEulerAngles(_pitch * toDEGREES, _yaw * toDEGREES, _roll * toDEGREES));
}

void LaserClouds::updateCloud(){
  m_program->bind();
  cloud.bind();
  if(laserCloud != nullptr){
    sizePoints = laserCloud->size();
//    sizePoints = 10000;
//    qWarning("this");
    cloud.allocate(laserCloud->data(), sizePoints * sizeof(LaserCloudData));
  }else if(laserCloudPCL != nullptr){
    sizePoints = laserCloudPCL->size();
    cloud.allocate(laserCloudPCL->data(), sizePoints * sizeof(pointRGB));
  }else if(laserCloudIBEO != nullptr){
    sizePoints = laserCloudIBEO->size();
    cloud.allocate(laserCloudIBEO->data(), sizePoints * sizeof(pointDAI));
  }
  m_program->release();
}

void LaserClouds::setInitValues(){
  //obtaining the numeric id of the shader's variables
  i_position = m_program->attributeLocation("i_position");
  i_intensity = m_program->attributeLocation("i_intensity");
  i_color = m_program->attributeLocation("i_color");
  u_model = m_program->uniformLocation("u_model");
  u_vehicle = m_program->uniformLocation("u_vehicle");
  u_256range = m_program->uniformLocation("u_range");
  u_solid = m_program->uniformLocation("u_solid");
  u_colorize = m_program->uniformLocation("u_colorize");
  u_palette = m_program->uniformLocation("u_palette");
  u_numColors = m_program->uniformLocation("u_numColors");
  u_rgb = m_program->uniformLocation("u_rgb");
  u_ignore = m_program->uniformLocation("u_ignore");

  i_distance = m_program->attributeLocation("i_distance");
  i_angle = m_program->attributeLocation("i_angle");
  i_layer = m_program->attributeLocation("i_layer");
  u_angularResolution = m_program->uniformLocation("u_angularResolution");
  u_verticalAngle = m_program->uniformLocation("u_verticalAngle");
  u_isPointDAI = m_program->uniformLocation("u_isPointDAI");

  //initializing the color palette
  palette[0] = QVector3D(0.2, 0.5, 0.7); //grayish blue
  palette[1] = QVector3D(0, 1, 0); //green
  palette[2] = QVector3D(1, 1, 0); //yellow
  palette[3] = QVector3D(1, 0, 0); //red

  num_colors = 4;

  //creating the openGL buffers
  cloud.create();
}

void LaserClouds::drawCloud(){

  glPointSize(pointSize);
  //this is to disable the depth test in openGL, it will make all the points
  //to be drawn on top of all the previously drawn objects
  //glDisable(GL_DEPTH_TEST);

  if(modelVehicle != nullptr)
    m_program->setUniformValue(u_vehicle, *modelVehicle);
  else
    m_program->setUniformValue(u_vehicle, emptyMatrix);

  m_program->setUniformValue(u_model, modelCloud);
  m_program->setUniformValue(u_256range, isIntensityInterger);
  m_program->setUniformValue(u_solid, solid);
  m_program->setUniformValue(u_colorize, colorized);
  m_program->setUniformValue(u_numColors, num_colors);
  m_program->setUniformValue(u_rgb, isRGB);
  m_program->setUniformValue(u_ignore, ignoreSolid);
  m_program->setUniformValueArray(u_palette, palette, 10);

  m_program->setUniformValue(u_angularResolution, angularResolution);
  m_program->setUniformValue(u_verticalAngle, verticalAngle);
  m_program->setUniformValue(u_isPointDAI, isPointDAI);

  cloud.bind();

  // Offset for position
  quintptr offset = 0;

  if(!isRGB){
    //Tell OpenGL programmable pipeline where find vertex positions
    m_program->setAttributeBuffer(i_position, GL_FLOAT, offset, 3, sizeof(LaserCloudData));

    offset += 3 * sizeof(float);
    m_program->setAttributeBuffer(i_intensity, GL_FLOAT, offset, 1, sizeof(LaserCloudData));
  }else if(isPointDAI){
    //Tell OpenGL programmable pipeline where find vertex positions
    m_program->setAttributeBuffer(i_distance, GL_FLOAT, offset, 1, sizeof(pointDAI));

    offset += sizeof(float);
    m_program->setAttributeBuffer(i_angle, GL_FLOAT, offset, 1, sizeof(pointDAI));

    offset += sizeof(float);
    m_program->setAttributeBuffer(i_intensity, GL_FLOAT, offset, 1, sizeof(pointDAI));

    offset += sizeof(float);
    m_program->setAttributeBuffer(i_layer, GL_FLOAT, offset, 1, sizeof(pointDAI));
  }else{
    //Tell OpenGL programmable pipeline where find vertex positions
    m_program->setAttributeBuffer(i_position, GL_FLOAT, offset, 3, sizeof(pointRGB));

    offset += 3 * sizeof(float);
    m_program->setAttributeBuffer(i_color, GL_FLOAT, offset, 3, sizeof(pointRGB));

    offset += 3 * sizeof(float);
    m_program->setAttributeBuffer(i_intensity, GL_FLOAT, offset, 1, sizeof(pointRGB));
  }

  if(!isPointDAI){
    m_program->enableAttributeArray(i_position);
    m_program->enableAttributeArray(i_intensity);
    if(isRGB)
      m_program->enableAttributeArray(i_color);
  }else{
    m_program->enableAttributeArray(i_distance);
    m_program->enableAttributeArray(i_angle);
    m_program->enableAttributeArray(i_intensity);
    m_program->enableAttributeArray(i_layer);
  }

  //Draw point using indices from VBO
  glDrawArrays(GL_POINTS, 0, sizePoints);

  if(!isPointDAI){
    if(isRGB)
      m_program->disableAttributeArray(i_color);
    m_program->disableAttributeArray(i_intensity);
    m_program->disableAttributeArray(i_position);
  }else{
    m_program->disableAttributeArray(i_layer);
    m_program->disableAttributeArray(i_intensity);
    m_program->disableAttributeArray(i_angle);
    m_program->disableAttributeArray(i_distance);
  }

  //glEnable(GL_DEPTH_TEST);
  glPointSize(1.0f);
}
