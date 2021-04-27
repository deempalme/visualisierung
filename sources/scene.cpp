#include "headers/scene.h"

Scene::Scene(QWidget *parent) :
  QOpenGLWidget(parent),
  initial_scenaryRotationY(0),
  scenaryRotationY(initial_scenaryRotationY),
  vehicleTransformationMatrix(nullptr),
  navigationTransformationMatrix(nullptr),
  plus90VehicleTransformationMatrix(nullptr),
  fixedFrameTransformationMatrix(nullptr),
  fixedFrameModel(nullptr),
  vehiclePitch(nullptr),
  vehicleYaw(nullptr),
  vehicleRoll(nullptr),
  vehicleSpeed(nullptr),
  vehiclePositionX(nullptr),
  vehiclePositionY(nullptr),
  vehiclePositionZ(nullptr),
  vehicleSteeringAngle(nullptr),
  vehicleForwardAcceleration(nullptr),
  vehicleLeftwardAcceleration(nullptr),
  vehicleUpwardAcceleration(nullptr),
  staticVehicleYaw(0),
  framePitch(nullptr),
  frameYaw(nullptr),
  frameRoll(nullptr),
  vehicle_latitude(nullptr),
  vehicle_longitude(nullptr),
  isInstalling(nullptr),
  lightPos(QVector3D(30,30,-15)),
  initial_camera_eyePos(QVector3D(0, 0, 12)),
  camera_eyePos(initial_camera_eyePos),
  initial_camera_centerPos(),
  camera_centerPos(),
  camera_translation(),
  camera_lastPos(),
  initial_camera_upPos(QVector3D(0, 1, 0)),
  camera_upPos(initial_camera_upPos),
  nonRotativeCamera_eyePos(initial_camera_eyePos),
  initial_scenaryRotationX(-400),
  scenaryRotationX(initial_scenaryRotationX),
  initial_scenaryRotationZ(0),
  scenaryRotationZ(initial_scenaryRotationZ),
  initial_zoom(1),
  zoom(initial_zoom),
  lasers(0),
  vh_posX(0),
  vh_posY(0),
  vh_posZ(0),
  posRearAxis(1.475),
  posFrontalAxis(-1.475),
  lateralAxisLength(1.5),
  longitudinalAxisLength(2.95),
  vh_width(1.50),
  vehicleParts1(),
  frameCoordinates(0),
  vehicleParts2(),
  db5active(false),
  shuttleActive(false),
  shuttle_chassis(false),
  rot(0),
  acceleration_effect(0),
  isVehicleCreated(false),
  steeringAngle(0),
  updateRotation(true),
  the_conversor(nullptr)
{
  //It is necessary to configure OpenGL to run at the best performance
  //QSurfaceFormat will do the job
  QSurfaceFormat glFormat;
  //this is for multisampling:
  //every draw operation will be run 4 times (consider this for performance),
  //if you want to increment this number select a number base 2: 2^n example: (1, 2, 4, 8, 16, 32, ...)
  glFormat.setSamples(4);
  //this is for the depth buffer size, in this moment it is 24 bits, choose among: 16, 24 and 32 bits (32bits is rarely suported)
  glFormat.setDepthBufferSize(24);
  //defining OpenGL as glFormat
  glFormat.setProfile(QSurfaceFormat::NoProfile);
  //this stablishes the openGL version:
  //the selected version is OpenGL:3.0 with GLSL:1.30
  glFormat.setRenderableType(QSurfaceFormat::OpenGL);
  //glFormat.setMajorVersion(3);
  //glFormat.setMinorVersion(0);
  //applying the glFormat
  setFormat(glFormat);
  makeCurrent();

  emptyMatrixNormalized = emptyMatrix.normalMatrix();
}

Scene::~Scene(){
  for(int i = 0; i < lasers.size(); i++)
    if(lasers[i] != nullptr)
      delete lasers[i];

  for(int i = 0; i < obstacles.size(); i++)
    if(obstacles[i] != nullptr)
      delete obstacles[i];

  delete skybox;
  delete streets;
  delete vehicle;

  for(int i = 0; i < vehicleParts1.size(); i++)
    if(vehicleParts1[i] != nullptr)
      delete vehicleParts1[i];

  for(int i = 0; i < vehicleParts2.size(); i++)
    if(vehicleParts2[i] != nullptr)
      delete vehicleParts2[i];

  for(int i = 0; i < vehicleParts3.size(); i++)
    if(vehicleParts3[i] != nullptr)
      delete vehicleParts3[i];

  for(int i = 0; i < buildings.size(); i++)
    if(buildings[i] != nullptr)
      delete buildings[i];

  for(int i = 0; i < frameCoordinates.size(); i++)
    if(frameCoordinates[i] != nullptr)
      delete frameCoordinates[i];

  for(int i = 0; i < ground.size(); i++)
    if(ground[i] != nullptr)
      delete ground[i];

  for(int i = 0; i < trajectories.size(); i++)
    if(trajectories[i] != nullptr)
      delete trajectories[i];

  for(int i = 0; i < all_textures.size(); i++)
    delete all_textures[i];
}

