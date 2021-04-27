#include "headers/cylindricalobjects.h"


CylindricalObjects::CylindricalObjects(QOpenGLShaderProgram *_program,
                                       const vector<Obstacle> *const _cylinders) :
  m_program(_program),
  indexCylindersBuffer(QOpenGLBuffer::IndexBuffer),
  cylinders(_cylinders),
  transformationMatrixPtr(nullptr)
{
  indexCylindersBuffer.create();
  cylindersBuffer.create();

  //value attributes for openGL shaders
  i_position = m_program->attributeLocation("i_position");
  i_color = m_program->attributeLocation("i_color");
  i_normal = m_program->attributeLocation("i_normal");
  u_model = m_program->uniformLocation("u_model");
  u_vehicle = m_program->uniformLocation("u_vehicle");

  //cylinder vertices
  QVector3D pC[64] = {
    QVector3D(-0.097545,-0.100000,-0.490393),
    QVector3D(-0.097545, 0.000000,-0.490393),
    QVector3D( 0.097545,-0.100000,-0.490393),
    QVector3D( 0.097545, 0.000000,-0.490393),
    QVector3D( 0.277785,-0.100000,-0.415735),
    QVector3D( 0.277785, 0.000000,-0.415735),
    QVector3D( 0.415735,-0.100000,-0.277785),
    QVector3D( 0.415735, 0.000000,-0.277785),
    QVector3D( 0.490393,-0.100000,-0.097545),
    QVector3D( 0.490393, 0.000000,-0.097545),
    QVector3D( 0.490393,-0.100000, 0.097545),
    QVector3D( 0.490393, 0.000000, 0.097545),
    QVector3D( 0.415735,-0.100000, 0.277785),
    QVector3D( 0.415735, 0.000000, 0.277785),
    QVector3D( 0.277785,-0.100000, 0.415735),
    QVector3D( 0.277785, 0.000000, 0.415735),
    QVector3D( 0.097545,-0.100000, 0.490393),
    QVector3D( 0.097545, 0.000000, 0.490393),
    QVector3D(-0.097545,-0.100000, 0.490393),
    QVector3D(-0.097545, 0.000000, 0.490393),
    QVector3D(-0.277785,-0.100000, 0.415735),
    QVector3D(-0.277785, 0.000000, 0.415735),
    QVector3D(-0.415735,-0.100000, 0.277785),
    QVector3D(-0.415735, 0.000000, 0.277785),
    QVector3D(-0.490393,-0.100000, 0.097545),
    QVector3D(-0.490393, 0.000000, 0.097545),
    QVector3D(-0.490393,-0.100000,-0.097545),
    QVector3D(-0.490393, 0.000000,-0.097545),
    QVector3D(-0.415735,-0.100000,-0.277785),
    QVector3D(-0.415735, 0.000000,-0.277785),
    QVector3D(-0.277785,-0.100000,-0.415735),
    QVector3D(-0.277785, 0.000000,-0.415735),
    QVector3D(-0.078036, 0.050000,-0.392314),
    QVector3D( 0.078036, 0.050000,-0.392314),
    QVector3D( 0.222228, 0.050000,-0.332588),
    QVector3D( 0.332588, 0.050000,-0.222228),
    QVector3D( 0.392314, 0.050000,-0.078036),
    QVector3D( 0.392314, 0.050000, 0.078036),
    QVector3D( 0.332588, 0.050000, 0.222228),
    QVector3D( 0.222228, 0.050000, 0.332588),
    QVector3D( 0.078036, 0.050000, 0.392314),
    QVector3D(-0.078036, 0.050000, 0.392314),
    QVector3D(-0.222228, 0.050000, 0.332588),
    QVector3D(-0.332588, 0.050000, 0.222228),
    QVector3D(-0.392314, 0.050000, 0.078036),
    QVector3D(-0.392314, 0.050000,-0.078036),
    QVector3D(-0.332588, 0.050000,-0.222228),
    QVector3D(-0.222228, 0.050000,-0.332588),

    QVector3D(-0.050000,-0.100000,-0.490393),
    QVector3D(-0.050000, 0.000000,-0.490393),
    QVector3D( 0.050000,-0.100000,-0.490393),
    QVector3D( 0.050000, 0.000000,-0.490393),
    QVector3D( 0.490393,-0.100000,-0.050000),
    QVector3D( 0.490393, 0.000000,-0.050000),
    QVector3D( 0.490393,-0.100000, 0.050000),
    QVector3D( 0.490393, 0.000000, 0.050000),
    QVector3D( 0.050000,-0.100000, 0.490393),
    QVector3D( 0.050000, 0.000000, 0.490393),
    QVector3D(-0.050000,-0.100000, 0.490393),
    QVector3D(-0.050000, 0.000000, 0.490393),
    QVector3D(-0.490393,-0.100000, 0.050000),
    QVector3D(-0.490393, 0.000000, 0.050000),
    QVector3D(-0.490393,-0.100000,-0.050000),
    QVector3D(-0.490393, 0.000000,-0.050000)
  };

  QVector3D nC[21] = {
    QVector3D( 0.0000, 0.0000,-1.0000),
    QVector3D( 0.3827, 0.0000,-0.9239),
    QVector3D( 0.7071, 0.0000,-0.7071),
    QVector3D( 0.9239, 0.0000,-0.3827),
    QVector3D( 1.0000, 0.0000, 0.0000),
    QVector3D( 0.9239, 0.0000, 0.3827),
    QVector3D( 0.7071, 0.0000, 0.7071),
    QVector3D( 0.3827, 0.0000, 0.9239),
    QVector3D( 0.0000, 0.0000, 1.0000),
    QVector3D(-0.3827, 0.0000, 0.9239),
    QVector3D(-0.7071, 0.0000, 0.7071),
    QVector3D(-0.9239, 0.0000, 0.3827),
    QVector3D(-1.0000, 0.0000, 0.0000),
    QVector3D(-0.9239, 0.0000,-0.3827),
    QVector3D( 0.0000, 1.0000,-0.0000),
    QVector3D(-0.7071, 0.0000,-0.7071),
    QVector3D(-0.3827, 0.0000,-0.9239),

    QVector3D( 0.0000, 0.0000,-1.0000),
    QVector3D( 1.0000, 0.0000, 0.0000),
    QVector3D( 0.0000, 0.0000, 1.0000),
    QVector3D(-1.0000, 0.0000, 0.0000)
  };

  unsigned short fC[288] = {
    1, 1, 2, 1, 3, 1, 4, 1,
    3, 2, 4, 2, 5, 2, 6, 2,
    5, 3, 6, 3, 7, 3, 8, 3,
    7, 4, 8, 4, 9, 4, 10, 4,
    9, 5, 10, 5, 11, 5, 12, 5,
    11, 6, 12, 6, 13, 6, 14, 6,
    13, 7, 14, 7, 15, 7, 16, 7,
    15, 8, 16, 8, 17, 8, 18, 8,
    17, 9, 18, 9, 19, 9, 20, 9,
    29, 16, 30, 16, 31, 16, 32, 16,
    31, 17, 32, 17, 1, 17, 2, 17,
    19, 10, 20, 10, 21, 10, 22, 10,
    21, 11, 22, 11, 23, 11, 24, 11,
    23, 12, 24, 12, 25, 12, 26, 12,
    25, 13, 26, 13, 27, 13, 28, 13,
    27, 14, 28, 14, 29, 14, 30, 14,
    20, 15, 18, 15, 42, 15, 41, 15,
    30, 15, 28, 15, 47, 15, 46, 15,
    6, 15, 4, 15, 35, 15, 34, 15,
    16, 15, 14, 15, 40, 15, 39, 15,
    26, 15, 24, 15, 45, 15, 44, 15,
    12, 15, 10, 15, 38, 15, 37, 15,
    22, 15, 20, 15, 43, 15, 42, 15,
    32, 15, 30, 15, 48, 15, 47, 15,
    8, 15, 6, 15, 36, 15, 35, 15,
    18, 15, 16, 15, 41, 15, 40, 15,
    4, 15, 2, 15, 34, 15, 33, 15,
    28, 15, 26, 15, 46, 15, 45, 15,
    14, 15, 12, 15, 39, 15, 38, 15,
    24, 15, 22, 15, 44, 15, 43, 15,
    2, 15, 32, 15, 33, 15, 48, 15,
    10, 15, 8, 15, 37, 15, 36, 15,

    49, 18, 50, 18, 51, 18, 52, 18,
    53, 19, 54, 19, 55, 19, 56, 19,
    57, 20, 58, 20, 59, 20, 60, 20,
    61, 21, 62, 21, 63, 21, 64, 21
  };

  int e, n, m;
  for(int i = 0; i < 128; i++){
    e = i * 2;
    n = fC[e] - 1;
    m = fC[e + 1] - 1;

    cylr[i].position = pC[n];
    cylr[i].normal = nC[m];

    cylr[i + 128].position = pC[n];
    cylr[i + 128].normal = QVector3D(nC[m][0], -nC[m][1], nC[m][2]);
  }

  for(int i = 0; i < 16; i++){
    e = 256 + i * 2;
    n = fC[e] - 1;
    m = fC[e + 1] - 1;

    cylr[i + 256].position = pC[n];
    cylr[i + 256].normal = nC[m];
  }

  updateCylinders();
}

