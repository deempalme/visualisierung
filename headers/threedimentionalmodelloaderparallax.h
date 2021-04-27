#ifndef THREEDIMENTIONALMODELLOADERPARALLAX_H
#define THREEDIMENTIONALMODELLOADERPARALLAX_H

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QQuaternion>
#include <QMatrix4x4>
#include <QVector>
#include <QFile>
#include <QApplication>

#include "headers/parameters.h"
#include "headers/definitions.h"

class ThreeDimentionalModelLoaderParallax
{
public:
  ThreeDimentionalModelLoaderParallax(unsigned int _type, QOpenGLShaderProgram *_program);
  ~ThreeDimentionalModelLoaderParallax();
  void changePosition(const float _xposition = 0, const float _yposition = 0, const float _zposition = 0);
  void changeRotation(const float _pitch = 0, const float _yaw = 0, const float _roll = 0);
  void changeSize(float _width = 1, float _height = 1, float _length = -1);
  void drawModel();

private:
  bool readData();

  QOpenGLShaderProgram *m_program;

  QVector3D translation, scalation;
  QQuaternion rotation;
  QMatrix4x4 m_model;
  QMatrix3x3 m_normal;
  QOpenGLBuffer modelData;
  QString fileName, fileString;
  unsigned int dataSize, i_position, i_uv, i_normal, i_tangent, i_bitangent, u_normal, u_model, u_scale, type;
  bool left;
};

#endif // THREEDIMENTIONALMODELLOADERPARALLAX_H