void Scene::initializeGL(){
  //qWarning() << (char*)glGetString(GL_VERSION);
  //qWarning() << (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

  glClearColor(0, 0, 0, 1); //background color (black)
  //this line allows anti-aliasing to create fine edges
  glEnable(GL_MULTISAMPLE);
  //this line allows z-buffer to avoid objects behind others appear in front
  glEnable(GL_DEPTH_TEST);
  //this allows textures
  glEnable(GL_TEXTURE_2D);
  //these allow alpha transparency in the rendering
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //------------------------------------------------//
  //                 shader section                 //
  //------------------------------------------------//
  QOpenGLShader sh_skyboxV(QOpenGLShader::Vertex);
  sh_skyboxV.compileSourceFile(":/shaders/skyboxVert");

  QOpenGLShader sh_skyboxF(QOpenGLShader::Fragment);
  sh_skyboxF.compileSourceFile(":/shaders/skyboxFrag");

  QOpenGLShader sh_laserV(QOpenGLShader::Vertex);
  sh_laserV.compileSourceFile(":/shaders/laserVert");

  QOpenGLShader sh_laserF(QOpenGLShader::Fragment);
  sh_laserF.compileSourceFile(":/shaders/laserFrag");

  QOpenGLShader sh_vehicleV(QOpenGLShader::Vertex);
  sh_vehicleV.compileSourceFile(":/shaders/vehicleVert");

  QOpenGLShader sh_vehicleF(QOpenGLShader::Fragment);
  sh_vehicleF.compileSourceFile(":/shaders/vehicleFrag");

  QOpenGLShader sh_groundV(QOpenGLShader::Vertex);
  sh_groundV.compileSourceFile(":/shaders/groundVert");

  QOpenGLShader sh_groundF(QOpenGLShader::Fragment);
  sh_groundF.compileSourceFile(":/shaders/groundFrag");

  QOpenGLShader sh_streetsV(QOpenGLShader::Vertex);
  sh_streetsV.compileSourceFile(":/shaders/streetVert");

  QOpenGLShader sh_streetsF(QOpenGLShader::Fragment);
  sh_streetsF.compileSourceFile(":/shaders/streetFrag");

  QOpenGLShader sh_objectsV(QOpenGLShader::Vertex);
  sh_objectsV.compileSourceFile(":/shaders/objectsVert");

  QOpenGLShader sh_objectsF(QOpenGLShader::Fragment);
  sh_objectsF.compileSourceFile(":/shaders/objectsFrag");

  QOpenGLShader sh_complexV(QOpenGLShader::Vertex);
  sh_complexV.compileSourceFile(":/shaders/complexVert");

  QOpenGLShader sh_complexF(QOpenGLShader::Fragment);
  sh_complexF.compileSourceFile(":/shaders/complexFrag");

  QOpenGLShader sh_trajectoryV(QOpenGLShader::Vertex);
  sh_trajectoryV.compileSourceFile(":/shaders/trajectoryVert");

  QOpenGLShader sh_trajectoryF(QOpenGLShader::Fragment);
  sh_trajectoryF.compileSourceFile(":/shaders/trajectoryFrag");

  QOpenGLShader sh_normalsV(QOpenGLShader::Vertex);
  sh_normalsV.compileSourceFile(":/shaders/normalsVert");

  QOpenGLShader sh_normalsF(QOpenGLShader::Fragment);
  sh_normalsF.compileSourceFile(":/shaders/normalsFrag");

  QOpenGLShader sh_blinnphongV(QOpenGLShader::Vertex);
  sh_blinnphongV.compileSourceFile(":/shaders/blinnPhongVert");

  QOpenGLShader sh_blinnphongF(QOpenGLShader::Fragment);
  sh_blinnphongF.compileSourceFile(":/shaders/blinnPhongFrag");

  float scrY = scenaryRotationX/anglePrecision;
  camera_rotationMatrix.rotate(QQuaternion::fromEulerAngles(scrY, staticVehicleYaw + scenaryRotationY/anglePrecision, scenaryRotationZ/anglePrecision));
  if((scrY < 270) && (scrY > 90))
    camera_upPos = QVector3D(0, -1, 0);
  else
    camera_upPos = QVector3D(0, 1, 0);

  camera_eyePos = camera_rotationMatrix * camera_eyePos;
  view.lookAt(camera_eyePos, camera_centerPos, camera_upPos);
  proj.perspective(60.0, 1350/600.0f, 0.1, 3000.0);

  //------------------------------------------------//
  //                    laserCloud                  //
  //------------------------------------------------//

  if(!ls_program.addShader(&sh_laserV))
    qWarning("It was not possible to load the GLSL vertex code from the laser");
  if(!ls_program.addShader(&sh_laserF))
    qWarning("It was not possible to load the GLSL fragment code from the laser");
  if(!ls_program.link())
    qWarning("The GLSL laser program wasn't compiled properly");
  if(!ls_program.bind())
    qWarning("The GLSL laser program wasn't linked properly");

  ls_matrix = ls_program.uniformLocation("u_matrix");
  ls_masterColor = ls_program.uniformLocation("u_masterColor");
  master_matrix = proj * view;

  ls_program.release();

  //------------------------------------------------//
  //                      skybox                    //
  //------------------------------------------------//
  if(!sx_program.addShader(&sh_skyboxV))
    qWarning("It was not possible to load the GLSL vertex code from the skybox");
  if(!sx_program.addShader(&sh_skyboxF))
    qWarning("It was not possible to load the GLSL fragment code from the skybox");
  if(!sx_program.link())
    qWarning("The GLSL skybox program wasn't compiled properly");
  if(!sx_program.bind())
    qWarning("The GLSL skybox program wasn't linked properly");

  m_scale.scale(300);
  view2 = view;
  m_pvm = proj * view2 * m_scale;
  sx_pvm = sx_program.uniformLocation("u_pvm");
  sx_program.setUniformValue(sx_pvm, m_pvm);
  skybox = new Skybox(&sx_program);

  sx_program.release();

  //------------------------------------------------//
  //                      ground                    //
  //------------------------------------------------//
  if(!gd_program.addShader(&sh_groundV))
    qWarning("It was not possible to load the GLSL vertex code from the Ground");
  if(!gd_program.addShader(&sh_groundF))
    qWarning("It was not possible to load the GLSL fragment code from the Ground");
  if(!gd_program.link())
    qWarning("The ground GLSL program wasn't compiled properly");
  if(!gd_program.bind())
    qWarning("The ground GLSL program wasn't linked properly");

  gd_proj = gd_program.uniformLocation("u_proj");
  gd_view = gd_program.uniformLocation("u_view");
  gd_camera = gd_program.uniformLocation("u_cameraPos");
  gd_light = gd_program.uniformLocation("u_lightPos");
  gd_program.setUniformValue(gd_proj, proj);
  gd_program.setUniformValue(gd_view, view);

  gd_program.release();

  //------------------------------------------------//
  //                     streets                    //
  //------------------------------------------------//
  if(!st_program.addShader(&sh_streetsV))
    qWarning("It was not possible to load the GLSL vertex code from the Streets");
  if(!st_program.addShader(&sh_streetsF))
    qWarning("It was not possible to load the GLSL fragment code from the Streets");
  if(!st_program.link())
    qWarning("The streets GLSL program wasn't compiled properly");
  if(!st_program.bind())
    qWarning("The streets GLSL program wasn't linked properly");

  streets = new Streets(&st_program, &scenaryRotationY);
  st_lightPos = st_program.uniformLocation("lightPos");
  st_viewPos = st_program.uniformLocation("viewPos");
  st_matrix = st_program.uniformLocation("u_matrix");
  st_program.setUniformValue(st_matrix, master_matrix);
  st_program.setUniformValue(st_lightPos, lightPos);
  st_program.setUniformValue(st_viewPos, camera_eyePos);

  st_program.release();

  connect(streets->mapping, SIGNAL(MapLoaded()), this, SLOT(reloadMap()));
  connect(streets, SIGNAL(reloadMapMatrix()), this, SLOT(reloadMapModel()));
  streets->isInstalling = isInstalling;

  //------------------------------------------------//
  //                     objects                    //
  //------------------------------------------------//
  if(!ob_program.addShader(&sh_objectsV))
    qWarning("It was not possible to load the GLSL vertex code from the Obstacles");
  if(!ob_program.addShader(&sh_objectsF))
    qWarning("It was not possible to load the GLSL fragment code from the Obstacles");
  if(!ob_program.link())
    qWarning("The objects GLSL Obstacles program wasn't compiled properly");
  if(!ob_program.bind())
    qWarning("The objects GLSL Obstacles program wasn't linked properly");

  ob_lightPos = ob_program.uniformLocation("lightPos");
  ob_viewPos = ob_program.uniformLocation("viewPos");
  ob_matrix = ob_program.uniformLocation("u_matrix");
  ob_program.setUniformValue(ob_matrix, master_matrix);
  ob_program.setUniformValue(ob_lightPos, lightPos);
  ob_program.setUniformValue(ob_viewPos, camera_eyePos);

  ob_program.release();

  //------------------------------------------------//
  //                     vehicle                    //
  //------------------------------------------------//
  if(!vh_program.addShader(&sh_vehicleV))
    qWarning("It was not possible to load the GLSL vertex code from the vehicle");
  if(!vh_program.addShader(&sh_vehicleF))
    qWarning("It was not possible to load the GLSL fragment code from the vehicle");
  if(!vh_program.link())
    qWarning("The vehicle GLSL program wasn't compiled properly");
  if(!vh_program.bind())
    qWarning("The vehicle GLSL program wasn't linked properly");

  vh_proj = vh_program.uniformLocation("u_proj");
  vh_view = vh_program.uniformLocation("u_view");
  vh_light = vh_program.uniformLocation("u_lightPos");
  vh_camera = vh_program.uniformLocation("u_cameraPos");
  vh_model = vh_program.uniformLocation("u_model");
  vh_normal = vh_program.uniformLocation("u_normal");
  vehicle = new Vehicle(&vh_program, whichCompany);

  vh_program.release();

  //------------------------------------------------//
  //                    Trajectory                  //
  //------------------------------------------------//
  if(!tj_program.addShader(&sh_trajectoryV))
    qWarning("It was not possible to load the GLSL vertex code from the trajectories");
  if(!tj_program.addShader(&sh_trajectoryF))
    qWarning("It was not possible to load the GLSL fragment code from the trajectories");
  if(!tj_program.link())
    qWarning("The trajectories GLSL program wasn't compiled properly");
  if(!tj_program.bind())
    qWarning("The trajectories GLSL program wasn't linked properly");

  tj_matrix = tj_program.uniformLocation("u_matrix");
  tj_lightPos = tj_program.uniformLocation("u_light");
  tj_viewPos = tj_program.uniformLocation("u_camera");

  //0:
  initTextures(QString(":/images/t_trajectA"));
  //1:
  initTextures(QString(":/images/t_trajectB"));

  glGenTextures(1, &tj_tex1);
  glGenTextures(1, &tj_tex2);
  tj_program.setUniformValue("u_texture1", tj_tex1);
  tj_program.setUniformValue("u_texture2", tj_tex2);
  tj_program.setUniformValue(tj_matrix, master_matrix);
  tj_program.setUniformValue(tj_lightPos, lightPos);
  tj_program.setUniformValue(tj_viewPos, camera_eyePos);

  tj_program.release();

  //------------------------------------------------//
  //                 model loading                  //
  //------------------------------------------------//
  if(!m1_program.addShader(&sh_complexV))
    qWarning("It was not possible to load the GLSL vertex code from the Model Loader");
  if(!m1_program.addShader(&sh_complexF))
    qWarning("It was not possible to load the GLSL fragment code from the Model Loader");
  if(!m1_program.link())
    qWarning("The model loading GLSL program wasn't compiled properly");
  if(!m1_program.bind())
    qWarning("The model loading GLSL program wasn't linked properly");

  glGenTextures(1, &m1_id_diffuse);
  glGenTextures(1, &m1_id_normal);
  glGenTextures(1, &m1_id_displacement);
  glGenTextures(1, &m1_id_specular);
  m1_program.setUniformValue("u_diffuse", m1_id_diffuse);
  m1_program.setUniformValue("u_normal", m1_id_normal);
  m1_program.setUniformValue("u_displacement", m1_id_displacement);
  m1_program.setUniformValue("u_specular", m1_id_specular);

  m1_lightPos = m1_program.uniformLocation("u_light_position");
  m1_viewPos = m1_program.uniformLocation("u_camera_position");
  m1_matrix = m1_program.uniformLocation("u_matrix");
  m1_vehicle = m1_program.uniformLocation("u_vehicle");
  m1_normal_vehicle = m1_program.uniformLocation("u_vehicle_normal");
  m1_static = m1_program.uniformLocation("u_static");
  m1_program.setUniformValue(m1_matrix, master_matrix);
  m1_program.setUniformValue(m1_lightPos, lightPos);
  m1_program.setUniformValue(m1_viewPos, camera_eyePos);

  m1_program.release();

  //------------------------------------------------//
  //                 model loading 2                //
  //------------------------------------------------//
  if(!m2_program.addShader(&sh_normalsV))
    qWarning("It was not possible to load the GLSL vertex code from the Model Loader 2");
  if(!m2_program.addShader(&sh_normalsF))
    qWarning("It was not possible to load the GLSL fragment code from the Model Loader 2");
  if(!m2_program.link())
    qWarning("The model loading GLSL program wasn't compiled properly");
  if(!m2_program.bind())
    qWarning("The model loading GLSL program wasn't linked properly");

  glGenTextures(1, &m2_id_diffuse);
  glGenTextures(1, &m2_id_normal);
  glGenTextures(1, &m2_id_specular);
  m2_program.setUniformValue("u_diffuse", m2_id_diffuse);
  m2_program.setUniformValue("u_normal", m2_id_normal);
  m2_program.setUniformValue("u_specular", m2_id_specular);

  #ifdef ROS_DATA
    //2:
    initTextures(E_MODELS3D + "textures/coordinates_diffuse.png");
    //3:
    initTextures(E_MODELS3D + "textures/coordinates_normal.png");
    //4:
    initTextures(E_MODELS3D + "textures/coordinates_specular.png");
  #else
    //2:
    initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/coordinates_diffuse.png");
    //3:
    initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/coordinates_normal.png");
    //4:
    initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/coordinates_specular.png");
  #endif

  m2_lightPos = m2_program.uniformLocation("u_light_position");
  m2_viewPos = m2_program.uniformLocation("u_camera_position");
  m2_matrix = m2_program.uniformLocation("u_matrix");
  m2_vehicle = m2_program.uniformLocation("u_vehicle");
  m2_normal_vehicle = m2_program.uniformLocation("u_vehicle_normal");
  m2_static = m2_program.uniformLocation("u_static");

  m2_program.setUniformValue(m2_matrix, master_matrix);
  m2_program.setUniformValue(m2_lightPos, lightPos);
  m2_program.setUniformValue(m2_viewPos, camera_eyePos);

  m2_program.release();

  //------------------------------------------------//
  //                 model loading 3                //
  //------------------------------------------------//
  if(!m3_program.addShader(&sh_blinnphongV))
    qWarning("It was not possible to load the GLSL vertex code from the Model Loader 3");
  if(!m3_program.addShader(&sh_blinnphongF))
    qWarning("It was not possible to load the GLSL fragment code from the Model Loader 3");
  if(!m3_program.link())
    qWarning("The model loading GLSL program wasn't compiled properly");
  if(!m3_program.bind())
    qWarning("The model loading GLSL program wasn't linked properly");

  glGenTextures(1, &m3_id_diffuse);
  glGenTextures(1, &m3_id_specular);
  m3_program.setUniformValue("u_diffuse", m3_id_diffuse);
  m3_program.setUniformValue("u_specular", m3_id_specular);

  m3_lightPos = m3_program.uniformLocation("u_light_position");
  m3_viewPos = m3_program.uniformLocation("u_camera_position");
  m3_matrix = m3_program.uniformLocation("u_matrix");
  m3_vehicle = m3_program.uniformLocation("u_vehicle");
  m3_normal_vehicle = m3_program.uniformLocation("u_vehicle_normal");

  m3_program.setUniformValue(m3_matrix, master_matrix);
  m3_program.setUniformValue(m3_lightPos, lightPos);
  m3_program.setUniformValue(m3_viewPos, camera_eyePos);

  m3_program.release();

  //------------------------------------------------//
  //                 model loading 4                //
  //------------------------------------------------//
  if(!m4_program.addShader(&sh_blinnphongV))
    qWarning("It was not possible to load the GLSL vertex code from the Model Loader 3");
  if(!m4_program.addShader(&sh_blinnphongF))
    qWarning("It was not possible to load the GLSL fragment code from the Model Loader 3");
  if(!m4_program.link())
    qWarning("The model loading GLSL program wasn't compiled properly");
  if(!m4_program.bind())
    qWarning("The model loading GLSL program wasn't linked properly");

  glGenTextures(1, &m4_id_diffuse);
  glGenTextures(1, &m4_id_specular);
  m4_program.setUniformValue("u_diffuse", m4_id_diffuse);
  m4_program.setUniformValue("u_specular", m4_id_specular);

  m4_lightPos = m4_program.uniformLocation("u_light_position");
  m4_viewPos = m4_program.uniformLocation("u_camera_position");
  m4_matrix = m4_program.uniformLocation("u_matrix");
  m4_vehicle = m4_program.uniformLocation("u_vehicle");
  m4_normal_vehicle = m4_program.uniformLocation("u_vehicle_normal");

  m4_program.setUniformValue(m4_matrix, master_matrix);
  m4_program.setUniformValue(m4_lightPos, lightPos);
  m4_program.setUniformValue(m4_viewPos, camera_eyePos);

  m4_program.release();
}