CylindricalObjects::~CylindricalObjects(){
  indexCylindersBuffer.destroy();
  cylindersBuffer.destroy();
}

void CylindricalObjects::changeInput(const vector<Obstacle> *const _newInput){
  cylinders = _newInput;
  updateCylinders();
}

void CylindricalObjects::setTransformationMatrix(const QMatrix4x4 *const _transformationMatrix){
  transformationMatrixPtr = _transformationMatrix;
}

void CylindricalObjects::translateCylindersMatrix(const float _translationX, const float _translationY,
                                                  const float _translationZ){
  transformationMatrix.translate(-_translationY, _translationZ, -_translationX);
}

void CylindricalObjects::rotateCylindersMatrix(const float _pitch, const float _yaw, const float _roll){
  transformationMatrix.rotate(QQuaternion::fromEulerAngles(_pitch * toDEGREES, _yaw * toDEGREES,
                                                           _roll * toDEGREES));
}

void CylindricalObjects::updateCylinders(){

  float pN = 0;
  int tC = cylinders->size() * 272;

  vector<unsigned int> indices;
  vector<CylinderData> cylindrs(tC);
  //rotation matrix
  QMatrix4x4 matrix;
  //temporal dimension
  QVector3D dimension;
  //temporal position
  QVector3D position;
  //temporal for color
  QVector3D tColor;
  //line width
  QVector3D lineW, lineW2;

  for(int z = 0; z < cylinders->size(); z++){
    unsigned int pe = 0;
    matrix.setToIdentity();
    matrix.rotate(QQuaternion::fromEulerAngles(cylinders->at(z).pitch * toDEGREES,
                                               cylinders->at(z).yaw * toDEGREES,
                                               cylinders->at(z).roll * toDEGREES).normalized());

    dimension = QVector3D(cylinders->at(z).width, cylinders->at(z).height, cylinders->at(z).length);
    position = QVector3D(-cylinders->at(z).positionY, cylinders->at(z).positionZ + dimension[1]/2.0f, -cylinders->at(z).positionX);
    tColor = QVector3D(cylinders->at(z).r, cylinders->at(z).g, cylinders->at(z).b)/255.0f;
    lineW = QVector3D(cylinders->at(z).lineWidth / 0.1f, cylinders->at(z).lineWidth / 0.1f, cylinders->at(z).lineWidth / 0.1f) / dimension;
    lineW2 = QVector3D(0.4f / (0.5f - cylinders->at(z).lineWidth / dimension[0]), 0, 0.4f / (0.5f - cylinders->at(z).lineWidth / dimension[2]));

    indices.push_back(pN);

    for(int i = 0; i < 256; i++){
      if(i < 128){
        if(cylr[i].position[1] < 0)
          cylindrs[pN].position = QVector3D(cylr[i].position[0], 0.5 + cylr[i].position[1] * lineW[1], cylr[i].position[2]);
        else if(cylr[i].position[1] > 0)
          cylindrs[pN].position = QVector3D(cylr[i].position[0] / lineW2[0], 0.5, cylr[i].position[2] / lineW2[2]);
        else
          cylindrs[pN].position = QVector3D(cylr[i].position[0], 0.5, cylr[i].position[2]);
      }else{
        if(cylr[i].position[1] < 0)
          cylindrs[pN].position = QVector3D(cylr[i].position[0], -0.5 - cylr[i].position[1] * lineW[1], cylr[i].position[2]);
        else if(cylr[i].position[1] > 0)
          cylindrs[pN].position = QVector3D(cylr[i].position[0] / lineW2[0], -0.5, cylr[i].position[2] / lineW2[2]);
        else
          cylindrs[pN].position = QVector3D(cylr[i].position[0], -0.5, cylr[i].position[2]);
      }
      cylindrs[pN].position *= dimension;
      cylindrs[pN].position = matrix * cylindrs[pN].position;
      cylindrs[pN].position += position;
      cylindrs[pN].normal = matrix * cylr[i].normal;
      cylindrs[pN].color = tColor;

      if(pe >= 3){
        indices.push_back(pN);
        pe = 0;
      }else if(pe == 0){
        indices.push_back(pN);
        pe++;
      }else
        pe++;
      indices.push_back(pN++);
    }

    pe = 0;

    for(int i = 0; i < 16; i++){
      if(cylr[i + 256].position[1] < 0)
        if((cylr[i + 256].position[0] > 0.1) || (cylr[i + 256].position[0] < -0.1))
          cylindrs[pN].position = QVector3D(cylr[i + 256].position[0], -0.5, cylr[i + 256].position[2] * lineW[2]);
        else
          cylindrs[pN].position = QVector3D(cylr[i + 256].position[0] * lineW[0], -0.5, cylr[i + 256].position[2]);
      else
        if((cylr[i + 256].position[0] > 0.1) || (cylr[i + 256].position[0] < -0.1))
          cylindrs[pN].position = QVector3D(cylr[i + 256].position[0], 0.5, cylr[i + 256].position[2] * lineW[2]);
        else
          cylindrs[pN].position = QVector3D(cylr[i + 256].position[0] * lineW[0], 0.5, cylr[i + 256].position[2]);

      cylindrs[pN].position *= dimension;
      cylindrs[pN].position = matrix * cylindrs[pN].position;
      cylindrs[pN].position += position;
      cylindrs[pN].normal = matrix * cylr[i + 256].normal;
      cylindrs[pN].color = tColor;

      if(pe >= 3){
        indices.push_back(pN);
        pe = 0;
      }else if(pe == 0){
        indices.push_back(pN);
        pe++;
      }else
        pe++;
      indices.push_back(pN++);
    }
  }

  // Transfer vertex data to VBO 0
  cylindersBuffer.bind();
  cylindersBuffer.allocate(cylindrs.data(), tC * sizeof(CylinderData));

  // Transfer index data to VBO 1
  sizeCylinders = indices.size();
  indexCylindersBuffer.bind();
  indexCylindersBuffer.allocate(indices.data(), sizeCylinders * sizeof(unsigned int));
}

