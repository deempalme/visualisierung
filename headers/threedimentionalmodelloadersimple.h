#ifndef THREEDIMENTIONALMODELLOADERSIMPLE_H
#define THREEDIMENTIONALMODELLOADERSIMPLE_H

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QQuaternion>
#include <QMatrix4x4>
#include <QVector>
#include <QFile>
#include <QApplication>

#include "headers/parameters.h"
#include "headers/definitions.h"

class ThreeDimentionalModelLoaderSimple
{
public:
  ThreeDimentionalModelLoaderSimple(unsigned int _type, QOpenGLShaderProgram *_program);
  ~ThreeDimentionalModelLoaderSimple();
  void changePosition(const float _xposition = 0, const float _yposition = 0, const float _zposition = 0);
  void applyAcceleration(const float _acceleration = 0);
  void changeRotation(const float _pitch = 0, const float _yaw = 0, const float _roll = 0);
  void changeSize(const float _width = 1, const float _height = 1, const float _length = 1);
  void drawModel();

private:
  bool readData();

  QOpenGLShaderProgram *m_program;

  QVector3D translation, scalation;
  QQuaternion rotation, rotation2;
  QMatrix4x4 m_model, m_model2;
  QMatrix3x3 m_normal, m_normal2;
  QOpenGLBuffer modelData;
  QString fileName, fileString;
  unsigned int dataSize, i_position, i_normal, i_uv, u_normal,
  u_normal2, u_model, u_model2, u_scale, type;
};

#endif // THREEDIMENTIONALMODELLOADERSIMPLE_H