void Scene::paintGL(){
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if(*onSkybox){
    if(!sx_program.bind()) return;
    skybox->drawSkybox();
    sx_program.release();
  }

  if(*onStreets){
    if(!st_program.bind()) return;
    streets->drawStreets(true);
    st_program.release();
  }

  if(*onCloud){
    if(!ls_program.bind()) return;
    ls_program.setUniformValue(ls_masterColor, *onColorize);
    for(int i = 0; i < lasers.size(); i++)
      if(lasers[i] != nullptr && isLaserActive->value(i))
        lasers[i]->drawCloud();
    ls_program.release();
  }

  if(*onSignalization){
    if(!st_program.bind()) return;
    streets->drawStreets(false);
    st_program.release();
  }

  if(*onGround){
    if(!gd_program.bind()) return;
    for(int i = 0; i < ground.size(); i++)
    if(ground[i] != nullptr && isGroundActive->value(i))
      ground[i]->drawGround();
    gd_program.release();
  }

  if(*onMovables){
    if(!ob_program.bind()) return;
    for(int i = 0; i < obstacles.size(); i++)
      if(obstacles[i] != nullptr && isObstacleActive->value(i))
        obstacles[i]->drawObstacle();
    for(int i = 0; i < cylinders.size(); i++)
      if(cylinders[i] != nullptr && isCylinderActive->value(i))
        cylinders[i]->drawCylinders();
    ob_program.release();
  }

  if(*onVehicle){
    if((*onChassis || *onAxes) && (isVehicleCreated)){
      bool ok = (vehiclePositionX != nullptr && vehiclePositionY != nullptr && vehiclePositionZ != nullptr)? true : false;
      if(ok){
        if(updateRotation){
          QVector3D temporal(*vehiclePositionX, *vehiclePositionY, *vehiclePositionZ);
          if(temporal.x() == 0 && temporal.y() == 0 && temporal.z() == 0)
            previousVehiclePosition = QVector3D();
          else
            previousVehiclePosition = temporal - previousVehiclePosition;

          rot += previousVehiclePosition.length() / tireCircunference;
//          qWarning() << rot;
          previousVehiclePosition = temporal;
          rot = (rot > 360)? rot - 360 : rot;
          rot = (rot < 0)? rot + 360 : rot;
          updateRotation = false;
        }
      }else{
        rot = 0;
      }
      if(vehicleSteeringAngle != nullptr)
        steeringAngle += (*vehicleSteeringAngle - steeringAngle)/3;
//      steeringAngle += (*vehicleSteeringAngle * toDEGREES - steeringAngle)/3;

      if(*onAxesRotation && *onAxes && ok && rot != 0){
        vehicleParts2[FRONTAL_AXIS]->changeRotation(0, 90, -rot);
        vehicleParts2[CENTRAL_AXIS]->changeRotation(0, 0, -rot);
        vehicleParts2[REAR_AXIS]->changeRotation(0, 90, -rot);
      }
      if(*onChassisRebounding && *onChassis && (vehicleUpwardAcceleration != nullptr)){
        #ifdef ROS_DATA
          acceleration_effect += ((0.06 * (*vehicleUpwardAcceleration)) - acceleration_effect)/3.0f; //- 0.4905;
        #else
          acceleration_effect += ((0.06 * (9.81 - *vehicleUpwardAcceleration)) - acceleration_effect)/3.0f; //- 0.4905;
        #endif

        vehicleParts2[COORDINATES]->applyAcceleration(acceleration_effect);
        vehicleParts2[CENTRAL_AXIS]->applyAcceleration(acceleration_effect);
        vehicleParts2[STEERING]->applyAcceleration(acceleration_effect);
        vehicle->applyAcceleration(acceleration_effect);
      }
      if(ok && rot != 0){
        if(vehicleSteeringAngle != nullptr){
          vehicleParts1[FRONTAL_LEFT_TIRE]->changeRotation(0, 90 - steeringAngle/3.0f, -rot);
          vehicleParts1[FRONTAL_RIGHT_TIRE]->changeRotation(0, -90 - steeringAngle/3.0f, rot);
        }
        vehicleParts1[REAR_LEFT_TIRE]->changeRotation(0, 90, -rot);
        vehicleParts1[REAR_RIGHT_TIRE]->changeRotation(0, -90, rot);
      }
      m1_program.bind();
      m1_program.setUniformValue(m1_static, false);
      all_textures[5]->bind(m1_id_diffuse);
      all_textures[6]->bind(m1_id_displacement);
      all_textures[7]->bind(m1_id_specular);
      all_textures[8]->bind(m1_id_normal);

      vehicleParts1[FRONTAL_LEFT_TIRE]->drawModel();
      vehicleParts1[REAR_LEFT_TIRE]->drawModel();
      all_textures[9]->bind(m1_id_displacement);

      vehicleParts1[FRONTAL_RIGHT_TIRE]->drawModel();
      vehicleParts1[REAR_RIGHT_TIRE]->drawModel();

      m1_program.release();

      m2_program.bind();
      m2_program.setUniformValue(m2_static, false);
      all_textures[2]->bind(m2_id_diffuse);
      all_textures[3]->bind(m2_id_normal);
      all_textures[4]->bind(m2_id_specular);
      vehicleParts2[COORDINATES]->drawModel();

      if(*onAxes){
        if(vehicleSteeringAngle != nullptr)
          if(db5active && *onChassis){
            vehicleParts2[STEERING]->changeRotation(70, -steeringAngle, 0);
//            vehicleParts2[STEERING]->changePosition(1.35, -0.27, 0.65);
          }else if((shuttleActive || shuttle_chassis) && *onChassis){
            vehicleParts2[STEERING]->changeRotation(20, -steeringAngle, 0);
//            vehicleParts2[STEERING]->changePosition(1.35, -0.27, 0.65);
          }else if(!*onChassis){
            vehicleParts2[STEERING]->changePosition(1.85, -0.27, 0.6);
            vehicleParts2[STEERING]->changeRotation(50, -steeringAngle, 0);
          }else
            vehicleParts2[STEERING]->changeRotation(50, -steeringAngle, 0);


        all_textures[10]->bind(m2_id_diffuse);
        all_textures[11]->bind(m2_id_normal);
        all_textures[12]->bind(m2_id_specular);
        vehicleParts2[STEERING]->drawModel();

        if(*onAxesTexture)
          all_textures[16]->bind(m2_id_diffuse);
        else
          all_textures[13]->bind(m2_id_diffuse);
        all_textures[14]->bind(m2_id_normal);
        all_textures[15]->bind(m2_id_specular);
        vehicleParts2[REAR_AXIS]->drawModel();
        vehicleParts2[CENTRAL_AXIS]->drawModel();
        vehicleParts2[FRONTAL_AXIS]->drawModel();
      }
      m2_program.release();
    }

    if(*onVehicle){
      if(*onChassis && (isVehicleCreated)){
        if(*onChassisRebounding && (vehicleUpwardAcceleration != nullptr)){
          if(db5active){
            vehicleParts3[DB5]->applyAcceleration(acceleration_effect);
            vehicleParts3[WINDOWS]->applyAcceleration(acceleration_effect);
          }else if(shuttleActive || shuttle_chassis){
            vehicleParts3[SHUTTLE_CHASSIS]->applyAcceleration(acceleration_effect);
            if(shuttleActive){
              vehicleParts3[SHUTTLE]->applyAcceleration(acceleration_effect);
              vehicleParts3[SHUTTLE_WINDOWS]->applyAcceleration(acceleration_effect);
            }
          }else{
            vehicleParts2[CHASSIS]->applyAcceleration(acceleration_effect);
          }
        }
        if(db5active){
          m3_program.bind();
          all_textures[17]->bind(m3_id_diffuse);
          all_textures[18]->bind(m3_id_specular);
          vehicleParts3[DB5]->drawModel();
          vehicleParts3[WINDOWS]->drawModel();
          m3_program.release();
        }else if(shuttleActive || shuttle_chassis){
          m3_program.bind();
          all_textures[17]->bind(m3_id_diffuse);
          all_textures[18]->bind(m3_id_specular);
          vehicleParts3[SHUTTLE_CHASSIS]->drawModel();
          if(shuttleActive){
            all_textures[19]->bind(m3_id_diffuse);
            vehicleParts3[SHUTTLE]->drawModel();

            all_textures[20]->bind(m3_id_diffuse);
            vehicleParts3[SHUTTLE_WINDOWS]->drawModel();
          }
          m3_program.release();
        }else{
          m2_program.bind();
          all_textures[17]->bind(m2_id_diffuse);
          all_textures[18]->bind(m2_id_normal);
          all_textures[19]->bind(m2_id_specular);
          vehicleParts2[CHASSIS]->drawModel();
          m2_program.release();
        }
      }
      if(isVehicleCreated){
        m4_program.bind();

        m4_program.setUniformValue(m4_vehicle, street_scooter_building);
        m4_program.setUniformValue(m4_normal_vehicle, street_scooter_building_normal);

        all_textures[all_textures.size() - 2]->bind(m4_id_diffuse);
        all_textures[4]->bind(m4_id_specular);
        buildings[STREET_SCOOTER]->drawModel();

        m4_program.setUniformValue(m4_vehicle, campus_boulevar_building);
        m4_program.setUniformValue(m4_normal_vehicle, campus_boulevar_building_normal);

        all_textures[all_textures.size() - 1]->bind(m4_id_diffuse);
        buildings[CAMPUS_BOULEVAR]->drawModel();
        m4_program.release();
      }
    }

    if(*onBox){
      if(!vh_program.bind()) return;
      vehicle->drawVehicle();
      vh_program.release();
    }
  }

  if(*onFrameCoordinates){
    !m2_program.bind();
    m2_program.setUniformValue(m2_static, true);
    all_textures[2]->bind(m2_id_diffuse);
    all_textures[3]->bind(m2_id_normal);
    all_textures[4]->bind(m2_id_specular);
    for(int i = 0; i < frameCoordinates.size(); i++)
      if(frameCoordinates[i] != nullptr)
        frameCoordinates[i]->drawModel();
    m2_program.release();
  }

//  QElapsedTimer timer;
//  timer.start();
  if(*onTrajectory){
    if(!tj_program.bind()) return;
    all_textures[0]->bind(tj_tex1);
    all_textures[1]->bind(tj_tex2);
    for(int i = 0; i < trajectories.size(); i++)
      if(trajectories[i] != nullptr && isTrajectoryActive->value(i))
        trajectories[i]->drawTrajectory();
    tj_program.release();
  }
//  qWarning("Total time required: %.4f milliseconds", 0.000001 * timer.nsecsElapsed());
}

void Scene::resizeGL( int w, int h ){
  glViewport(0, 0, w, h);

  proj.setToIdentity();
  proj.perspective(60.0, w/(float)h, 0.1, 3000.0);

  setLookAt(false);

  emit resizing();
}

void Scene::mousePressEvent(QMouseEvent *event){
  m_lastPos = event->pos();
}

