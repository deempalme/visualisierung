#ifndef TRAJECTORY_H
#define TRAJECTORY_H
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <math.h>
#include <QVector>
#include <vector>

#include <QElapsedTimer>

#include "headers/definitions.h"

using std::vector;

struct trajecData{
  float position[3]; //vertex position
  float color[3];    //vertex color
  float texcoor[2] = {0, 0};  //vertex texture coordinates
  float type = 0;     //vertex texture type
};

class Trajectory{
public:
  Trajectory(QOpenGLShaderProgram *_program, const vector<vector<trajectoryData> > *const _trajectory);
  ~Trajectory();
  void setTransformationMatrix(const QMatrix4x4 *const _transformationMatrix);
  void translateTrajectoryMatrix(const float _translationX = 0, const float _translationY = 0, const float _translationZ = 0);
  void rotateTrajectoryMatrix(const float _pitch = 0, const float _yaw = 0, const float _roll = 0);
  void drawTrajectory();
  void updateTrajectory();

private:
  float angle(float _x, float _y);

  QOpenGLShaderProgram *m_program;
  unsigned int i_position, i_color, i_texcoor, i_type, u_model, u_model2, indicesSize, indicesSize2;

  QVector<unsigned int> indices, indices2;
  QVector<trajecData> trajectoryPoints, trajectoryPoints2;
  const vector<vector<trajectoryData> > *const trajectoryList;

  QOpenGLBuffer arrayTrajectoryBuf, arrayTrajectoryBuf2, indexTrajectoryBuf, indexTrajectoryBuf2;
  const QMatrix4x4 *transformationMatrixPtr;
  QMatrix4x4 transformationMatrix, emptyMatrix;
};

#endif // TRAJECTORY_H
