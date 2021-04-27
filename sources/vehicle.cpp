#include "headers/vehicle.h"

Vehicle::Vehicle(QOpenGLShaderProgram *_program, int *_whichCompany) :
  m_program(_program),
  vehicleIndexBuffer(QOpenGLBuffer::IndexBuffer),
  carWidth(1.7),
  carHeight(2),
  carLenght(4.1),
  carCenter(1.475),
  whichCompany(_whichCompany)
{

  i_position = m_program->attributeLocation("i_position");
  i_texcoor = m_program->attributeLocation("i_texcoor");
  i_normal = m_program->attributeLocation("i_normal");
  u_type = m_program->uniformLocation("u_type");
  u_model = m_program->uniformLocation("u_vehicle");

  glGenTextures(1, &texture1);
  m_program->setUniformValue("diffuseDHL", texture1);

  vehicleBuffer.create();
  vehicleIndexBuffer.create();

  updateData();

  initTextures(QString(":/images/DHL"));
}

Vehicle::~Vehicle(){
  vehicleBuffer.destroy();
  vehicleIndexBuffer.destroy();
}

void Vehicle::updateData(){
  //this values are for the vehicle's vertex coordinates
  QVector3D nP[8] = {
    QVector3D(-carWidth,-carHeight, carLenght)/2,
    QVector3D(-carWidth, carHeight, carLenght)/2,
    QVector3D( carWidth, carHeight, carLenght)/2,
    QVector3D( carWidth,-carHeight, carLenght)/2,
    QVector3D( carWidth,-carHeight,-carLenght)/2,
    QVector3D( carWidth, carHeight,-carLenght)/2,
    QVector3D(-carWidth, carHeight,-carLenght)/2,
    QVector3D(-carWidth,-carHeight,-carLenght)/2
  };

  //this values are for the vehicle's texture coordinates
  QVector3D nN[6] = {
    QVector3D( 0, 0,-1),
    QVector3D( 1, 0, 0),
    QVector3D( 0, 0, 1),
    QVector3D(-1, 0, 0),
    QVector3D( 0,-1, 0),
    QVector3D( 0, 1, 0)
  };

  //this values are for the vehicle's face edges relations with vertex
  int vtF[24] = { 0,1,3,2, 3,2,4,5, 4,5,7,6, 7,6,0,1, 0,3,7,4, 1,2,6,5 };

  //these are for the face points ordering
  int itF[6] = { 0,0,1,2,3,3 };

  int pA = 0, pX = 0;
  unsigned int inxc[36];
  QVector2D mT(0, 0);

  QMatrix4x4 rotm;
  rotm.rotate(QQuaternion::fromEulerAngles(0, -90, 0));

  //this vector contains the car cube to draw
  vehicleData car[24];

  //vertices calculation of car's cube
  for(int i = 0; i < 6; i++){
    int cpf = i * 4;
    for(int w = 0; w < 4; w++){
      int apf = cpf + w;
      car[pA].position = nP[vtF[apf]] + QVector3D(0, carHeight/2.0f - 0.08, 0);
      if((i == 0) || (i == 2)){
        switch(w){
        case 0:
          mT = QVector2D(0.9, 2.1);
        break;
        case 1:
          mT = QVector2D(0.9, 0.9);
        break;
        case 2:
          mT = QVector2D(0.1, 2.1);
        break;
        case 3:
          mT = QVector2D(0.1, 0.9);
        break;
        }
        car[pA].texcoor = mT;
        mT = QVector2D(0, 0);
      }else{
        car[pA].texcoor = mT;
      }
      car[pA].normal = nN[i];
      pA++;
    }
    for(int d = 0; d < 6; d++){
      inxc[pX++] = cpf + itF[d];
    }
  }

  // Transfer vertex data to VBO 0
  vehicleBuffer.bind();
  vehicleBuffer.allocate(car, 24 * sizeof(vehicleData));

  // Transfer index data to VBO 1
  vehicleIndexBuffer.bind();
  vehicleIndexBuffer.allocate(inxc, 36 * sizeof(unsigned int));
}

void Vehicle::changeSize(const float _width, const float _height, const float _length){
  carWidth = _width;
  carHeight = _height;
  carLenght = _length;

  updateData();
}

void Vehicle::translateVehicle(const float _x, const float _y, const float _z){
  translation = QVector3D(-_y, _z, -_x);
  modelMatrix.setToIdentity();
  modelMatrix.translate(translation);
}

void Vehicle::applyAcceleration(const float _acceleration){
  QVector3D translation2 = QVector3D(translation[0], translation[1] + _acceleration, translation[2]);
  modelMatrix.setToIdentity();
  modelMatrix.translate(translation2);
}

void Vehicle::drawVehicle(){
  vehicleBuffer.bind();
  vehicleIndexBuffer.bind();
  texture->bind(texture1);

  m_program->setUniformValue(u_type, *whichCompany);
  m_program->setUniformValue(u_model, modelMatrix);

  // Offset for position
  quintptr offset = 0;

  //Tell OpenGL programmable pipeline where find vertex positions
  m_program->setAttributeBuffer(i_position, GL_FLOAT, offset, 3, sizeof(vehicleData));

  //colors
  offset += sizeof(QVector3D);
  m_program->setAttributeBuffer(i_texcoor, GL_FLOAT, offset, 2, sizeof(vehicleData));

  //normals
  offset += sizeof(QVector2D);
  m_program->setAttributeBuffer(i_normal, GL_FLOAT, offset, 3, sizeof(vehicleData));

  m_program->enableAttributeArray(i_position);
  m_program->enableAttributeArray(i_texcoor);
  m_program->enableAttributeArray(i_normal);

  //Draw cube geometry using indices from VBO
  glDrawElements(GL_TRIANGLE_STRIP, 36, GL_UNSIGNED_INT, 0);

  m_program->disableAttributeArray(i_position);
  m_program->disableAttributeArray(i_texcoor);
  m_program->disableAttributeArray(i_normal);
}

void Vehicle::initTextures(const QString _image){
  texture = new QOpenGLTexture(QImage(_image));

  // Set nearest filtering mode for texture minification
  texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
  // Set bilinear filtering mode for texture magnification
  texture->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
  texture->setWrapMode(QOpenGLTexture::Repeat);
  texture->setAutoMipMapGenerationEnabled(true);
  texture->setMaximumAnisotropy(16);
}
