#include "headers/movingobjects.h"

MovingObjects::MovingObjects(QOpenGLShaderProgram *_program, const vector<Obstacle> *const _obstacles) :
  m_program(_program),
  obstacles(_obstacles),
  indexObstacleBuffer(QOpenGLBuffer::IndexBuffer),
  indexArrowsBuffer(QOpenGLBuffer::IndexBuffer),
  transformationMatrixPtr(nullptr)
{
  obstacleBuffer.create();
  indexObstacleBuffer.create();

  arrowsBuffer.create();
  indexArrowsBuffer.create();

  //value attributes for openGL shaders
  i_position = m_program->attributeLocation("i_position");
  i_color = m_program->attributeLocation("i_color");
  i_normal = m_program->attributeLocation("i_normal");
  u_model = m_program->uniformLocation("u_model");
  u_vehicle = m_program->uniformLocation("u_vehicle");

  //creation of an extra vector to determine the position of the cube vertices before all transformations
  //the cube has a size of 1 cubic unit, the center of the cube is placed in the coordinates (0,0,0)
  nP[0] = QVector3D(-0.5,-0.5, 0.5);
  nP[1] = QVector3D(-0.5, 0.5, 0.5);
  nP[2] = QVector3D( 0.5, 0.5, 0.5);
  nP[3] = QVector3D( 0.5,-0.5, 0.5);
  nP[4] = QVector3D( 0.5,-0.5,-0.5);
  nP[5] = QVector3D( 0.5, 0.5,-0.5);
  nP[6] = QVector3D(-0.5, 0.5,-0.5);
  nP[7] = QVector3D(-0.5,-0.5,-0.5);

  nL[0] = QVector3D( 1,  1, 0);
  nL[1] = QVector3D( 1, -1, 0);
  nL[2] = QVector3D(-1, -1, 0);
  nL[3] = QVector3D(-1,  1, 0);

  nL[4] = QVector3D(0,  1, -1);
  nL[5] = QVector3D(0, -1, -1);
  nL[6] = QVector3D(0, -1,  1);
  nL[7] = QVector3D(0,  1,  1);

  nL[8] = QVector3D(-1, 0, -1);
  nL[9] = QVector3D( 1, 0, -1);
  nL[10] = QVector3D( 1, 0,  1);
  nL[11] = QVector3D(-1, 0,  1);

  //arrow vertices:
  QVector3D pA[13];
  pA[0] = QVector3D(-0.022627,-0.022627,-0.000000);
  pA[1] = QVector3D( 0.000000,-0.000000,-0.500000);
  pA[2] = QVector3D( 0.022627,-0.022627, 0.000000);
  pA[3] = QVector3D( 0.022627, 0.022627, 0.000000);
  pA[4] = QVector3D(-0.022627, 0.022627,-0.000000);
  pA[5] = QVector3D(-0.022627,-0.022627,-0.350000);
  pA[6] = QVector3D( 0.022627,-0.022627,-0.350000);
  pA[7] = QVector3D( 0.022627, 0.022627,-0.350000);
  pA[8] = QVector3D(-0.022627, 0.022627,-0.350000);
  pA[9] = QVector3D(-0.075000,-0.000000,-0.350000);
  pA[10] = QVector3D( 0.000000,-0.075000,-0.350000);
  pA[11] = QVector3D( 0.075000,-0.000000,-0.350000);
  pA[12] = QVector3D( 0.000000, 0.075000,-0.350000);

  //arrow face's normals
  QVector3D nA[9];
  nA[0] = QVector3D(-0.6667,-0.6667,-0.3333);
  nA[1] = QVector3D(-0.6667, 0.6667,-0.3333);
  nA[2] = QVector3D( 0.6667, 0.6667,-0.3333);
  nA[3] = QVector3D( 0.6667,-0.6667,-0.3333);
  nA[4] = QVector3D(-1.0000,-0.0000,-0.0000);
  nA[5] = QVector3D(-0.0000, 1.0000,-0.0000);
  nA[6] = QVector3D( 1.0000, 0.0000, 0.0000);
  nA[7] = QVector3D( 0.0000,-1.0000, 0.0000);
  nA[8] = QVector3D( 0.0000, 0.0000, 1.0000);

  //faces and normals of the arrow
  QVector2D fA[42];
  fA[0] = QVector2D(2,1);   fA[1] = QVector2D(11,1);   fA[2] = QVector2D(10,1);
  fA[3] = QVector2D(10,2);  fA[4] = QVector2D(13,2);   fA[5] = QVector2D(2,2);
  fA[6] = QVector2D(12,3);  fA[7] = QVector2D(2,3);    fA[8] = QVector2D(13,3);
  fA[9] = QVector2D(12,4);  fA[10] = QVector2D(11,4);  fA[11] = QVector2D(2,4);
  fA[12] = QVector2D(5,5);  fA[13] = QVector2D(6,5);   fA[14] = QVector2D(1,5);
  fA[15] = QVector2D(4,6);  fA[16] = QVector2D(8,6);   fA[17] = QVector2D(5,6);
  fA[18] = QVector2D(3,7);  fA[19] = QVector2D(8,7);   fA[20] = QVector2D(4,7);
  fA[21] = QVector2D(1,8);  fA[22] = QVector2D(6,8);   fA[23] = QVector2D(3,8);
  fA[24] = QVector2D(10,9); fA[25] = QVector2D(12,9);  fA[26] = QVector2D(13,9);
  fA[27] = QVector2D(12,9); fA[28] = QVector2D(10,9);  fA[29] = QVector2D(11,9);
  fA[30] = QVector2D(5,6);  fA[31] = QVector2D(8,6);   fA[32] = QVector2D(9,6);
  fA[33] = QVector2D(6,5);  fA[34] = QVector2D(5,5);   fA[35] = QVector2D(9,5);
  fA[36] = QVector2D(3,8);  fA[37] = QVector2D(6,8);   fA[38] = QVector2D(7,8);
  fA[39] = QVector2D(8,7);  fA[40] = QVector2D(3,7);   fA[41] = QVector2D(7,7);

  for(int i = 0; i < 42; i++){
    arrow[i].position = pA[(int)fA[i][0] - 1];
    arrow[i].normal = nA[(int)fA[i][1] - 1];
  }

  //creation of an extra array to indicate which vertices constitute face corners for a cube
  //       6 ------------- 5
  //      /|              /|
  //     / |             / |
  //    /  |            /  |
  //   /   |           /   |
  //  1 ------------- 2    |
  //  |    |          |    |
  //  |    7 ---------|--- 4
  //  |   /           |   /
  //  |  /            |  /
  //  | /             | /
  //  |/              |/
  //  0 ------------- 3
  nF1[0] = 0;  nF1[4] = 7;   nF1[8] = 3;  nF1[12] = 0;  nF1[16] = 2;  nF1[20] = 3;
  nF1[1] = 1;  nF1[5] = 6;   nF1[9] = 2;  nF1[13] = 1;  nF1[17] = 1;  nF1[21] = 0;
  nF1[2] = 2;  nF1[6] = 5;  nF1[10] = 5;  nF1[14] = 6;  nF1[18] = 6;  nF1[22] = 7;
  nF1[3] = 3;  nF1[7] = 4;  nF1[11] = 4;  nF1[15] = 7;  nF1[19] = 5;  nF1[23] = 4;

  //this array defines which points will serve to create normal, tangent and bitangent lines for each face
  //int nF2[18] = {0,1,3, 4,5,7, 3,2,4, 7,6,0, 2,1,5, 0,3,7};
  nF2[0] = 0;  nF2[3] = 4;  nF2[6] = 3;   nF2[9] = 7;  nF2[12] = 2;  nF2[15] = 0;
  nF2[1] = 3;  nF2[4] = 7;  nF2[7] = 2;  nF2[10] = 6;  nF2[13] = 1;  nF2[16] = 3;
  nF2[2] = 1;  nF2[5] = 5;  nF2[8] = 4;  nF2[11] = 0;  nF2[14] = 5;  nF2[17] = 7;

  //creation of an extra array to indicate which vertices create an entire face
  //4 extra vertices will be created per face, there are 6 faces.
  //the next vertices are per face, 8 vertices each face, this will be use for GL_TRIANGLE_STRIP
  //vertices: 0 - 3 are external, 4 - 7 are internal
  //  1 ------------------ 2
  //  | 5 -------------- 6 |
  //  | |                | |
  //  | |                | |
  //  | |                | |
  //  | |                | |
  //  | |                | |
  //  | |                | |
  //  | 4 -------------- 7 |
  //  0 ------------------ 3
  //the first and last point shall be repeated to indicate beginning and end of GL_TRIANGLE_STRIP
  nF3[0] = 0;  nF3[6] = 6;
  nF3[1] = 0;  nF3[7] = 3;
  nF3[2] = 4;  nF3[8] = 7;
  nF3[3] = 1;  nF3[9] = 0;
  nF3[4] = 5;  nF3[10] = 4;
  nF3[5] = 2;  nF3[11] = 4;

  nN2[0] = QVector3D( 0, 0, 1);
  nN2[1] = QVector3D( 0, 0,-1);
  nN2[2] = QVector3D( 1, 0, 0);
  nN2[3] = QVector3D(-1, 0, 0);
  nN2[4] = QVector3D( 0, 1, 0);
  nN2[5] = QVector3D( 0,-1, 0);

  m_program->setUniformValue(u_model, transformationMatrix);

  updateObstacle();
}

