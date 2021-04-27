#include "headers/ground.h"

Ground::Ground(QOpenGLShaderProgram *_program, const vector<Boxes> *const _boxes, const bool _drawLines) :
  m_program(_program),
  indicesGroundBuffer(QOpenGLBuffer::IndexBuffer),
  boxes(_boxes),
  modelMatrix(nullptr),
  groundMatrix(),
  emptyMatrix(),
  drawLines(false),
  initiated(false)
{
  linesBuffer.create();
  groundBuffer.create();
  indicesGroundBuffer.create();

  u_lines = m_program->uniformLocation("u_lines");
  u_model = m_program->uniformLocation("u_model");
  u_model2 = m_program->uniformLocation("u_model2");
  u_normal = m_program->uniformLocation("u_normal");
  u_normal2 = m_program->uniformLocation("u_normal2");
  i_position = m_program->attributeLocation("i_position");
  i_color = m_program->attributeLocation("i_color");
  i_normal = m_program->attributeLocation("i_normal");

  // ------------------------------------------------------------------------------------ //
  // --------------------------------- cube coordinates --------------------------------- //
  // ------------------------------------------------------------------------------------ //

  //creation of an extra array to indicate which vertices constitute face corners for a cube
  //       3 ------------- 2
  //      /|              /|
  //     / |             / |
  //    /  |            /  |
  //   /   |           /   |
  //  0 ------------- 1    |
  //  |    |          |    |
  //  |    7 ---------|--- 6
  //  |   /           |   /
  //  |  /            |  /
  //  | /       *     | /      <- the origin is at the center bottom
  //  |/              |/
  //  4 ------------- 5
  eP[0] = QVector3D(-0.5, 1.0, 0.5);
  eP[1] = QVector3D( 0.5, 1.0, 0.5);
  eP[2] = QVector3D( 0.5, 1.0,-0.5);
  eP[3] = QVector3D(-0.5, 1.0,-0.5);
  eP[4] = QVector3D(-0.5, 0.0, 0.5);
  eP[5] = QVector3D( 0.5, 0.0, 0.5);
  eP[6] = QVector3D( 0.5, 0.0,-0.5);
  eP[7] = QVector3D(-0.5, 0.0,-0.5);

  //this is to determine which edges conform a face
  //the face is composed by two triangles:
  //  0 --------------- 1
  //  |               # |
  //  |             #   |
  //  |           #     |
  //  |         #       |
  //  |       #         |
  //  |     #           |
  //  |   #             |
  //  | #               |
  //  3 --------------- 2

  nF[0] = 3;  nF[4] = 0;   nF[8] = 1;  nF[12] = 2;  nF[16] = 3;
  nF[1] = 2;  nF[5] = 1;   nF[9] = 5;  nF[13] = 6;  nF[17] = 7;
  nF[2] = 0;  nF[6] = 4;  nF[10] = 2;  nF[14] = 3;  nF[18] = 0;
  nF[3] = 1;  nF[7] = 5;  nF[11] = 6;  nF[15] = 7;  nF[19] = 4;

  //this is for the face normals
  eN[0] = QVector3D( 0, 1, 0);
  eN[1] = QVector3D( 0, 0, 1);
  eN[2] = QVector3D( 1, 0, 0);
  eN[3] = QVector3D( 0, 0,-1);
  eN[4] = QVector3D(-1, 0, 0);

  emptyNormal = emptyMatrix.normalMatrix();

  setLines(_drawLines);
  updateGround();
}

Ground::~Ground(){
  linesBuffer.destroy();
  groundBuffer.destroy();
  indicesGroundBuffer.destroy();
}

void Ground::setTransformationMatrix(const QMatrix4x4 *const _transformationMatrix){
  modelMatrix = _transformationMatrix;
}

void Ground::translateGround(const float _translationX, const float _translationY, const float _translationZ){
  groundMatrix.translate(-_translationY, _translationZ, -_translationX);
  groundNormal = groundMatrix.normalMatrix();
}

void Ground::rotateGround(const float _pitch, const float _yaw, const float _roll){
  groundMatrix.rotate(QQuaternion::fromEulerAngles(_pitch * toDEGREES, _yaw * toDEGREES, _roll * toDEGREES));
  groundNormal = groundMatrix.normalMatrix();
}

void Ground::setLines(const bool _drawLines){
  drawLines = _drawLines;

  if(drawLines && !initiated){
    initiated = true;
    // ------------------------------------------------------------------------------------ //
    // ------------------------------ Creating the lines ---------------------------------- //
    // ------------------------------------------------------------------------------------ //
    int iframeWidth = 1000;
    int iframeLength = 1000;
    float halfWidth = iframeWidth/2.0f;
    float halfLength = iframeLength/2.0f;
    sizeLineBuffer = (iframeWidth + iframeLength + 2) * 2;
    QVector3D LinePoints[sizeLineBuffer];

    int e = 0;
    for(int i = 0; i <= iframeWidth; i += 2){
      LinePoints[e++] = QVector3D(i - halfWidth, 0, -halfLength);
      LinePoints[e++] = QVector3D(i - halfWidth, 0, iframeWidth - halfLength);
    }

    for(int i = 0; i <= iframeLength; i += 2){
      LinePoints[e++] = QVector3D(-halfWidth, 0, i - halfLength);
      LinePoints[e++] = QVector3D(iframeLength - halfWidth, 0, i - halfLength);
    }

    linesBuffer.bind();
    linesBuffer.allocate(LinePoints, sizeLineBuffer * sizeof(QVector3D));

    // ------------------------------------------------------------------------------------ //
    // -------------------------------- End line creation --------------------------------- //
    // ------------------------------------------------------------------------------------ //
  }
}