void Scene::mouseMoveEvent(QMouseEvent *event){
  int dx = event->x() - m_lastPos.x();
  int dy = event->y() - m_lastPos.y();

  if(event->buttons() & Qt::LeftButton){

    setRotation(scenaryRotationX + 8 * dy, 0);
    setRotation(scenaryRotationY + 8 * dx, 1);

    camera_rotationMatrix.setToIdentity();
    if((vehicleYaw != nullptr) && *isCameraLocked)
      if(frameYaw != nullptr)
        staticVehicleYaw = (*vehicleYaw - *frameYaw) * toDEGREES;
      else
        staticVehicleYaw = *vehicleYaw * toDEGREES;

    float scrY = scenaryRotationX/anglePrecision;
    camera_rotationMatrix.rotate(QQuaternion::fromEulerAngles(scrY, staticVehicleYaw + scenaryRotationY/anglePrecision, scenaryRotationZ/anglePrecision));
    if((scrY < 270) && (scrY > 90))
      camera_upPos = QVector3D(0, -1, 0);
    else
      camera_upPos = QVector3D(0, 1, 0);

    setLookAt(true);
  }else if(event->buttons() & Qt::RightButton){

    camera_translation += camera_rotationMatrix * QVector3D(-dx * 0.04 * zoom, dy * 0.04 * zoom, 0);
    setLookAt(false);
  }
  emit mouseMoving();
  m_lastPos = event->pos();
}

void Scene::wheelEvent(QWheelEvent *event){
  if(event->angleDelta().ry() > 0) zooming();
  else zooming(false);
}

void Scene::zooming(const bool _in){
  float previousZoom = zoom;

  if(_in){
    zoom *= 0.9;
    zoom = (zoom < 0.05) ? 0.05 : zoom;
  }else{
    zoom *= 1.1;
    zoom = (zoom > 15)? 15 : zoom;
  }

  if(previousZoom != zoom) setLookAt(false);
  emit mouseMoving();
}

void Scene::setCoordinateConversor(CoordinateConversor *_conversor){
  the_conversor = _conversor;
}

void Scene::setRotation(int angle, const int axis){
  if(angle < 0)
    angle += 360 * anglePrecision;
  else if(angle > 360 * anglePrecision)
    angle -= 360 * anglePrecision;

  switch(axis){
  case 0:
    if(angle != scenaryRotationX) scenaryRotationX = angle;
  break;
  case 1:
    if(angle != scenaryRotationY) scenaryRotationY = angle;
  break;
  case 2:
    if(angle != scenaryRotationZ) scenaryRotationZ = angle;
  break;
  }
}

void Scene::setLookAt(const bool _rotate){
  if(*isStaticCamera){
    camera_centerPos = camera_lastPos + camera_translation;
    camera_eyePos = camera_rotationMatrix * (zoom * nonRotativeCamera_eyePos) + camera_centerPos;
  }else{
    if((vehicleYaw != nullptr) && *isCameraLocked){
      camera_rotationMatrix.setToIdentity();
      if(frameYaw != nullptr)
        staticVehicleYaw = (*vehicleYaw - *frameYaw) * toDEGREES;
      else
        staticVehicleYaw = *vehicleYaw * toDEGREES;

      float scrY = scenaryRotationX/anglePrecision;
      camera_rotationMatrix.rotate(QQuaternion::fromEulerAngles(scrY, staticVehicleYaw + scenaryRotationY/anglePrecision, scenaryRotationZ/anglePrecision));
      if((scrY < 270) && (scrY > 90))
        camera_upPos = QVector3D(0, -1, 0);
      else
        camera_upPos = QVector3D(0, 1, 0);
    }
    camera_lastPos = *vehicleTransformationMatrix * initial_camera_centerPos;
    camera_centerPos = camera_lastPos + camera_translation;
    camera_eyePos = camera_rotationMatrix * (zoom * nonRotativeCamera_eyePos) + camera_centerPos;
  }

  if(_rotate){
    view2.setToIdentity();
    view2.lookAt(camera_rotationMatrix * nonRotativeCamera_eyePos, QVector3D(), camera_upPos);
//    view2.lookAt(camera_rotationMatrix * nonRotativeCamera_eyePos, initial_camera_centerPos, camera_upPos);
    m_pvm = proj * view2 * m_scale;

    sx_program.bind();
    sx_program.setUniformValue(sx_pvm, m_pvm);
    sx_program.release();
  }

  view.setToIdentity();
  view.lookAt(camera_eyePos, camera_centerPos, camera_upPos);

  master_matrix = proj * view;

  ls_program.bind();
  ls_program.setUniformValue(ls_matrix, master_matrix);
  ls_program.release();

  //QVector3D sun = view * (lightPos + camera_centerPos);

  vehicleNormalMatrix = vehicleTransformationMatrix->normalMatrix();

  vh_program.bind();
  vh_program.setUniformValue(vh_proj, proj);
  vh_program.setUniformValue(vh_view, view);
  vh_program.setUniformValue(vh_light, lightPos);
  vh_program.setUniformValue(vh_camera, camera_eyePos);
  vh_program.setUniformValue(vh_normal, vehicleNormalMatrix);
  vh_program.release();

  gd_program.bind();
  gd_program.setUniformValue(gd_proj, proj);
  gd_program.setUniformValue(gd_view, view);
  gd_program.setUniformValue(gd_light, lightPos);
  gd_program.setUniformValue(gd_camera, camera_eyePos);
  gd_program.release();

  st_program.bind();
  st_program.setUniformValue(st_matrix, master_matrix * (*fixedFrameModel));
  st_program.setUniformValue(st_lightPos, lightPos);
  st_program.setUniformValue(st_viewPos, camera_eyePos);
  st_program.release();

  ob_program.bind();
  ob_program.setUniformValue(ob_matrix, master_matrix);
  ob_program.setUniformValue(ob_lightPos, lightPos);
  ob_program.setUniformValue(ob_viewPos, camera_eyePos);
  ob_program.release();

  m1_program.bind();
  m1_program.setUniformValue(m1_matrix, master_matrix);
  m1_program.setUniformValue(m1_lightPos, lightPos);
  m1_program.setUniformValue(m1_viewPos, camera_eyePos);
  m1_program.release();

  m2_program.bind();
  m2_program.setUniformValue(m2_matrix, master_matrix);
  m2_program.setUniformValue(m2_lightPos, lightPos);
  m2_program.setUniformValue(m2_viewPos, camera_eyePos);
  m2_program.release();

  m3_program.bind();
  m3_program.setUniformValue(m3_matrix, master_matrix);
  m3_program.setUniformValue(m3_lightPos, lightPos);
  m3_program.setUniformValue(m3_viewPos, camera_eyePos);
  m3_program.release();

  m4_program.bind();
  m4_program.setUniformValue(m4_matrix, master_matrix);
  m4_program.setUniformValue(m4_lightPos, lightPos);
  m4_program.setUniformValue(m4_viewPos, camera_eyePos);
  m4_program.release();

  tj_program.bind();
  tj_program.setUniformValue(tj_matrix, master_matrix);
  tj_program.setUniformValue(tj_lightPos, lightPos);
  tj_program.setUniformValue(tj_viewPos, camera_eyePos);
  tj_program.release();

  streets->updateMap();

  emit rotating();
  update();
}

void Scene::initTextures(QString _image, bool _mirrored, bool _inverted){
  //load the image file
  QImage texture(_image);

  //checks if the image file exist
  if(!texture.isNull()){
    //creates a new texture
    QOpenGLTexture *new_texture;

    if(_inverted) texture.invertPixels();

    if(_mirrored) new_texture = new QOpenGLTexture(texture.mirrored());
    else new_texture = new QOpenGLTexture(texture);

    //saves the texture in vector
    all_textures.append(new_texture);
    unsigned short texture_id(all_textures.length() - 1);

    // Set nearest filtering mode for texture minification
    all_textures[texture_id]->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    // Set bilinear filtering mode for texture magnification
    all_textures[texture_id]->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
    all_textures[texture_id]->setWrapMode(QOpenGLTexture::Repeat);
    all_textures[texture_id]->setAutoMipMapGenerationEnabled(true);
    //anisotropy improves the texture quality at long distances
    all_textures[texture_id]->setMaximumAnisotropy(16);
  }else{
    #ifdef ROS_DATA
      qWarning() << "*the file for the models was not found: " + _image;
    #else
      qWarning() << "the file for the models was not found: " + _image;
    #endif
  }
}

void Scene::modTexture(unsigned short _id, QString _image, bool _mirrored, bool _inverted){
  //load the image file
  QImage texture(_image);

  //checks if the image file exist
  if(!_image.isNull() && all_textures.size() > _id){
    if(all_textures[_id] != nullptr){
      all_textures[_id]->destroy();
      //all_textures[_id] = nullptr;
    }
    //creates a new texture
    QOpenGLTexture *new_texture;

    if(_inverted) texture.invertPixels();

    if(_mirrored) new_texture = new QOpenGLTexture(texture.mirrored());
    else new_texture = new QOpenGLTexture(texture);

    //saves the texture in vector
    all_textures[_id] = new_texture;
    // Set nearest filtering mode for texture minification
    all_textures[_id]->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    // Set bilinear filtering mode for texture magnification
    all_textures[_id]->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
    all_textures[_id]->setWrapMode(QOpenGLTexture::Repeat);
    all_textures[_id]->setAutoMipMapGenerationEnabled(true);
    //anisotropy improves the texture quality at long distances
    all_textures[_id]->setMaximumAnisotropy(16);
  }else
    qWarning() << "The path to 3D models folder was not correctly specified: " + E_MODELS3D;
}

