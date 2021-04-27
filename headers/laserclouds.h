#ifndef LASERCLOUDS_H
#define LASERCLOUDS_H

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QMatrix4x4>

#include "headers/definitions.h"

class LaserClouds
{
public:
  LaserClouds(QOpenGLShaderProgram *_program, const vector<LaserCloudData> *const _inputCloud, const float _pointSize,
              const bool _gradient = true, const bool _colorized = true, const bool _isIntensityHigherThanOne = false);
  LaserClouds(QOpenGLShaderProgram *_program, const vector<pointRGB> *const _inputCloud, const float _pointSize,
              const bool _gradient = true, const bool _colorized = true, const bool _isIntensityHigherThanOne = false);
  LaserClouds(QOpenGLShaderProgram *_program, const vector<pointDAI> *const _inputCloud, const float _pointSize,
              const float _angularResolutionDEGREES, const float _verticalAngleBetweenLayersDEGREES,
              const bool _gradient = true, const bool _colorized = true, const bool _isIntensityHigherThanOne = false);
  ~LaserClouds();

  //see headers/console.h to see more information
  void ChangeInput(const vector<LaserCloudData> *const _newInputCloud);
  void ChangeInput(const vector<pointRGB> *const _newInputCloud);
  void setTransformationMatrix(const QMatrix4x4 *const _transformationMatrix);
  void setOptions(const bool _gradient = true, const bool _colorized = true, const bool _ignoreSolidColor = false, const bool _isIntensityHigherThanOne = false);
  void setSolidColor(const float _SolidColor[3], const bool _gradient = true);
  bool ignoreSolidColor(const bool _ignore = true);
  void setLaserColorPalette(vector<array<float, 3> > _palette, const bool _gradient = true);
  void translateLaserMatrix(const float _x, const float _y, const float _z);
  void rotateLaserMatrix(const float _pitch, const float _yaw, const float _roll);
  void drawCloud();
  void updateCloud();

private:
  void setInitValues();

  QOpenGLShaderProgram *m_program;
  QOpenGLBuffer cloud;

  unsigned int i_position, i_intensity, i_color, i_distance, i_angle, i_layer, u_vehicle, u_model,
  u_256range, u_solid, u_rgb, u_ignore, u_colorize, u_palette, u_numColors, u_angularResolution,
  u_verticalAngle, u_isPointDAI;
  unsigned int sizePoints;

  const vector<LaserCloudData> *laserCloud;
  const vector<pointRGB> *laserCloudPCL;
  const vector<pointDAI> *laserCloudIBEO;
  QVector3D palette[10];
  const QMatrix4x4 *modelVehicle;
  QMatrix4x4 modelCloud, emptyMatrix;
  bool colorized, isIntensityInterger, solid, isRGB, ignoreSolid;
  float num_colors;
  float angularResolution, verticalAngle;
  bool isPointDAI;

  float pointSize;
};

#endif // LASERCLOUDS_H