void Ground::updateGround(){
  vector<GroundCubes> groundPoints;
  vector<unsigned int> indices;
  GroundCubes singleBox;
  unsigned int q = 0, pp = 0;

//  QElapsedTimer timer;
//  timer.start();

  // ------------------------------------------------------------------------------------ //
  // ------------------------------- Creating the squares ------------------------------- //
  // ------------------------------------------------------------------------------------ //
  for(int i = 0; i < boxes->size(); i++){
    pp = groundPoints.size();

    singleBox.r = boxes->at(i).r;
    singleBox.g = boxes->at(i).g;
    singleBox.b = boxes->at(i).b;

    if(boxes->at(i).height != 0.0f){
      for(int u = 0; u < 20; u++){
        singleBox.x = eP[nF[u]][0] * boxes->at(i).width - boxes->at(i).positionY;
        singleBox.y = eP[nF[u]][1] * boxes->at(i).height + boxes->at(i).positionZ;
        singleBox.z = eP[nF[u]][2] * boxes->at(i).length - boxes->at(i).positionX;

        q = floor(u/4.0f);
        singleBox.u = eN[q][0];
        singleBox.v = eN[q][1];
        singleBox.w = eN[q][2];
        groundPoints.push_back(singleBox);
      }
      indices.push_back(pp);
      for(int u = 0; u < 20; u++)
        indices.push_back(pp + u);
      indices.push_back(pp + 19);
    }else{
      for(int u = 0; u < 4; u++){
        singleBox.x = eP[nF[u]][0] * boxes->at(i).width - boxes->at(i).positionY;
        singleBox.y = boxes->at(i).positionZ;
        singleBox.z = eP[nF[u]][2] * boxes->at(i).length - boxes->at(i).positionX;

        singleBox.u = eN[0][0];
        singleBox.v = eN[0][1];
        singleBox.w = eN[0][2];
        groundPoints.push_back(singleBox);
      }
      indices.push_back(pp);
      for(int u = 0; u < 4; u++)
        indices.push_back(pp + u);
      indices.push_back(pp + 3);
    }
  }
//  qWarning("Operations: %u - total time required: %.4f milliseconds", boxes->size(), 0.000001 * timer.nsecsElapsed());
  // ------------------------------------------------------------------------------------ //
  // -------------------------------- End square creation ------------------------------- //
  // ------------------------------------------------------------------------------------ //

  sizeGroundBuffer = indices.size();
  indicesGroundBuffer.bind();
  indicesGroundBuffer.allocate(indices.data(), sizeGroundBuffer * sizeof(unsigned int));

  groundBuffer.bind();
  groundBuffer.allocate(groundPoints.data(), groundPoints.size() * sizeof(GroundCubes));
}

void Ground::drawGround(){
  if(modelMatrix != nullptr){
    modelNormal = modelMatrix->normalMatrix();
    m_program->setUniformValue(u_model, *modelMatrix);
    m_program->setUniformValue(u_normal, modelNormal);
  }else{
    m_program->setUniformValue(u_model, emptyMatrix);
    m_program->setUniformValue(u_normal, emptyNormal);
  }
  m_program->setUniformValue(u_model2, groundMatrix);
  m_program->setUniformValue(u_normal2, groundNormal);

  // Offset for position
  quintptr offset = 0;

  if(drawLines){
    //glDisable(GL_DEPTH_TEST);
//    glLineWidth(3);
    m_program->setUniformValue(u_lines, true);
    linesBuffer.bind();

    //Tell OpenGL programmable pipeline where find vertex positions
    m_program->setAttributeBuffer(i_position, GL_FLOAT, 0, 3, sizeof(QVector3D));

    m_program->enableAttributeArray(i_position);

    //Draw point using indices from VBO
    glDrawArrays(GL_LINES, 0, sizeLineBuffer);

    m_program->disableAttributeArray(i_position);
    //glEnable(GL_DEPTH_TEST);
//    glLineWidth(1);
  }

  // ------------------------------------------------------------------------------------ //
  // --------------------------------- this is for boxes -------------------------------- //
  // ------------------------------------------------------------------------------------ //

  m_program->setUniformValue(u_lines, false);
  // Tell OpenGL which VBOs to use
  indicesGroundBuffer.bind();
  groundBuffer.bind();

  offset = 0;

  //Tell OpenGL programmable pipeline where find vertex positions
  m_program->setAttributeBuffer(i_position, GL_FLOAT, offset, 3, sizeof(GroundCubes));

  //colors
  offset += 3 * sizeof(float);
  m_program->setAttributeBuffer(i_color, GL_FLOAT, offset, 3, sizeof(GroundCubes));

  //normals
  offset += 3 * sizeof(float);
  m_program->setAttributeBuffer(i_normal, GL_FLOAT, offset, 3, sizeof(GroundCubes));

  m_program->enableAttributeArray(i_position);
  m_program->enableAttributeArray(i_color);
  m_program->enableAttributeArray(i_normal);

  //Draw cube geometry using indices from VBO
  glDrawElements(GL_TRIANGLE_STRIP, sizeGroundBuffer, GL_UNSIGNED_INT, 0);

  m_program->disableAttributeArray(i_position);
  m_program->disableAttributeArray(i_color);
  m_program->disableAttributeArray(i_normal);
}
