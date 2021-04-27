#include "headers/trajectory.h"

Trajectory::Trajectory(QOpenGLShaderProgram *program, const vector<vector<trajectoryData> > *const _trajectory) :
  m_program(program),
  trajectoryPoints(0),
  trajectoryList(_trajectory),
  indexTrajectoryBuf(QOpenGLBuffer::IndexBuffer),
  indexTrajectoryBuf2(QOpenGLBuffer::IndexBuffer),
  transformationMatrixPtr(nullptr)
{
  arrayTrajectoryBuf.create();
  indexTrajectoryBuf.create();
  arrayTrajectoryBuf2.create();
  indexTrajectoryBuf2.create();

  i_position = m_program->attributeLocation("i_position");
  i_color = m_program->attributeLocation("i_color");
  i_texcoor = m_program->attributeLocation("i_texcoor");
  i_type = m_program->attributeLocation("i_type");
  u_model = m_program->uniformLocation("u_model");
  u_model2 = m_program->uniformLocation("u_model2");

  m_program->setUniformValue(u_model2, transformationMatrix);
  updateTrajectory();
}

Trajectory::~Trajectory(){
  arrayTrajectoryBuf.destroy();
  indexTrajectoryBuf.destroy();
  arrayTrajectoryBuf2.destroy();
  indexTrajectoryBuf2.destroy();
}

void Trajectory::setTransformationMatrix(const QMatrix4x4 *const _transformationMatrix){
  transformationMatrixPtr = _transformationMatrix;
}

void Trajectory::translateTrajectoryMatrix(const float _translationX, const float _translationY, const float _translationZ){
  transformationMatrix.translate(-_translationY, _translationZ, -_translationX);
}

void Trajectory::rotateTrajectoryMatrix(const float _pitch, const float _yaw, const float _roll){
  transformationMatrix.rotate(QQuaternion::fromEulerAngles(_pitch * toDEGREES, _yaw * toDEGREES, _roll * toDEGREES));
}


float Trajectory::angle(float _x, float _y){
  float angle = atan(_y/_x);

  if(_x == 0)
    if(_y == 0)
      angle = 0;
    else if(_y > 0)
      angle = _PI2;
    else if(_y < 0)
      angle = _3PI2;
  if(_x < 0 && _y >= 0)
    angle += _PI;
  else if(_x < 0 && _y < 0)
    angle += _PI;
  else if(_x > 0 && _y < 0)
    angle += _2PI;

  return angle;
}

void Trajectory::updateTrajectory(){
  // ------------------------------------------------------------------------------------ //
  // ---------------------------- Creating the trajectories ----------------------------- //
  // ------------------------------------------------------------------------------------ //
  indices.clear();
  indices2.clear();
  trajectoryPoints.clear();
  trajectoryPoints2.clear();

  int sTraj = trajectoryList->size(); //obtaining the quantity of trajectories
  unsigned int pos = 0, pos2 = 0;

  //temporal variable for trajectoryData struct
  trajecData temPoint;
  float point[2], nextPoint[2], prevPoint[2], tempPoint[2];
  float angleOld, angleNew, angleActual;
  float distance, Distance = 0, offset, relativeDistance = 0;
  float divisor;

//  QElapsedTimer timer;
//  timer.start();
//  int spoints2;

  for(int n = 0; n < sTraj; n++){
    int sPoints = trajectoryList->at(n).size();//obtaining the quantity of data points for each street

//    spoints2 = sPoints;

    relativeDistance = 0;
    Distance = 0;
    indices.append(pos);

    for(int i = 0; i < sPoints; i++){
      //creation of the lines
      temPoint.position[0] = -trajectoryList->at(n).at(i).positionY;
      temPoint.position[1] = trajectoryList->at(n).at(i).positionZ + 0.0001*i;
      temPoint.position[2] = -trajectoryList->at(n).at(i).positionX;
      temPoint.color[0] = trajectoryList->at(n).at(i).r;
      temPoint.color[1] = trajectoryList->at(n).at(i).g;
      temPoint.color[2] = trajectoryList->at(n).at(i).b;
      temPoint.type = trajectoryList->at(n).at(i).lineType;
      trajectoryPoints2.append(temPoint);
      if(i > 0)
        indices2.append(pos2);
      indices2.append(pos2++);

      offset = trajectoryList->at(n).at(i).lineWidth/2.0f;

      //trajectory calculation
      point[0] = -trajectoryList->at(n).at(i).positionY;
      point[1] = -trajectoryList->at(n).at(i).positionX;


      if(i == 0){
        prevPoint[0] = point[0];
        prevPoint[1] = point[1];
      }

      if(i < (sPoints - 1)){
        nextPoint[0] = -trajectoryList->at(n).at(i + 1).positionY - point[0];
        nextPoint[1] = -trajectoryList->at(n).at(i + 1).positionX - point[1];

        tempPoint[0] = point[0] - prevPoint[0];
        tempPoint[1] = point[1] - prevPoint[1];

        distance = sqrt(pow(nextPoint[0], 2) + pow(nextPoint[1], 2));

        angleNew = angle(nextPoint[0], nextPoint[1]);
        if(i == 0) angleOld = angleNew;
        if(angleNew < _PI && angleOld > _PI) angleNew += _2PI;
        angleActual = (angleNew - angleOld)/2.0f - _PI2;
        angleActual += angleOld;
        angleOld = angleNew;
      }else{
        angleActual = angleOld - _PI2;
      }
      if(distance >= 0.1f || Distance >= 0.1f){
        divisor = sin(angleNew - angleActual);
        if(divisor < 0.3 && divisor > -0.3) divisor = 1;
        offset /= divisor;

        temPoint.position[0] = point[0] + offset * cos(angleActual);
        temPoint.position[2] = point[1] + offset * sin(angleActual);
        temPoint.texcoor[0] = 0;
        temPoint.texcoor[1] = relativeDistance;
        trajectoryPoints.append(temPoint);
        indices.append(pos++);

        temPoint.position[0] = point[0] - offset * cos(angleActual);
        temPoint.position[2] = point[1] - offset * sin(angleActual);
        temPoint.texcoor[0] = 1;
        trajectoryPoints.append(temPoint);
        indices.append(pos++);
        Distance = 0;
      }else
        Distance += distance;

      relativeDistance += distance;
    }

    if(sPoints > 0)
      indices.append(pos - 1);

    if(sPoints > 0)
      indices2.append(pos2 - 1);
  }
//  qWarning("Operations: %u - total time required: %.4f milliseconds", spoints2, 0.000001 * timer.nsecsElapsed());

  // Transfer index data to VBO 1
  indicesSize = indices.size();
  indexTrajectoryBuf.bind();
  indexTrajectoryBuf.allocate(indices.data(), indicesSize * sizeof(unsigned int));

  arrayTrajectoryBuf.bind();
  arrayTrajectoryBuf.allocate(trajectoryPoints.data(), trajectoryPoints.size() * sizeof(trajecData));

  indicesSize2 = indices2.size();
  indexTrajectoryBuf2.bind();
  indexTrajectoryBuf2.allocate(indices2.data(), indicesSize2 * sizeof(unsigned int));

  arrayTrajectoryBuf2.bind();
  arrayTrajectoryBuf2.allocate(trajectoryPoints2.data(), trajectoryPoints2.size() * sizeof(trajecData));
}

