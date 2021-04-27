#ifndef THREEDIMENTIONALMODELLOADERNORMALS_H
#define THREEDIMENTIONALMODELLOADERNORMALS_H

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QQuaternion>
#include <QMatrix4x4>
#include <QVector>
#include <QFile>
#include <QApplication>

#include "headers/parameters.h"
#include "headers/definitions.h"

class ThreeDimentionalModelLoaderNormals
{
public:
  ThreeDimentionalModelLoaderNormals(unsigned int _type, QOpenGLShaderProgram *_program);
  ~ThreeDimentionalModelLoaderNormals();
  void changePosition(const float _xposition = 0, const float _yposition = 0, const float _zposition = 0);
  void applyAcceleration(const float _acceleration = 0);
  void changeRotation(const float _pitch = 0, const float _yaw = 0, const float _roll = 0);
  void changeSize(const float _width = 1, const float _height = 1, const float _length = 1);
  void changeDimensions(const float _width = -1, const float _length = -1, const float _height = -1);
  void changeChassis(const float _A = 0.20, const float _B = 0.65, const float _C = 1.15,
                     const float _D = 1.15, const float _E = 0.20, const float _F = 1.50);
  void drawModel();

private:
  bool readData(const bool _dimensions = false, const float _A = 0.20, const float _B = 0.65,
                const float _C = 1.15, const float _D = 1.15, const float _E = 0.20, const float _F = 1.50);

  QOpenGLShaderProgram *m_program;

  QVector3D translation, scalation, acceleration;
  QQuaternion rotation, rotation2;
  QMatrix4x4 m_model, m_model2;
  QMatrix3x3 m_normal, m_normal2;
  QOpenGLBuffer modelData;
  QString fileName, fileString;
  int dataSize, i_position, i_normal, i_uv, i_tangent, i_bitangent, u_normal,
  u_normal2, u_model, u_model2, type;
  float width, length, height;
  bool left;
};

#endif // THREEDIMENTIONALMODELLOADERNORMALS_H
