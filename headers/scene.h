#ifndef SCENE_H
#define SCENE_H

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QSurfaceFormat>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QVector3D>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMap>
#include <vector>

#include <QElapsedTimer>

#include "headers/laserclouds.h"
#include "headers/skybox.h"
#include "headers/vehicle.h"
#include "headers/ground.h"
#include "headers/streets.h"
#include "headers/movingobjects.h"
#include "headers/cylindricalobjects.h"
#include "headers/threedimentionalmodelloaderparallax.h"
#include "headers/threedimentionalmodelloadernormals.h"
#include "headers/threedimentionalmodelloadersimple.h"
#include "headers/trajectory.h"
#include "headers/coordinateconversor.h"

using namespace std;

class Scene : public QOpenGLWidget
{
  Q_OBJECT
public:
  explicit Scene(QWidget *parent = 0);
  ~Scene();

  int initial_scenaryRotationY, scenaryRotationY, *whichCompany;
  bool *onCloud, *onColorize, *onMovables, *onVehicle, *onStreets, *onSignalization,
  *onTrajectory, *onGround, *onSkybox, *onChassis, *onChassisRebounding, *onAxesRotation,
  *onAxesTexture, *onAxes, *onBox, *onFrameCoordinates, *isStaticCamera, *isCameraLocked;
  QMap<uint, bool> *isLaserActive, *isObstacleActive, *isCylinderActive, *isGroundActive, *isTrajectoryActive;
  Streets *streets;

  //see headers/console.h to see more information
  void ChangeCameraPosition(const float _x = 0, const float _y = 0, const float _z = 12);
  void ChangeCameraUpVector(const float _x = 0, const float _y = 1, const float _z = 0);
  void ChangeCameraLookingAt(const float _x = 0, const float _y = 0, const float _z = 0);
  void ChangeScenaryRotation(const float _rotationX = -25, const float _rotationY = 0, const float _rotationZ = 0);
  void ChangeScenaryZoom(const float _zoom = 1);
  void setTopView();
  void setStandardView();

  //-------------------------------------------- Laser handling --------------------------------------------------//
  unsigned int addLaser(const vector<LaserCloudData> *const _laserCloud, const QMatrix4x4 *const _transformationMatrix,
                        const float _pointSize = 1, const bool _gradient = true, const bool _colorized = true,
                        const bool _isIntensityHigherThanOne = false);
  unsigned int addLaser(const vector<LaserCloudData> *const _laserCloud, const QMatrix4x4 *const _transformationMatrix,
                        const float _pointSize = 1, const float _SolidColor[3] = {}, const bool _gradient = true,
                        const bool _colorized = true, const bool _isIntensityHigherThanOne = false);
  unsigned int addLaser(const vector<LaserCloudData> *const _laserCloud, const QMatrix4x4 *const _transformationMatrix,
                        const float _pointSize = 1, vector<array<float, 3> > _palette = {}, const bool _gradient = true,
                        const bool _colorized = true, const bool _isIntensityHigherThanOne = false);

  unsigned int addLaser(const vector<pointRGB> *const _laserCloud, const float _pointSize = 1, const bool _gradient = true,
                        const bool _colorized = true, const bool _isIntensityHigherThanOne = true);
  unsigned int addLaser(const vector<pointRGB> *const _laserCloud, const float _pointSize = 1, const float _SolidColor[3] = {},
                        const bool _gradient = true, const bool _colorized = true, const bool _isIntensityHigherThanOne = false);
  unsigned int addLaser(const vector<pointRGB> *const _laserCloud, const float _pointSize = 1, vector<array<float, 3> > _palette = {},
                        const bool _gradient = true, const bool _colorized = true, const bool _isIntensityHigherThanOne = false);
  unsigned int addLaser(const vector<pointRGB> *const _laserCloud, const QMatrix4x4 *const _transformationMatrix,
                        const float _pointSize = 1, const bool _gradient = true, const bool _colorized = true,
                        const bool _isIntensityHigherThanOne = false);
  unsigned int addLaser(const vector<pointRGB> *const _laserCloud, const QMatrix4x4 *const _transformationMatrix,
                        const float _pointSize = 1, const float _SolidColor[3] = {}, const bool _gradient = true,
                        const bool _colorized = true, const bool _isIntensityHigherThanOne = false);
  unsigned int addLaser(const vector<pointRGB> *const _laserCloud, const QMatrix4x4 *const _transformationMatrix,
                        const float _pointSize = 1, vector<array<float, 3> > _palette = {}, const bool _gradient = true,
                        const bool _colorized = true, const bool _isIntensityHigherThanOne = false);

