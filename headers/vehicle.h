#ifndef VEHICLE_H
#define VEHICLE_H

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QMatrix4x4>

struct vehicleData{
  QVector3D position;
  QVector2D texcoor;
  QVector3D normal;
};

class Vehicle
{
public:
  Vehicle(QOpenGLShaderProgram *_program, int *_whichCompany);
  ~Vehicle();

  void changeSize(const float _width = 1.7f, const float _height = 2.0f, const float _length = 4.1f);
  void translateVehicle(const float _x = 1.475, const float _y = 0, const float _z = 0);
  void applyAcceleration(const float _acceleration = 0);
  void drawVehicle();

private:
  void initTextures(const QString _image);
  void updateData();

  QOpenGLShaderProgram *m_program;
  QOpenGLBuffer vehicleBuffer, vehicleIndexBuffer;

  unsigned int i_position, i_texcoor, i_normal, u_type, u_model;
  float carWidth, carHeight, carLenght, carCenter;

  QOpenGLTexture *texture;
  GLuint texture1;

  QMatrix4x4 modelMatrix;
  QVector3D translation;

  int *whichCompany;
};

#endif // VEHICLE_H