void CylindricalObjects::drawCylinders(){
  // Offset for position
  quintptr offset = 0;

  if(transformationMatrixPtr != nullptr)
    m_program->setUniformValue(u_vehicle, *transformationMatrixPtr);
  else
    m_program->setUniformValue(u_vehicle, emptyMatrix);

  m_program->setUniformValue(u_model, transformationMatrix);

  // Tell OpenGL which VBOs to use
  indexCylindersBuffer.bind();
  cylindersBuffer.bind();

  //Tell OpenGL programmable pipeline where find vertex positions
  m_program->setAttributeBuffer(i_position, GL_FLOAT, offset, 3, sizeof(CylinderData));

  //colors
  offset += sizeof(QVector3D);
  m_program->setAttributeBuffer(i_color, GL_FLOAT, offset, 3, sizeof(CylinderData));

  //normals
  offset += sizeof(QVector3D);
  m_program->setAttributeBuffer(i_normal, GL_FLOAT, offset, 3, sizeof(CylinderData));

  m_program->enableAttributeArray(i_position);
  m_program->enableAttributeArray(i_color);
  m_program->enableAttributeArray(i_normal);

  //Draw cube geometry using indices from VBO
  glDrawElements(GL_TRIANGLE_STRIP, sizeCylinders, GL_UNSIGNED_INT, 0);

  m_program->disableAttributeArray(i_position);
  m_program->disableAttributeArray(i_color);
  m_program->disableAttributeArray(i_normal);
}
