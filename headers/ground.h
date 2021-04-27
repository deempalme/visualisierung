#ifndef GROUND_H
#define GROUND_H

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QVector3D>
#include <QMatrix4x4>
#include <vector>
#include <math.h>

#include <QElapsedTimer>

#include "headers/definitions.h"

#ifndef CUBES_S
#define CUBES_S
struct GroundCubes{
  //edge coordinates
  float x;
  float y;
  float z;
  //edge color
  float r;
  float g;
  float b;
  //edge normal
  float u;
  float v;
  float w;
};
#endif

class Ground {
public:
  Ground(QOpenGLShaderProgram *_program, const vector<Boxes> *const _boxes, const bool _drawLines);
  ~Ground();
  void setTransformationMatrix(const QMatrix4x4 *const _transformationMatrix = nullptr);
  void translateGround(const float _translationX = 0, const float _translationY = 0,
                       const float _translationZ = 0);
  void rotateGround(const float _pitch = 0, const float _yaw = 0, const float _roll = 0);
  void setLines(const bool _drawLines = true);
  void updateGround();
  void drawGround();

private:
  QOpenGLShaderProgram *m_program;
  QOpenGLBuffer linesBuffer, groundBuffer, indicesGroundBuffer;
  unsigned int sizeGroundBuffer, sizeLineBuffer, u_lines, u_model, u_model2, u_normal, u_normal2,
  i_position, i_color, i_normal, nF[20];
  QVector3D eP[8], eN[5];
  const vector<Boxes> *boxes;
  const QMatrix4x4 *modelMatrix;
  QMatrix4x4 groundMatrix, emptyMatrix;
  QMatrix3x3 modelNormal, emptyNormal, groundNormal;
  bool drawLines, initiated;
};

#endif // GROUND_H