  unsigned int addLaser(const vector<pointDAI> *const _laserCloud, const float _pointSize,
                        const float _angularResolutionDEGREES, const float _verticalAngleBetweenLayersDEGREES,
                        const QMatrix4x4 *const _transformationMatrix, const bool _gradient = true,
                        const bool _colorized = true, const bool _isIntensityHigherThanOne = false);
  unsigned int addLaser(const vector<pointDAI> *const _laserCloud, const float _pointSize,
                        const float _angularResolutionDEGREES, const float _verticalAngleBetweenLayersDEGREES,
                        const QMatrix4x4 *const _transformationMatrix, const float _SolidColor[3],
                        const bool _gradient = true, const bool _colorized = true,
                        const bool _isIntensityHigherThanOne = false);
  unsigned int addLaser(const vector<pointDAI> *const _laserCloud, const float _pointSize,
                        const float _angularResolutionDEGREES, const float _verticalAngleBetweenLayersDEGREES,
                        const QMatrix4x4 *const _transformationMatrix, vector<array<float, 3> > _palette,
                        const bool _gradient = true, const bool _colorized = true,
                        const bool _isIntensityHigherThanOne = false);

  bool setLaserColorPalette(unsigned int _id, vector<array<float, 3> > _palette, const bool _gradient = true);
  bool setLaserSolidColor(unsigned int _id, const float _SolidColor[3], const bool _gradient = true);
  bool ignoreSolidColor(unsigned int _id, const bool _ignore = true);
  bool setLaserOptions(unsigned int _id, const bool _gradient = true, const bool _colorized = true,
                       const bool _ignoreSolidColor = false, const bool _isIntensityHigherThanOne = false);
  bool setLaserTransformationMatrix(unsigned int _id, const QMatrix4x4 *const _transformationMatrix);
  bool translateLaserMatrix(unsigned int _id, const float _x = 0, const float _y = 0, const float _z = 0);
  bool rotateLaserMatrix(unsigned int _id, const float _pitch = 0, const float _yaw = 0, const float _roll = 0);
  bool updateCloud(unsigned int _id);
  bool deleteLaser(unsigned int _id);
  void purgeLaserClouds();

  //------------------------------------------ Obstacle handling -----------------------------------------------//
  unsigned int addObstacles(const vector<Obstacle> *const _obstacles, const QMatrix4x4 *const _transformationMatrix = nullptr);
  bool setObstacleTransformationMatrix(unsigned int _id, QMatrix4x4 *_transformationMatrix = nullptr);
  bool translateObstacleMatrix(unsigned int _id, const float _translationX = 0, const float _translationY = 0, const float _translationZ = 0);
  bool rotateObstacleMatrix(unsigned int _id, const float _pitch = 0, const float _yaw = 0, const float _roll = 0);
  bool deleteObstacles(unsigned int _id);
  void purgeObstacles();

  //------------------------------------------ Cylinder handling -----------------------------------------------//
  unsigned int addCylinders(const vector<Obstacle> *const _cylinders, const QMatrix4x4 *const _transformationMatrix = nullptr);
  bool setCylinderTransformationMatrix(unsigned int _id, QMatrix4x4 *_transformationMatrix = nullptr);
  bool translateCylinderMatrix(unsigned int _id, const float _translationX = 0, const float _translationY = 0, const float _translationZ = 0);
  bool rotateCylinderMatrix(unsigned int _id, const float _pitch = 0, const float _yaw = 0, const float _roll = 0);
  bool deleteCylinder(unsigned int _id);
  void purgeCylinders();

