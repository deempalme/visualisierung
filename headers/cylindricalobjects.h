#ifndef CYLINDRICALOBJECTS_H
#define CYLINDRICALOBJECTS_H

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QVector>
#include <vector>

#include "headers/definitions.h"

using std::vector;

struct CylinderData{
  QVector3D position;
  QVector3D color;
  QVector3D normal;
};

class CylindricalObjects
{
public:
  CylindricalObjects(QOpenGLShaderProgram *_program, const vector<Obstacle> *const _cylinders);
  ~CylindricalObjects();
  void changeInput(const vector<Obstacle> *const _newInput);
  void setTransformationMatrix(const QMatrix4x4 *const _transformationMatrix);
  void translateCylindersMatrix(const float _translationX = 0, const float _translationY = 0,
                                const float _translationZ = 0);
  void rotateCylindersMatrix(const float _pitch = 0, const float _yaw = 0, const float _roll = 0);
  void updateCylinders();
  void drawCylinders();

private:
  QOpenGLShaderProgram *m_program;
  unsigned int i_position, i_color, i_normal, u_model, u_vehicle, sizeCylinders;
  QOpenGLBuffer cylindersBuffer, indexCylindersBuffer;

  const vector<Obstacle> *cylinders;
  CylinderData cylr[272];
  const QMatrix4x4 *transformationMatrixPtr;
  QMatrix4x4 transformationMatrix, emptyMatrix;
};

#endif // CYLINDRICALOBJECTS_H