MovingObjects::~MovingObjects(){
  obstacleBuffer.destroy();
  indexObstacleBuffer.destroy();
  arrowsBuffer.destroy();
  indexArrowsBuffer.destroy();
}

void MovingObjects::changeInput(const vector<Obstacle> *const _newInputObstacle){
  obstacles = _newInputObstacle;
  updateObstacle();
}

void MovingObjects::setTransformationMatrix(const QMatrix4x4 *const _transformationMatrix){
  transformationMatrixPtr = _transformationMatrix;
}

void MovingObjects::translateObstacleMatrix(const float _translationX, const float _translationY, const float _translationZ){
  transformationMatrix.translate(-_translationY, _translationZ, -_translationX);
}

void MovingObjects::rotateObstacleMatrix(const float _pitch, const float _yaw, const float _roll){
  transformationMatrix.rotate(QQuaternion::fromEulerAngles(_pitch * toDEGREES, _yaw * toDEGREES, _roll * toDEGREES));
}

void MovingObjects::updateObstacle(){
  int pA = 0, cA = 0, tA = 0;

  //this function is to count the number of cubes in each frame and it multiplies for
  //48 times (this is because each cube has 48 vertices)
  int tC = obstacles->size() * 48;
  for(int i = 0; i < obstacles->size(); i++)
    if(obstacles->at(i).arrow)
      tA++;
  tA *= 42;

  //creating a temporal varibales to avoid declaring it each loop
  vector<ObjectData> cube(tC);
  vector<ObjectData> arrows(tA);
  QVector<unsigned int> indices;
  QVector<unsigned int> indicesA;
  //rotation matrix
  QMatrix4x4 matrix, matrix2;
  //orientation quaternion
  QQuaternion rotation, rotation2;
  //creation of 48 vertices
  QVector3D positions[48];
  //temporal dimension
  QVector3D dimension;
  //temporal position
  QVector3D position;
  //temporal for face normals
  QVector3D normF;
  //temporal for color
  QVector3D tColor;
  //line width
  float lnWidth;

//  QElapsedTimer timer;
//  timer.start();

  //vertices calculation of every movable object in scene
  for(int z = 0; z < obstacles->size(); z++){
    int qr = 48 * z;

    rotation = QQuaternion::fromEulerAngles(obstacles->at(z).pitch * toDEGREES, obstacles->at(z).yaw * toDEGREES, obstacles->at(z).roll * toDEGREES).normalized();
    matrix.setToIdentity();
    matrix.rotate(rotation);

    rotation2 = QQuaternion::fromEulerAngles(obstacles->at(z).arrow_pitch * toDEGREES, obstacles->at(z).arrow_yaw * toDEGREES - 90, obstacles->at(z).arrow_roll * toDEGREES).normalized();
    matrix2.setToIdentity();
    matrix2.rotate(rotation2);

    dimension = QVector3D(obstacles->at(z).width, obstacles->at(z).height, obstacles->at(z).length);
    position = QVector3D(-obstacles->at(z).positionY, obstacles->at(z).positionZ, -obstacles->at(z).positionX);
    tColor = QVector3D(obstacles->at(z).r, obstacles->at(z).g, obstacles->at(z).b)/255.0f;
    lnWidth = obstacles->at(z).lineWidth;

    //this creates the cube faces with eight vertex points
    for(int i = 0; i < 6; i++){
      int p1 = i*4;
      int p4 = i*8;

      //before this part only exists eight points per cube, it is necessary to have eight points per face
      //4 points represent the exterior vertices and 4 the interior vertices for a line width of variable "l"
      //the line thickness is represented by polygons, because of this it must exist 8 points per face
      //there are six faces per cube, therefore 48 points are created.
      for(int e = 0; e < 8; e++){
        int pe = p4 + e;
        int pf = p1 + e;
        pf = (e < 4)? pf : pf-4;
        positions[pe] = nP[nF1[pf]];
        if(e >= 4){
          if(i < 2){
            positions[pe] += (nL[e - 4] * lnWidth)/dimension;
          }else if(i < 4){
            positions[pe] += (nL[e] * lnWidth)/dimension;
          }else{
            positions[pe] += (nL[e + 4] * lnWidth)/dimension;
          }
        }
        //point scaling
        positions[pe] *= dimension;
        //point rotation and transformation matrix
        positions[pe] = matrix * positions[pe];
        //point translation
        positions[pe] += position - QVector3D(0, -dimension[1]/2.0f, 0);
      }

      //creates normal line for each face
      normF = nN2[i];
      normF = matrix * normF;

      //normalizing vectors
      normF.normalize();

      //add all the information to the array of vertices compatible with GLSL shader
      for(int e = 0; e < 8; e++){
        int pq = p4 + e;
        cube[pA].position = positions[pq];
        cube[pA].color = tColor;
        cube[pA++].normal = normF;
      }
      //add all the indices for points in an array compatible with GLSL shader
      //the indices indicates the order of points in the polygons
      //the type of polygon to draw is GL_TRIANGLE_STRIP
      //there are 48 points per cube
      for(int u = 0; u < 12; u++){
        indices.append(nF3[u] + p4 + qr);
      }
    }
    if(obstacles->at(z).arrow){
      for(int i = 0; i < 42; i++){
        arrows[cA].position = (matrix2 * (arrow[i].position * dimension)) + position + QVector3D(0, dimension[1]/2.0f, 0);
        arrows[cA].normal = matrix2 * arrow[i].normal;
        arrows[cA].color = tColor;
        indicesA.append(cA++);
      }
    }
  }
//  qWarning("Operations: %u - total time required: %.4f milliseconds", obstacles->size(), 0.000001 * timer.nsecsElapsed());


  // Transfer vertex data to VBO 0
  obstacleBuffer.bind();
  obstacleBuffer.allocate(cube.data(), tC * sizeof(ObjectData));

  // Transfer index data to VBO 1
  sizeObstacles = indices.size();
  indexObstacleBuffer.bind();
  indexObstacleBuffer.allocate(indices.data(), sizeObstacles * sizeof(unsigned int));

  // Transfer vertex data to VBO 0
  arrowsBuffer.bind();
  arrowsBuffer.allocate(arrows.data(), tA * sizeof(ObjectData));

  // Transfer index data to VBO 1
  sizeArrows = indicesA.size();
  indexArrowsBuffer.bind();
  indexArrowsBuffer.allocate(indicesA.data(), sizeArrows * sizeof(unsigned int));
}