  //----------------------------------------- Trajectory handling ----------------------------------------------//
  unsigned int addTrajectory(const vector<vector<trajectoryData> > *const _trajectory);
  unsigned int addTrajectory(const vector<vector<trajectoryData> > *const _trajectory, const QMatrix4x4 *const _transformationMatrix);
  bool setTrajectoryTransformationMatrix(unsigned int _id, const QMatrix4x4 *const _transformationMatrix = nullptr);
  bool translateTrajectoryMatrix(unsigned int _id, const float _translationX = 0, const float _translationY = 0, const float _translationZ = 0);
  bool rotateTrajectoryMatrix(unsigned int _id, const float _pitch = 0, const float _yaw = 0, const float _roll = 0);
  bool updateTrajectory(unsigned int _id);
  bool updateAllTrajectories();
  bool deleteTrajectory(unsigned int _id);
  void purgeTrajectories();

  //-------------------------------------------- Maps handling -------------------------------------------------//
  bool setMapPosition(double *_latitude, double *_longitude);
  bool setMapRange(unsigned int _width = 150, unsigned int _length = 150);
  void setMapTransformationMatrix(QMatrix4x4 *_transformationMatrix = nullptr);
  void translateMap(const float _x = 0, const float _y = 0, const float _z = 0);
  void rotateMap(const float _pitch = 0, const float _yaw = 0, const float _roll = 0);
  bool forceUpdateMap();
  QString seeMapFolderPath();

  // ---------------------------------------- Ground handling ---------------------------------------- //
  unsigned int addGround(const vector<Boxes> *const _boxes,
                         const bool _drawLines = true, const QMatrix4x4 *const _transformationMatrix = nullptr);
  bool setGroundTransformationMatrix(unsigned int _id,
                                     const QMatrix4x4 *const _transformationMatrix = nullptr);
  bool setGroundWithLines(unsigned int _id, const bool _drawLines = true);
  bool translateGround(unsigned int  _id, const float _translationX = 0, const float _translationY = 0,
                       const float _translationZ = 0);
  bool rotateGround(unsigned int  _id, const float _pitch = 0, const float _yaw = 0, const float _roll = 0);
  bool updateGround(unsigned int _id);
  bool updateAllGround();
  bool deleteGround(unsigned int _id);
  void purgeGround();

  // ------------------------------------------- Vehicle ------------------------------------------- //
  bool addVehicle(const unsigned int _model = CHASSIS);
  void setLateralAxesLength(const float _length = 1.5);
  void setTireSize(const float _diameter = 0.54, const float _width = 0.18);
  void setVehicleDimensions(const float _A = 0.20, const float _B = 0.65, const float _C = 1.15,
                            const float _D = 1.15, const float _E = 0.20, const float _F = 1.50, const float _height = 1.7);
  void changeChassisPosition(const float _x = 1.475, const float _y = 0, const float _z = 0);
  void moveVehicleOriginsCoordinateSystem(const float _x = 0, const float _y = 0, const float _z = 0.05);

  unsigned int addFixedFrame(const float _x = 0, const float _y = 0, const float _z = 0,
                             const float _pitch = 0, const float _yaw = 0, const float _roll = 0);
  bool deleteFixedFrame(const unsigned int _id);
  void purgeFixedFrames();
  void nextFrame();

  // ----------------------------------------- zoom in/out ----------------------------------------- //
  void zooming(const bool _in = true);

  void setCoordinateConversor(CoordinateConversor *_conversor);

  const QMatrix4x4 *const masterMatrix();

  QMatrix4x4 *vehicleTransformationMatrix, *navigationTransformationMatrix, *plus90VehicleTransformationMatrix,
  *fixedFrameTransformationMatrix, *fixedFrameModel;
  double *vehiclePitch, *vehicleYaw, *vehicleRoll, *vehicleSpeed, *vehiclePositionX, *vehiclePositionY, *vehiclePositionZ,
  *vehicleSteeringAngle, *vehicleForwardAcceleration, *vehicleLeftwardAcceleration, *vehicleUpwardAcceleration,
  staticVehicleYaw, *framePitch, *frameYaw, *frameRoll, *vehicle_latitude, *vehicle_longitude;
  bool *isInstalling;

protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int w, int h);
  void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
  void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
  void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