void Trajectory::drawTrajectory(){
  //glDisable(GL_DEPTH_TEST);
  indexTrajectoryBuf.bind();
  arrayTrajectoryBuf.bind();

  if(transformationMatrixPtr != nullptr)
    m_program->setUniformValue(u_model, *transformationMatrixPtr);
  else
    m_program->setUniformValue(u_model, emptyMatrix);
  m_program->setUniformValue(u_model2, transformationMatrix);

  quintptr offset = 0;
  m_program->setAttributeBuffer(i_position, GL_FLOAT, offset, 3, sizeof(trajecData));

  offset += 3 * sizeof(float);
  m_program->setAttributeBuffer(i_color, GL_FLOAT, offset, 3, sizeof(trajecData));

  offset += 3 * sizeof(float);
  m_program->setAttributeBuffer(i_texcoor, GL_FLOAT, offset, 2, sizeof(trajecData));

  offset += 2 * sizeof(float);
  m_program->setAttributeBuffer(i_type, GL_FLOAT, offset, 1, sizeof(trajecData));

  m_program->enableAttributeArray(i_position);
  m_program->enableAttributeArray(i_color);
  m_program->enableAttributeArray(i_texcoor);
  m_program->enableAttributeArray(i_type);

  glDrawElements(GL_TRIANGLE_STRIP, indicesSize, GL_UNSIGNED_INT, NULL);

  m_program->disableAttributeArray(i_type);
  m_program->disableAttributeArray(i_texcoor);
  m_program->disableAttributeArray(i_color);
  m_program->disableAttributeArray(i_position);

  //simple lines
  indexTrajectoryBuf2.bind();
  arrayTrajectoryBuf2.bind();

  offset = 0;
  m_program->setAttributeBuffer(i_position, GL_FLOAT, offset, 3, sizeof(trajecData));

  offset += 3 * sizeof(float);
  m_program->setAttributeBuffer(i_color, GL_FLOAT, offset, 3, sizeof(trajecData));

  offset += 3 * sizeof(float);
  m_program->setAttributeBuffer(i_texcoor, GL_FLOAT, offset, 2, sizeof(trajecData));

  offset += 2 * sizeof(float);
  m_program->setAttributeBuffer(i_type, GL_FLOAT, offset, 1, sizeof(trajecData));

  m_program->enableAttributeArray(i_position);
  m_program->enableAttributeArray(i_color);
  m_program->enableAttributeArray(i_texcoor);
  m_program->enableAttributeArray(i_type);

  glDrawElements(GL_LINES, indicesSize2, GL_UNSIGNED_INT, NULL);

  m_program->disableAttributeArray(i_type);
  m_program->disableAttributeArray(i_texcoor);
  m_program->disableAttributeArray(i_color);
  m_program->disableAttributeArray(i_position);

  //glEnable(GL_DEPTH_TEST);
}