void MovingObjects::drawObstacle(){
  // Offset for position
  quintptr offset = 0;

  if(transformationMatrixPtr != nullptr)
    m_program->setUniformValue(u_vehicle, *transformationMatrixPtr);
  else
    m_program->setUniformValue(u_vehicle, emptyMatrx);

  m_program->setUniformValue(u_model, transformationMatrix);

  // Tell OpenGL which VBOs to use
  indexObstacleBuffer.bind();
  obstacleBuffer.bind();

  //Tell OpenGL programmable pipeline where find vertex positions
  m_program->setAttributeBuffer(i_position, GL_FLOAT, offset, 3, sizeof(ObjectData));

  //colors
  offset += sizeof(QVector3D);
  m_program->setAttributeBuffer(i_color, GL_FLOAT, offset, 3, sizeof(ObjectData));

  //normals
  offset += sizeof(QVector3D);
  m_program->setAttributeBuffer(i_normal, GL_FLOAT, offset, 3, sizeof(ObjectData));

  m_program->enableAttributeArray(i_position);
  m_program->enableAttributeArray(i_color);
  m_program->enableAttributeArray(i_normal);

  //Draw cube geometry using indices from VBO
  glDrawElements(GL_TRIANGLE_STRIP, sizeObstacles, GL_UNSIGNED_INT, 0);

  m_program->disableAttributeArray(i_position);
  m_program->disableAttributeArray(i_color);
  m_program->disableAttributeArray(i_normal);

  indexArrowsBuffer.bind();
  arrowsBuffer.bind();

  offset = 0;

  //Tell OpenGL programmable pipeline where find vertex positions
  m_program->setAttributeBuffer(i_position, GL_FLOAT, offset, 3, sizeof(ObjectData));

  //colors
  offset += sizeof(QVector3D);
  m_program->setAttributeBuffer(i_color, GL_FLOAT, offset, 3, sizeof(ObjectData));

  //normals
  offset += sizeof(QVector3D);
  m_program->setAttributeBuffer(i_normal, GL_FLOAT, offset, 3, sizeof(ObjectData));

  m_program->enableAttributeArray(i_position);
  m_program->enableAttributeArray(i_color);
  m_program->enableAttributeArray(i_normal);

  //Draw cube geometry using indices from VBO
  glDrawElements(GL_TRIANGLES, sizeArrows, GL_UNSIGNED_INT, 0);

  m_program->disableAttributeArray(i_position);
  m_program->disableAttributeArray(i_color);
  m_program->disableAttributeArray(i_normal);
}