private:
  void setData(vector<bool*> _options);
  void setRotation(int angle, const int axis);
  void setLookAt(const bool _rotate);
  void setTirePosition();
  void setAxesPosition();

  void initTextures(QString _image, bool _mirrored = true, bool _inverted = false);
  void modTexture(unsigned short _id, QString _image, bool _mirrored = true, bool _inverted = false);

  QOpenGLShaderProgram sx_program, vh_program, ls_program, gd_program,
  st_program, ob_program, m1_program, m2_program, m3_program, m4_program, tj_program;

  QVector3D lightPos, initial_camera_eyePos, camera_eyePos, initial_camera_centerPos, camera_centerPos,
  camera_translation, camera_lastPos, initial_camera_upPos, camera_upPos;
  QVector3D nonRotativeCamera_eyePos;
  QMatrix4x4 camera_rotationMatrix, emptyMatrix;
  QMatrix3x3 emptyMatrixNormalized;

  QPointF m_lastPos;
  int initial_scenaryRotationX, scenaryRotationX, initial_scenaryRotationZ, scenaryRotationZ;
  float initial_zoom, zoom;

  int i_position, i_normal, i_tangent, i_bitangent, i_texcoor,
  u_model, u_view, u_proj, u_normal, u_light, u_camera, u_parallax;

  QMatrix4x4 model, proj, view, view2;

  QVector<LaserClouds*> lasers;
  int ls_matrix, ls_masterColor;
  QMatrix4x4 master_matrix;

  Skybox *skybox;
  unsigned int sx_pvm;
  QMatrix4x4 m_pvm, m_scale, model2;

  Vehicle *vehicle;
  int vh_proj, vh_view, vh_light, vh_camera, vh_model, vh_normal;
  float vh_posX, vh_posY, vh_posZ, posRearAxis, posFrontalAxis, lateralAxisLength, longitudinalAxisLength,
  tireCircunference, vh_width;
  QVector3D previousVehiclePosition;
  QMatrix3x3 vehicleNormalMatrix;

  QVector<Ground*> ground;
  unsigned int gd_proj, gd_view, gd_camera, gd_light;

  unsigned int st_lightPos, st_viewPos, st_matrix;

  QVector<MovingObjects*> obstacles;
  QVector<CylindricalObjects*> cylinders;
  int ob_lightPos, ob_viewPos, ob_matrix;

  QMap<unsigned short, ThreeDimentionalModelLoaderParallax*> vehicleParts1;
  GLuint m1_id_diffuse, m1_id_displacement, m1_id_normal, m1_id_specular;
  int m1_lightPos, m1_viewPos, m1_matrix, m1_vehicle, m1_normal_vehicle, m1_static;

  QVector<ThreeDimentionalModelLoaderNormals*> frameCoordinates;
  QMap<unsigned short, ThreeDimentionalModelLoaderNormals*> vehicleParts2;
  GLuint m2_id_diffuse, m2_id_normal, m2_id_specular;
  int m2_lightPos, m2_viewPos, m2_matrix, m2_vehicle, m2_normal_vehicle, m2_static;

  QMap<unsigned short, ThreeDimentionalModelLoaderSimple*> vehicleParts3;
  QMap<unsigned short, ThreeDimentionalModelLoaderSimple*> buildings;
  GLuint m3_id_diffuse, m3_id_specular, m4_id_diffuse, m4_id_specular;
  int m3_lightPos, m3_viewPos, m3_matrix, m3_vehicle, m3_normal_vehicle, m3_static;
  int m4_lightPos, m4_viewPos, m4_matrix, m4_vehicle, m4_normal_vehicle, m4_static;
  bool db5active, shuttleActive, shuttle_chassis;

  float rot, acceleration_effect;
  bool isVehicleCreated;
  double steeringAngle;
  QVector3D chassisPos, chassisSize, steeringPos;

  QVector<Trajectory*> trajectories;
  int tj_lightPos, tj_viewPos, tj_matrix;
  GLuint tj_tex1, tj_tex2;
  QVector<QOpenGLTexture*> all_textures;

  vector<bool*> options;
  bool dataset, updateRotation;

  QMatrix4x4 previousFixedFrameModel, street_scooter_building, campus_boulevar_building;
  QMatrix3x3 street_scooter_building_normal, campus_boulevar_building_normal;

  CoordinateConversor *the_conversor;

private slots:
  //change the company button logotype and vehicle
  void changeCompany();
  void reloadMap();
  void reloadMapModel();

signals:
  void resizing();
  void rotating();
  void mouseMoving();
};

#endif // SCENE_H