void Scene::changeCompany(){
  if(isVehicleCreated){
    *whichCompany = (*whichCompany >= 2)? 0 : *whichCompany + 1;

    m1_program.bind();
    #ifdef ROS_DATA
      switch(*whichCompany){
      case NOBRAND:
        modTexture(5, E_MODELS3D + "textures/tire_diffuseNONE.png");
        modTexture(6, E_MODELS3D + "textures/tire_displacementNONE.png", true, true);
        modTexture(7, E_MODELS3D + "textures/tire_specularNONE.png");

        modTexture(9, E_MODELS3D + "textures/tire_displacementNONE2.png", true, true);
      break;
      case DHL:
        modTexture(5, E_MODELS3D + "textures/tire_diffuseDHL.png");
        modTexture(6, E_MODELS3D + "textures/tire_displacementDHL.png", true, true);
        modTexture(7, E_MODELS3D + "textures/tire_specularDHL.png");

        modTexture(9, E_MODELS3D + "textures/tire_displacementDHL2.png", true, true);
      break;
      default:
        modTexture(5, E_MODELS3D + "textures/tire_diffuse.png");
        modTexture(6, E_MODELS3D + "textures/tire_displacement.png", true, true);
        modTexture(7, E_MODELS3D + "textures/tire_specular.png");

        modTexture(9, E_MODELS3D + "textures/tire_displacement2.png", true, true);
      break;
      }
      m1_program.release();

      if(db5active){
        m3_program.bind();
        switch(*whichCompany){
        case NOBRAND:
          modTexture(17, E_MODELS3D + "textures/db5_diffuseNONE.png");
          modTexture(18, E_MODELS3D + "textures/db5_specularNONE.png");
        break;
        case DHL:
          modTexture(17, E_MODELS3D + "textures/db5_diffuseDHL.png");
          modTexture(18, E_MODELS3D + "textures/db5_specularDHL.png");
        break;
        default:
          modTexture(17, E_MODELS3D + "textures/db5_diffuse.png");
          modTexture(18, E_MODELS3D + "textures/db5_specular.png");
        break;
        }
        m3_program.release();
      }
    #else
      switch(*whichCompany){
      case NOBRAND:
        modTexture(5, qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_diffuseNONE.png");
        modTexture(6, qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_displacementNONE.png", true, true);
        modTexture(7, qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_specularNONE.png");

        modTexture(9, qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_displacementNONE2.png", true, true);
      break;
      case DHL:
        modTexture(5, qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_diffuseDHL.png");
        modTexture(6, qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_displacementDHL.png", true, true);
        modTexture(7, qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_specularDHL.png");

        modTexture(9, qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_displacementDHL2.png", true, true);
      break;
      default:
        modTexture(5, qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_diffuse.png");
        modTexture(6, qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_displacement.png", true, true);
        modTexture(7, qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_specular.png");

        modTexture(9, qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_displacement2.png", true, true);
      break;
      }
      m1_program.release();

      if(db5active){
        m3_program.bind();
        switch(*whichCompany){
        case NOBRAND:
          modTexture(17, qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/db5_diffuseNONE.png");
          modTexture(18, qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/db5_specularNONE.png");
        break;
        case DHL:
          modTexture(17, qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/db5_diffuseDHL.png");
          modTexture(18, qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/db5_specularDHL.png");
        break;
        default:
          modTexture(17, qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/db5_diffuse.png");
          modTexture(18, qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/db5_specular.png");
        break;
        }
        m3_program.release();
      }
    #endif
    update();
  }
}

void Scene::reloadMap(){
  *fixedFrameModel = previousFixedFrameModel;
  streets->loadMap();
  nextFrame();
}

void Scene::reloadMapModel(){
  previousFixedFrameModel = *navigationTransformationMatrix;
  previousFixedFrameModel.rotate(QQuaternion::fromEulerAngles(0, -*frameYaw * toDEGREES - 90, 0));
}

void Scene::setTopView(){
  scenaryRotationX = -1439;
  scenaryRotationY = 0;
  scenaryRotationZ = 0;
  zoom = 3.75;
  camera_eyePos = QVector3D(0, 0, 45);
  if(*isStaticCamera)
    camera_lastPos = *vehicleTransformationMatrix * QVector3D();
  else
    camera_lastPos = QVector3D();
  camera_upPos = QVector3D(0, 1, 0);
  camera_translation = QVector3D();
  camera_rotationMatrix.setToIdentity();
  if((vehicleYaw != nullptr) && *isCameraLocked)
    if(frameYaw != nullptr)
      staticVehicleYaw = (*vehicleYaw - *frameYaw) * toDEGREES;
    else
      staticVehicleYaw = *vehicleYaw * toDEGREES;

  camera_rotationMatrix.rotate(QQuaternion::fromEulerAngles(scenaryRotationX/anglePrecision, staticVehicleYaw + scenaryRotationY/anglePrecision, scenaryRotationZ/anglePrecision));
  setLookAt(true);
  emit mouseMoving();
}

void Scene::setStandardView(){
  scenaryRotationX = initial_scenaryRotationX;
  scenaryRotationY = initial_scenaryRotationY;
  scenaryRotationZ = initial_scenaryRotationZ;
  zoom = initial_zoom;
  camera_eyePos = initial_camera_eyePos;
  if(*isStaticCamera)
    camera_lastPos = *vehicleTransformationMatrix * initial_camera_centerPos;
  else
    camera_lastPos = initial_camera_centerPos;
  camera_upPos = initial_camera_upPos;
  camera_centerPos = initial_camera_centerPos;
  camera_translation = QVector3D();
  camera_rotationMatrix.setToIdentity();
  if((vehicleYaw != nullptr))// && *isCameraLocked){
    if(frameYaw != nullptr)
      staticVehicleYaw = (*vehicleYaw - *frameYaw) * toDEGREES;
    else
      staticVehicleYaw = *vehicleYaw * toDEGREES;

  camera_rotationMatrix.rotate(QQuaternion::fromEulerAngles(scenaryRotationX/anglePrecision, staticVehicleYaw + scenaryRotationY/anglePrecision, scenaryRotationZ/anglePrecision));
  setLookAt(true);
  emit mouseMoving();
}

void Scene::nextFrame(){
  setLookAt(true);
  updateRotation = true;

  m1_program.bind();
  m1_program.setUniformValue(m1_vehicle, *vehicleTransformationMatrix);
  m1_program.setUniformValue(m1_normal_vehicle, vehicleTransformationMatrix->normalMatrix());
  m1_program.release();

  m2_program.bind();
  m2_program.setUniformValue(m2_vehicle, *vehicleTransformationMatrix);
  m2_program.setUniformValue(m2_normal_vehicle, vehicleTransformationMatrix->normalMatrix());
  m2_program.release();

  m3_program.bind();
  m3_program.setUniformValue(m3_vehicle, *vehicleTransformationMatrix);
  m3_program.setUniformValue(m3_normal_vehicle, vehicleTransformationMatrix->normalMatrix());
  m3_program.release();

//  vector2d position_building_1(the_conversor->distances(*vehicle_latitude, *vehicle_longitude, 50.78418, 6.042664));
  QVector2D building1(the_conversor->GPStoXY(50.78418, 6.042664));
  QVector2D building2(the_conversor->GPStoXY(50.78236, 6.046611));
//  qWarning() << building1 << building2;
//  qWarning() << "street scooter" << position_building_1.x << position_building_1.y;
//  vector2d position_building_2(the_conversor->distances(*vehicle_latitude, *vehicle_longitude, 50.78236, 6.046611));
//  qWarning() << "campus boulevar" << position_building_2.x << position_building_2.y;
  street_scooter_building.setToIdentity();
  street_scooter_building.translate(building1.y(), *vehiclePositionZ, building1.x());
//  street_scooter_building.rotate(QQuaternion::fromEulerAngles(-*framePitch * toDEGREES, -*frameYaw * toDEGREES, -*frameRoll * toDEGREES));
  street_scooter_building_normal = street_scooter_building.normalMatrix();

  campus_boulevar_building.setToIdentity();
  campus_boulevar_building.translate(building2.y(), *vehiclePositionZ, building2.x());
//  campus_boulevar_building.rotate(QQuaternion::fromEulerAngles(-*framePitch * toDEGREES, -*frameYaw * toDEGREES, -*frameRoll * toDEGREES));
  campus_boulevar_building_normal = campus_boulevar_building.normalMatrix();

  vh_program.bind();
  vh_program.setUniformValue(vh_model, *vehicleTransformationMatrix);
  vh_program.release();

  if(*onStreets || *onSignalization)
    streets->updateMap();
  if(*onMovables){
    for(int i = 0; i < obstacles.size(); i++)
      if(obstacles[i] != nullptr && isObstacleActive->value(i))
        obstacles[i]->updateObstacle();
    for(int i = 0; i < cylinders.size(); i++)
      if(cylinders[i] != nullptr && isCylinderActive->value(i))
        cylinders[i]->updateCylinders();
  }
  if(*onCloud)
    for(int i = 0; i < lasers.size(); i++)
      if(lasers[i] != nullptr && isLaserActive->value(i))
        lasers[i]->updateCloud();
  if(*onGround)
    for(int i = 0; i < ground.size(); i++)
      if(ground[i] != nullptr && isGroundActive->value(i))
        ground[i]->updateGround();
  update();
}

const QMatrix4x4 *const Scene::masterMatrix(){
  return &master_matrix;
}

void Scene::ChangeCameraPosition(const float _x, const float _y, const float _z){
  initial_camera_eyePos = nonRotativeCamera_eyePos = camera_eyePos = QVector3D(-_y, _z, -_x);
  setLookAt(true);
}

void Scene::ChangeCameraUpVector(const float _x, const float _y, const float _z){
  initial_camera_upPos = camera_upPos = QVector3D(-_y, _z, -_x);
  setLookAt(true);
}

void Scene::ChangeCameraLookingAt(const float _x, const float _y, const float _z){
  initial_camera_centerPos = camera_lastPos = camera_centerPos = QVector3D(-_y, _z, -_x);
  setLookAt(true);
}

void Scene::ChangeScenaryRotation(const float _rotationX, const float _rotationY, const float _rotationZ){
  initial_scenaryRotationX = scenaryRotationX = _rotationX * anglePrecision * toDEGREES;
  initial_scenaryRotationY = scenaryRotationY = _rotationY * anglePrecision * toDEGREES;
  initial_scenaryRotationZ = scenaryRotationZ = _rotationZ * anglePrecision * toDEGREES;
  camera_rotationMatrix.setToIdentity();
  camera_rotationMatrix.rotate(QQuaternion::fromEulerAngles(_rotationX, _rotationY, _rotationZ));
  if((_rotationX < 270) && (_rotationX > 90))
    camera_upPos = QVector3D(0, -1, 0);
  else
    camera_upPos = QVector3D(0, 1, 0);

  setLookAt(true);
}

void Scene::ChangeScenaryZoom(const float _zoom){
  initial_zoom = zoom = _zoom;
  setLookAt(true);
}

unsigned int Scene::addLaser(const vector<LaserCloudData> *const _laserCloud, const QMatrix4x4 *const _transformationMatrix,
                             const float _pointSize, const bool _gradient, const bool _colorized,
                             const bool _isIntensityHigherThanOne){
  ls_program.bind();
  LaserClouds *laser = new LaserClouds(&ls_program, _laserCloud, _pointSize, _gradient, _colorized, _isIntensityHigherThanOne);
  if(_transformationMatrix != nullptr)
    laser->setTransformationMatrix(_transformationMatrix);
  laser->updateCloud();
  lasers.append(laser);
  ls_program.release();

  return lasers.length() - 1;
}

unsigned int Scene::addLaser(const vector<LaserCloudData> *const _laserCloud, const QMatrix4x4 *const _transformationMatrix,
                             const float _pointSize, const float _SolidColor[3], const bool _gradient, const bool _colorized,
                             const bool _isIntensityHigherThanOne){
  ls_program.bind();
  LaserClouds *laser = new LaserClouds(&ls_program, _laserCloud, _pointSize, _gradient, _colorized, _isIntensityHigherThanOne);
  if(_transformationMatrix != nullptr)
    laser->setTransformationMatrix(_transformationMatrix);
  laser->setSolidColor(_SolidColor, _gradient);
  laser->updateCloud();
  lasers.append(laser);
  ls_program.release();

  return lasers.length() - 1;
}

unsigned int Scene::addLaser(const vector<LaserCloudData> *const _laserCloud, const QMatrix4x4 *const _transformationMatrix,
                             const float _pointSize, vector<array<float, 3> > _palette, const bool _gradient,
                             const bool _colorized, const bool _isIntensityHigherThanOne){
  ls_program.bind();
  LaserClouds *laser = new LaserClouds(&ls_program, _laserCloud, _pointSize, _gradient, _colorized, _isIntensityHigherThanOne);
  if(_transformationMatrix != nullptr)
    laser->setTransformationMatrix(_transformationMatrix);
  laser->setLaserColorPalette(_palette, _gradient);
  laser->updateCloud();
  lasers.append(laser);
  ls_program.release();

  return lasers.length() - 1;
}

unsigned int Scene::addLaser(const vector<pointRGB> *const _laserCloud, const QMatrix4x4 *const _transformationMatrix,
                             const float _pointSize, const bool _gradient, const bool _colorized,
                             const bool _isIntensityHigherThanOne){
  ls_program.bind();
  LaserClouds *laser = new LaserClouds(&ls_program, _laserCloud, _pointSize, _gradient, _colorized, _isIntensityHigherThanOne);
  if(_transformationMatrix != nullptr)
    laser->setTransformationMatrix(_transformationMatrix);
  laser->updateCloud();
  lasers.append(laser);
  ls_program.release();

  return lasers.length() - 1;
}

unsigned int Scene::addLaser(const vector<pointRGB> *const _laserCloud, const QMatrix4x4 *const _transformationMatrix,
                             const float _pointSize, const float _SolidColor[3], const bool _gradient,
                              const bool _colorized, const bool _isIntensityHigherThanOne){
  ls_program.bind();
  LaserClouds *laser = new LaserClouds(&ls_program, _laserCloud, _pointSize, _gradient, _colorized, _isIntensityHigherThanOne);
  if(_transformationMatrix != nullptr)
    laser->setTransformationMatrix(_transformationMatrix);
  laser->setSolidColor(_SolidColor, _gradient);
  laser->updateCloud();
  lasers.append(laser);
  ls_program.release();

  return lasers.length() - 1;
}

unsigned int Scene::addLaser(const vector<pointRGB> *const _laserCloud, const QMatrix4x4 *const _transformationMatrix,
                             const float _pointSize, vector<array<float, 3> > _palette, const bool _gradient,
                             const bool _colorized, const bool _isIntensityHigherThanOne){
  ls_program.bind();
  LaserClouds *laser = new LaserClouds(&ls_program, _laserCloud, _pointSize, _gradient, _colorized, _isIntensityHigherThanOne);
  if(_transformationMatrix != nullptr)
    laser->setTransformationMatrix(_transformationMatrix);
  laser->setLaserColorPalette(_palette, _gradient);
  laser->updateCloud();
  lasers.append(laser);
  ls_program.release();

  return lasers.length() - 1;
}

unsigned int Scene::addLaser(const vector<pointDAI> *const _laserCloud, const float _pointSize,
                             const float _angularResolutionDEGREES, const float _verticalAngleBetweenLayersDEGREES,
                             const QMatrix4x4 *const _transformationMatrix, const bool _gradient,
                             const bool _colorized, const bool _isIntensityHigherThanOne){
  ls_program.bind();
  LaserClouds *laser = new LaserClouds(&ls_program, _laserCloud, _pointSize, _angularResolutionDEGREES,
                                       _verticalAngleBetweenLayersDEGREES, _gradient, _colorized,
                                       _isIntensityHigherThanOne);
  if(_transformationMatrix != nullptr)
    laser->setTransformationMatrix(_transformationMatrix);
  laser->updateCloud();
  lasers.append(laser);
  ls_program.release();

  return lasers.length() - 1;
}

unsigned int Scene::addLaser(const vector<pointDAI> *const _laserCloud, const float _pointSize,
                             const float _angularResolutionDEGREES, const float _verticalAngleBetweenLayersDEGREES,
                             const QMatrix4x4 *const _transformationMatrix, const float _SolidColor[3],
                             const bool _gradient, const bool _colorized, const bool _isIntensityHigherThanOne){
  ls_program.bind();
  LaserClouds *laser = new LaserClouds(&ls_program, _laserCloud, _pointSize, _angularResolutionDEGREES,
                                       _verticalAngleBetweenLayersDEGREES, _gradient, _colorized,
                                       _isIntensityHigherThanOne);
  if(_transformationMatrix != nullptr)
    laser->setTransformationMatrix(_transformationMatrix);
  laser->setSolidColor(_SolidColor, _gradient);
  laser->updateCloud();
  lasers.append(laser);
  ls_program.release();

  return lasers.length() - 1;
}

unsigned int Scene::addLaser(const vector<pointDAI> *const _laserCloud, const float _pointSize,
                             const float _angularResolutionDEGREES, const float _verticalAngleBetweenLayersDEGREES,
                             const QMatrix4x4 *const _transformationMatrix, vector<array<float, 3> > _palette,
                             const bool _gradient, const bool _colorized, const bool _isIntensityHigherThanOne){
  ls_program.bind();
  LaserClouds *laser = new LaserClouds(&ls_program, _laserCloud, _pointSize, _angularResolutionDEGREES,
                                       _verticalAngleBetweenLayersDEGREES, _gradient, _colorized,
                                       _isIntensityHigherThanOne);
  if(_transformationMatrix != nullptr)
    laser->setTransformationMatrix(_transformationMatrix);
  laser->setLaserColorPalette(_palette, _gradient);
  laser->updateCloud();
  lasers.append(laser);
  ls_program.release();

  return lasers.length() - 1;
}

bool Scene::setLaserColorPalette(unsigned int _id, vector<array<float, 3> > _palette, const bool _gradient){
  if((lasers.size() < _id) || (lasers[_id] == nullptr))
    return false;
  else{
    lasers[_id]->setLaserColorPalette(_palette, _gradient);
    return true;
  }
}

bool Scene::setLaserSolidColor(unsigned int _id, const float _SolidColor[3], const bool _gradient){
  if((lasers.size() < _id) || (lasers[_id] == nullptr))
    return false;
  else{
    lasers[_id]->setSolidColor(_SolidColor, _gradient);
    return true;
  }
}

bool Scene::ignoreSolidColor(unsigned int _id, const bool _ignore){
  if((lasers.size() < _id) || (lasers[_id] == nullptr))
    return false;
  else{
    lasers[_id]->ignoreSolidColor(_ignore);
    return true;
  }
}

bool Scene::setLaserOptions(unsigned int _id, const bool _gradient, const bool _colorized,
                            const bool _ignoreSolidColor, const bool _isIntensityHigherThanOne){
  if((lasers.size() < _id) || (lasers[_id] == nullptr))
    return false;
  else{
    lasers[_id]->setOptions(_gradient, _colorized, _ignoreSolidColor, _isIntensityHigherThanOne);
    return true;
  }
}

bool Scene::setLaserTransformationMatrix(unsigned int _id, const QMatrix4x4 *const _transformationMatrix){
  if((lasers.size() < _id) || (lasers[_id] == nullptr))
    return false;
  else{
    lasers[_id]->setTransformationMatrix(_transformationMatrix);
    return true;
  }
}

bool Scene::translateLaserMatrix(unsigned int _id, const float _x, const float _y, const float _z){
  if((lasers.size() < _id) || (lasers[_id] == nullptr))
    return false;
  else{
    lasers[_id]->translateLaserMatrix(_x, _y, _z);
    return true;
  }
}

bool Scene::rotateLaserMatrix(unsigned int _id, const float _pitch, const float _yaw, const float _roll){
  if((lasers.size() < _id) || (lasers[_id] == nullptr))
    return false;
  else{
    lasers[_id]->rotateLaserMatrix(_pitch, _yaw, _roll);
    return true;
  }
}

bool Scene::updateCloud(unsigned int _id){
  if((lasers.size() < _id) || (lasers[_id] == nullptr))
    return false;
  else{
    lasers[_id]->updateCloud();
    return true;
  }
}

bool Scene::deleteLaser(unsigned int _id){
  if((lasers.size() < _id) || (lasers[_id] == nullptr))
    return false;
  else{
    delete lasers[_id];
    lasers[_id] = nullptr;
    return true;
  }
}

void Scene::purgeLaserClouds(){
  for(int i = 0; i < lasers.size(); i++)
    if(lasers[i] != nullptr)
      delete lasers[i];
  lasers.clear();
  update();
}

unsigned int Scene::addObstacles(const vector<Obstacle> *const _obstacles, const QMatrix4x4 *const _transformationMatrix){
  ob_program.bind();
  MovingObjects *obstacle = new MovingObjects(&ob_program, _obstacles);
  obstacle->setTransformationMatrix(_transformationMatrix);
  obstacles.append(obstacle);
  ob_program.release();

  return obstacles.length() - 1;
}

bool Scene::setObstacleTransformationMatrix(unsigned int _id, QMatrix4x4 *_transformationMatrix){
  if((obstacles.size() < _id) || (obstacles[_id] == nullptr))
    return false;
  else{
    obstacles[_id]->setTransformationMatrix(_transformationMatrix);
    return true;
  }
}

bool Scene::translateObstacleMatrix(unsigned int _id, const float _translationX, const float _translationY, const float _translationZ){
  if((obstacles.size() < _id) || (obstacles[_id] == nullptr))
    return false;
  else{
    obstacles[_id]->translateObstacleMatrix(_translationX, _translationY, _translationZ);
    return true;
  }
}

bool Scene::rotateObstacleMatrix(unsigned int _id, const float _pitch, const float _yaw, const float _roll){
  if((obstacles.size() < _id) || (obstacles[_id] == nullptr))
    return false;
  else{
    obstacles[_id]->rotateObstacleMatrix(_pitch, _yaw, _roll);
    return true;
  }
}

bool Scene::deleteObstacles(unsigned int _id){
  if((obstacles.size() < _id) || (obstacles[_id] == nullptr))
    return false;
  else{
    delete obstacles[_id];
    obstacles[_id] = nullptr;
    return true;
  }
}

void Scene::purgeObstacles(){
  for(int i = 0; i < obstacles.size(); i++)
    if(obstacles[i] != nullptr)
      delete obstacles[i];
  obstacles.clear();
  update();
}

unsigned int Scene::addCylinders(const vector<Obstacle> *const _cylinders, const QMatrix4x4 *const _transformationMatrix){
  ob_program.bind();
  CylindricalObjects *cylinder = new CylindricalObjects(&ob_program, _cylinders);
  cylinder->setTransformationMatrix(_transformationMatrix);
  cylinders.append(cylinder);
  ob_program.release();

  return cylinders.length() - 1;
}

bool Scene::setCylinderTransformationMatrix(unsigned int _id, QMatrix4x4 *_transformationMatrix){
  if((cylinders.size() < _id) || (cylinders[_id] == nullptr))
    return false;
  else{
    cylinders[_id]->setTransformationMatrix(_transformationMatrix);
    return true;
  }
}

bool Scene::translateCylinderMatrix(unsigned int _id, const float _translationX, const float _translationY,
                                    const float _translationZ){
  if((cylinders.size() < _id) || (cylinders[_id] == nullptr))
    return false;
  else{
    cylinders[_id]->translateCylindersMatrix(_translationX, _translationY, _translationZ);
    return true;
  }
}

bool Scene::rotateCylinderMatrix(unsigned int _id, const float _pitch, const float _yaw, const float _roll){
  if((cylinders.size() < _id) || (cylinders[_id] == nullptr))
    return false;
  else{
    cylinders[_id]->rotateCylindersMatrix(_pitch, _yaw, _roll);
    return true;
  }
}

bool Scene::deleteCylinder(unsigned int _id){
  if((cylinders.size() < _id) || (cylinders[_id] == nullptr))
    return false;
  else{
    delete cylinders[_id];
    cylinders[_id] = nullptr;
    return true;
  }
}

void Scene::purgeCylinders(){
  for(int i = 0; i < cylinders.size(); i++)
    if(cylinders[i] != nullptr)
      delete cylinders[i];
  cylinders.clear();
  update();
}

unsigned int Scene::addTrajectory(const vector<vector<trajectoryData> > *const _trajectory, const QMatrix4x4 *const _transformationMatrix){
  tj_program.bind();
  Trajectory *trajectorial = new Trajectory(&tj_program, _trajectory);
  if(_transformationMatrix != nullptr) trajectorial->setTransformationMatrix(_transformationMatrix);
  trajectories.append(trajectorial);
  tj_program.release();

  return trajectories.length() - 1;
}

bool Scene::setTrajectoryTransformationMatrix(unsigned int _id, const QMatrix4x4 *const _transformationMatrix){
  if((trajectories.size() < _id) || (trajectories[_id] == nullptr))
    return false;
  else{
    trajectories[_id]->setTransformationMatrix(_transformationMatrix);
    return true;
  }
}

bool Scene::translateTrajectoryMatrix(unsigned int _id, const float _translationX, const float _translationY, const float _translationZ){
  if((trajectories.size() < _id) || (trajectories[_id] == nullptr))
    return false;
  else{
    trajectories[_id]->translateTrajectoryMatrix(_translationX, _translationY, _translationZ);
    return true;
  }
}

bool Scene::rotateTrajectoryMatrix(unsigned int _id, const float _pitch, const float _yaw, const float _roll){
  if((trajectories.size() < _id) || (trajectories[_id] == nullptr))
    return false;
  else{
    trajectories[_id]->rotateTrajectoryMatrix(_pitch, _yaw, _roll);
    return true;
  }
}

bool Scene::updateTrajectory(unsigned int _id){
  if((trajectories.size() < _id) || (trajectories[_id] == nullptr))
    return false;
  else{
    trajectories[_id]->updateTrajectory();
    return true;
  }
}

bool Scene::updateAllTrajectories(){
  for(int i = 0; i < trajectories.size(); i++)
    if(trajectories[i] != nullptr)
      if(isTrajectoryActive->value(i))
        trajectories[i]->updateTrajectory();
}

bool Scene::deleteTrajectory(unsigned int _id){
  if((trajectories.size() < _id) || (trajectories[_id] == nullptr))
    return false;
  else{
    delete trajectories[_id];
    trajectories[_id] = nullptr;
    return true;
  }
}

void Scene::purgeTrajectories(){
  for(int i = 0; i < trajectories.size(); i++)
    if(trajectories[i] != nullptr)
      delete trajectories[i];
  trajectories.clear();
  update();
}

bool Scene::setMapPosition(double *_latitude, double *_longitude){
  streets->isInstalling = isInstalling;
  return streets->setMapPosition(_latitude, _longitude);
}

bool Scene::setMapRange(unsigned int _width, unsigned int _length){
  streets->isInstalling = isInstalling;
  return streets->setMapRange(_width, _length);
}

void Scene::setMapTransformationMatrix(QMatrix4x4 *_transformationMatrix){
  streets->isInstalling = isInstalling;
  streets->setMapTransformationMatrix(_transformationMatrix);
}

void Scene::translateMap(const float _x, const float _y, const float _z){
  streets->isInstalling = isInstalling;
  streets->translateMap(_x, _y, _z);
}

void Scene::rotateMap(const float _pitch, const float _yaw, const float _roll){
  streets->isInstalling = isInstalling;
  streets->rotateMap(_pitch, _yaw, _roll);
}

bool Scene::forceUpdateMap(){
  return streets->forceUpdateMap();
}

QString Scene::seeMapFolderPath(){
  return streets->seeFolderPath();
}

// ---------------------------------------- Ground handling ---------------------------------------- //

unsigned int Scene::addGround(const vector<Boxes> *const _boxes, const bool _drawLines,
                              const QMatrix4x4 *const _transformationMatrix){
  gd_program.bind();
  Ground *newGround = new Ground(&gd_program, _boxes, _drawLines);
  newGround->setTransformationMatrix(_transformationMatrix);
  ground.append(newGround);
  gd_program.release();

  return ground.length() - 1;
}

bool Scene::setGroundTransformationMatrix(unsigned int _id, const QMatrix4x4 *const _transformationMatrix){
  if(ground.size() < _id) return false;
  else if(ground[_id] == nullptr) return false;
  else{
    ground[_id]->setTransformationMatrix(_transformationMatrix);
    return true;
  }
}

bool Scene::setGroundWithLines(unsigned int _id, const bool _drawLines){
  if(ground.size() < _id) return false;
  else if(ground[_id] == nullptr) return false;
  else{
    ground[_id]->setLines(_drawLines);
    return true;
  }
}

bool Scene::translateGround(unsigned int _id, const float _translationX, const float _translationY, const float _translationZ){
  if(ground.size() < _id) return false;
  else if(ground[_id] == nullptr) return false;
  else{
    ground[_id]->translateGround(_translationX, _translationY, _translationZ);
    return true;
  }
}

bool Scene::rotateGround(unsigned int _id, const float _pitch, const float _yaw, const float _roll){
  if(ground.size() < _id) return false;
  else if(ground[_id] == nullptr) return false;
  else{
    ground[_id]->rotateGround(_pitch, _yaw, _roll);
    return true;
  }
}

bool Scene::updateGround(unsigned int _id){
  if(ground.size() < _id) return false;
  else if(ground[_id] == nullptr) return false;
  else{
    ground[_id]->updateGround();
    return true;
  }
}

bool Scene::updateAllGround(){
  for(int i = 0; i < ground.length(); i++)
    if(ground[i] != nullptr)
      ground[i]->updateGround();

  return (ground.length() <= 0)? false : true;
}

bool Scene::deleteGround(unsigned int _id){
  if(ground.size() < _id) return false;
  else if(ground[_id] == nullptr) return false;
  else{
    delete ground[_id];
    return true;
  }
}

void Scene::purgeGround(){
  for(int i = 0; i < ground.length(); i++)
    if(ground[i] != nullptr)
      delete ground[i];
}

// ---------------------------------------- Vehicle ---------------------------------------- //

bool Scene::addVehicle(const unsigned int _model){
  db5active = (_model == DB5)? true : false;
  shuttleActive = (_model == SHUTTLE)? true : false;
  shuttle_chassis = (_model == SHUTTLE || _model == SHUTTLE_CHASSIS)? true : false;

  if(!isVehicleCreated){

    //creating the first section of the vehicle
    m1_program.bind();
    #ifdef ROS_DATA
      //left tires
      switch(*whichCompany){
      case NOBRAND:
        //5:
        initTextures(E_MODELS3D + "textures/tire_diffuseNONE.png");
        //6:
        initTextures(E_MODELS3D + "textures/tire_displacementNONE.png", true, true);
        //7:
        initTextures(E_MODELS3D + "textures/tire_specularNONE.png");
      break;
      case DHL:
        //5:
        initTextures(E_MODELS3D + "textures/tire_diffuseDHL.png");
        //6:
        initTextures(E_MODELS3D + "textures/tire_displacementDHL.png", true, true);
        //7:
        initTextures(E_MODELS3D + "textures/tire_specularDHL.png");
      break;
      default:
        //5:
        initTextures(E_MODELS3D + "textures/tire_diffuse.png");
        //6:
        initTextures(E_MODELS3D + "textures/tire_displacement.png", true, true);
        //7:
        initTextures(E_MODELS3D + "textures/tire_specular.png");
      break;
      }
      //8:
      initTextures(E_MODELS3D + "textures/tire_normal.png");
      //right tires
      switch(*whichCompany){
      case NOBRAND:
        //9:
        initTextures(E_MODELS3D + "textures/tire_displacementNONE2.png", true, true);
      break;
      case DHL:
        //9:
        initTextures(E_MODELS3D + "textures/tire_displacementDHL2.png", true, true);
      break;
      default:
        //9:
        initTextures(E_MODELS3D + "textures/tire_displacement2.png", true, true);
      break;
      }
    #else
      //left tires
      switch(*whichCompany){
      case NOBRAND:
        //5:
        initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_diffuseNONE.png");
        //6:
        initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_displacementNONE.png", true, true);
        //7:
        initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_specularNONE.png");
      break;
      case DHL:
        //5:
        initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_diffuseDHL.png");
        //6:
        initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_displacementDHL.png", true, true);
        //7:
        initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_specularDHL.png");
      break;
      default:
        //5:
        initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_diffuse.png");
        //6:
        initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_displacement.png", true, true);
        //7:
        initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_specular.png");
      break;
      }
      //8:
      initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_normal.png");
      //right tires
      switch(*whichCompany){
      case NOBRAND:
        //9:
        initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_displacementNONE2.png", true, true);
      break;
      case DHL:
        //9:
        initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_displacementDHL2.png", true, true);
      break;
      default:
        //9:
        initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/tire_displacement2.png", true, true);
      break;
      }
    #endif

    ThreeDimentionalModelLoaderParallax *tireFrontalLeft = new ThreeDimentionalModelLoaderParallax(TIRE_LEFT, &m1_program);
    ThreeDimentionalModelLoaderParallax *tireFrontalRight = new ThreeDimentionalModelLoaderParallax(TIRE_RIGHT, &m1_program);
    ThreeDimentionalModelLoaderParallax *tireRearLeft = new ThreeDimentionalModelLoaderParallax(TIRE_LEFT, &m1_program);
    ThreeDimentionalModelLoaderParallax *tireRearRight = new ThreeDimentionalModelLoaderParallax(TIRE_RIGHT, &m1_program);

    vehicleParts1[FRONTAL_LEFT_TIRE] = tireFrontalLeft;
    vehicleParts1[FRONTAL_RIGHT_TIRE] = tireFrontalRight;
    vehicleParts1[REAR_LEFT_TIRE] = tireRearLeft;
    vehicleParts1[REAR_RIGHT_TIRE] = tireRearRight;
    m1_program.release();

    //creating the second section of the vehicle
    m2_program.bind();

    #ifdef ROS_DATA
      //10:
      initTextures(E_MODELS3D + "textures/steering_diffuse.png");
      //11:
      initTextures(E_MODELS3D + "textures/steering_normal.png");
      //12:
      initTextures(E_MODELS3D + "textures/steering_specular.png");

      //13:
      initTextures(E_MODELS3D + "textures/bar_diffuse.png");
      //14:
      initTextures(E_MODELS3D + "textures/bar_normal.png");
      //15:
      initTextures(E_MODELS3D + "textures/bar_specular.png");
      //16:
      initTextures(E_MODELS3D + "textures/bar_diffuse2.png");
    #else
      //10:
      initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/steering_diffuse.png");
      //11:
      initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/steering_normal.png");
      //12:
      initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/steering_specular.png");

      //13:
      initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/bar_diffuse.png");
      //14:
      initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/bar_normal.png");
      //15:
      initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/bar_specular.png");
      //16:
      initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/bar_diffuse2.png");
    #endif

    ThreeDimentionalModelLoaderNormals *frontalAxis = new ThreeDimentionalModelLoaderNormals(BAR, &m2_program);
    ThreeDimentionalModelLoaderNormals *centralAxis = new ThreeDimentionalModelLoaderNormals(BAR, &m2_program);
    ThreeDimentionalModelLoaderNormals *rearAxis = new ThreeDimentionalModelLoaderNormals(BAR, &m2_program);
    ThreeDimentionalModelLoaderNormals *vehicleCoordinates = new ThreeDimentionalModelLoaderNormals(COORDINATES, &m2_program);
    ThreeDimentionalModelLoaderNormals *steeringWheel = new ThreeDimentionalModelLoaderNormals(STEERING, &m2_program);

    vehicleParts2[FRONTAL_AXIS] = frontalAxis;
    vehicleParts2[CENTRAL_AXIS] = centralAxis;
    vehicleParts2[REAR_AXIS] = rearAxis;
    vehicleParts2[COORDINATES] = vehicleCoordinates;
    vehicleParts2[STEERING] = steeringWheel;

    m2_program.release();

    //modifying the first section of the vehicle
    vehicleParts1[FRONTAL_LEFT_TIRE]->changeRotation(0, 90, 0);
    vehicleParts1[FRONTAL_RIGHT_TIRE]->changeRotation(0, -90, 0);
    vehicleParts1[REAR_LEFT_TIRE]->changeRotation(0, 90, 0);
    vehicleParts1[REAR_RIGHT_TIRE]->changeRotation(0, -90, 0);
    setTireSize(0.65);
    setLateralAxesLength(1.5);

    //modifying the second section of the vehicle
    vehicleParts2[FRONTAL_AXIS]->changeRotation(0, 90, 0);
    vehicleParts2[REAR_AXIS]->changeRotation(0, 90, 0);
    vehicleParts2[STEERING]->changeRotation(50, 0, 0);
    vehicleParts2[COORDINATES]->changePosition(0, 0, 0.05);

    if(db5active){
      m3_program.bind();
      #ifdef ROS_DATA
        //17:
        initTextures(E_MODELS3D + "textures/db5_diffuse.png");
        //18:
        initTextures(E_MODELS3D + "textures/db5_specular.png");
      #else
        //17:
        initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/db5_diffuse.png");
        //18:
        initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/db5_specular.png");
      #endif

      ThreeDimentionalModelLoaderSimple *chassis = new ThreeDimentionalModelLoaderSimple(DB5, &m3_program);
      ThreeDimentionalModelLoaderSimple *windows = new ThreeDimentionalModelLoaderSimple(WINDOWS, &m3_program);

      vehicleParts3[DB5] = chassis;
      vehicleParts3[WINDOWS] = windows;
      m3_program.release();

      vehicleParts1[FRONTAL_LEFT_TIRE]->changePosition(2.55, 0.75, 0);
      vehicleParts1[FRONTAL_RIGHT_TIRE]->changePosition(2.55, -0.75, 0);
      vehicleParts1[REAR_LEFT_TIRE]->changePosition(0, 0.75, 0);
      vehicleParts1[REAR_RIGHT_TIRE]->changePosition(0, -0.75, 0);

      vehicleParts2[CENTRAL_AXIS]->changeSize(1, 1, 2.55);
      vehicleParts2[CENTRAL_AXIS]->changePosition(1.25, 0, 0);
      vehicleParts2[REAR_AXIS]->changePosition(2.55, 0, 0);
      vehicleParts2[STEERING]->changePosition(1.35, -0.3, 0.65);

      vehicleParts3[DB5]->changePosition(1.47);
      vehicleParts3[WINDOWS]->changePosition(1.47);
    }else if(shuttleActive || shuttle_chassis){
      m3_program.bind();
      #ifdef ROS_DATA
        //17:
        initTextures(E_MODELS3D + "textures/shuttle_diffuse.png");
        //18:
        initTextures(E_MODELS3D + "textures/shuttle_specular.png");
        if(shuttle_chassis){
          //19:
          initTextures(E_MODELS3D + "textures/shuttle_body_diffuse.png");
          //20:
          initTextures(E_MODELS3D + "textures/shuttle_windows_diffuse.png");
        }
      #else
        //17:
        initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/shuttle_diffuse.png");
        //18:
        initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/shuttle_specular.png");
        if(shuttleActive){
          //19:
          initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/shuttle_body_diffuse.png");
          //20:
          initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/shuttle_windows_diffuse.png");
        }
      #endif

      if(shuttleActive){
        ThreeDimentionalModelLoaderSimple *body = new ThreeDimentionalModelLoaderSimple(SHUTTLE, &m3_program);
        vehicleParts3[SHUTTLE] = body;

//        vehicleParts3[SHUTTLE]->changeRotation(0, 180, 0);
        vehicleParts3[SHUTTLE]->changePosition(1.47);

        ThreeDimentionalModelLoaderSimple *windows = new ThreeDimentionalModelLoaderSimple(SHUTTLE_WINDOWS, &m3_program);
        vehicleParts3[SHUTTLE_WINDOWS] = windows;

//        vehicleParts3[SHUTTLE]->changeRotation(0, 180, 0);
        vehicleParts3[SHUTTLE_WINDOWS]->changePosition(1.47);
      }

      ThreeDimentionalModelLoaderSimple *chassis = new ThreeDimentionalModelLoaderSimple(SHUTTLE_CHASSIS, &m3_program);
      vehicleParts3[SHUTTLE_CHASSIS] = chassis;

      m3_program.release();

      vehicleParts1[FRONTAL_LEFT_TIRE]->changePosition(2.48, 0.85, 0);
      vehicleParts1[FRONTAL_RIGHT_TIRE]->changePosition(2.48, -0.85, 0);
      vehicleParts1[REAR_LEFT_TIRE]->changePosition(0, 0.85, 0);
      vehicleParts1[REAR_RIGHT_TIRE]->changePosition(0, -0.85, 0);

      vehicleParts2[CENTRAL_AXIS]->changeSize(1, 1, 2.43);
      vehicleParts2[CENTRAL_AXIS]->changePosition(1.25, 0, 0);
      vehicleParts2[REAR_AXIS]->changePosition(2.43, 0, 0);
      vehicleParts2[STEERING]->changePosition(2.83, 0.53, 1);

      vehicleParts3[SHUTTLE_CHASSIS]->changeRotation(0, 180, 0);
      vehicleParts3[SHUTTLE_CHASSIS]->changePosition(2.47);

      vehicleParts2[FRONTAL_AXIS]->changeSize(1.8, 1, 1);
      vehicleParts2[REAR_AXIS]->changeSize(1.8, 1, 1);
    }else{
      m2_program.bind();
      #ifdef ROS_DATA
        //17:
        initTextures(E_MODELS3D + "textures/car_diffuse.png");
        //18:
        initTextures(E_MODELS3D + "textures/car_normal.png");
        //19:
        initTextures(E_MODELS3D + "textures/car_specular.png");
      #else
        //17:
        initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/car_diffuse.png");
        //18:
        initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/car_normal.png");
        //19:
        initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/car_specular.png");
      #endif

      ThreeDimentionalModelLoaderNormals *chassis = new ThreeDimentionalModelLoaderNormals(CHASSIS, &m2_program);
      vehicleParts2[CHASSIS] = chassis;
      m2_program.release();

      setVehicleDimensions();
      changeChassisPosition();
    }

    m4_program.bind();

    #ifdef ROS_DATA
      //21:
      initTextures(E_MODELS3D + "textures/street_scooter_diffuse.png");
      //22:
      initTextures(E_MODELS3D + "textures/campus_boulevar_diffuse.png");
    #else
      //21:
      initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/street_scooter_diffuse.png");
      //22:
      initTextures(qApp->applicationDirPath() + "/" + E_MODELS3D + "textures/campus_boulevar_diffuse.png");
    #endif

    ThreeDimentionalModelLoaderSimple *street_scooter = new ThreeDimentionalModelLoaderSimple(STREET_SCOOTER, &m4_program);
    buildings[STREET_SCOOTER] = street_scooter;
    buildings[STREET_SCOOTER]->changeRotation(0, -90, 0);

    ThreeDimentionalModelLoaderSimple *campus_boulevar = new ThreeDimentionalModelLoaderSimple(CAMPUS_BOULEVAR, &m4_program);
    buildings[CAMPUS_BOULEVAR] = campus_boulevar;
    buildings[CAMPUS_BOULEVAR]->changeRotation(0, -90, 0);

    m4_program.release();
    isVehicleCreated = true;

    return true;
  }else{
    return false;
  }
}

void Scene::setLateralAxesLength(const float _length){
  lateralAxisLength = _length;
  vehicleParts2[FRONTAL_AXIS]->changeSize(_length, 1, 1);
  vehicleParts2[REAR_AXIS]->changeSize(_length, 1, 1);
  setTirePosition();
}

void Scene::setTireSize(const float _diameter, const float _width){
  vehicleParts1[FRONTAL_LEFT_TIRE]->changeSize(_width, _diameter);
  vehicleParts1[FRONTAL_RIGHT_TIRE]->changeSize(_width, _diameter);
  vehicleParts1[REAR_LEFT_TIRE]->changeSize(_width, _diameter);
  vehicleParts1[REAR_RIGHT_TIRE]->changeSize(_width, _diameter);
  tireCircunference = _diameter * _PI / 360;
}

void Scene::setTirePosition(){
  float half = lateralAxisLength/2.0f;
  vehicleParts1[FRONTAL_LEFT_TIRE]->changePosition(posFrontalAxis + vh_posX, half, 0);
  vehicleParts1[FRONTAL_RIGHT_TIRE]->changePosition(posFrontalAxis + vh_posX, -half, 0);
  vehicleParts1[REAR_LEFT_TIRE]->changePosition(posRearAxis + vh_posX, half, 0);
  vehicleParts1[REAR_RIGHT_TIRE]->changePosition(posRearAxis + vh_posX, -half, 0);
}

void Scene::setAxesPosition(){
  vehicleParts2[FRONTAL_AXIS]->changePosition(posFrontalAxis + vh_posX, vh_posY, vh_posZ);
  vehicleParts2[CENTRAL_AXIS]->changePosition(longitudinalAxisLength + vh_posX, vh_posY, vh_posZ);
  vehicleParts2[REAR_AXIS]->changePosition(posRearAxis + vh_posX, vh_posY, vh_posZ);
}

void Scene::setVehicleDimensions(const float _A, const float _B, const float _C, const float _D,
                                 const float _E, const float _F, const float _height){
  posRearAxis = -_D - _B/2.0f;
  posFrontalAxis = _C + _B/2.0f;
  longitudinalAxisLength = _D - _C;
  vh_width = _F;
  setAxesPosition();

  float total = _C + _D + _B;
  vehicleParts2[CENTRAL_AXIS]->changeSize(1, 1, total);
  setTirePosition();
  vehicleParts2[STEERING]->changePosition(posFrontalAxis + vh_posX + steeringOriginX, vh_posY + steeringOriginY + vh_width/2.0f, vh_posZ + steeringOriginZ);
  vehicle->changeSize(_F + 0.1, _height, _A + 2 * _B + _C + _D + _E + 0.1);
}

void Scene::changeChassisPosition(const float _x, const float _y, const float _z){
  vh_posX = _x;
  vh_posY = _y;
  vh_posZ = _z;
  vehicleParts2[STEERING]->changePosition(posFrontalAxis + vh_posX + steeringOriginX, vh_posY + steeringOriginY + vh_width/2.0f, vh_posZ + steeringOriginZ);
  vehicleParts2[CHASSIS]->changePosition(_x, _y, _z);
  setAxesPosition();
  setTirePosition();
  vehicle->translateVehicle(_x, _y, _z);
}

void Scene::moveVehicleOriginsCoordinateSystem(const float _x, const float _y, const float _z){
  vehicleParts2[COORDINATES]->changePosition(_x, _y, _z);
}

unsigned int Scene::addFixedFrame(const float _x, const float _y, const float _z,
                                  const float _pitch, const float _yaw, const float _roll){
  m2_program.bind();
  ThreeDimentionalModelLoaderNormals *frame = new ThreeDimentionalModelLoaderNormals(COORDINATES, &m2_program);
  frame->changeRotation(_pitch, _yaw, _roll);
  frame->changePosition(_x, _y, _z);
  frameCoordinates.append(frame);
  m2_program.release();

  return frameCoordinates.size() - 1;
}

bool Scene::deleteFixedFrame(const unsigned int _id){
  bool ok = false;
  if(frameCoordinates.size() > _id)
    if(frameCoordinates[_id] != nullptr){
      delete frameCoordinates[_id];
      ok = true;
    }
  return ok;
}

void Scene::purgeFixedFrames(){
  for(int i = 0; i < frameCoordinates.size(); i++)
    if(frameCoordinates[i] != nullptr)
      delete frameCoordinates[i];
  frameCoordinates.clear();
}
