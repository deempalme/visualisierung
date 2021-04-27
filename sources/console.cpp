#include "headers/console.h"
#include "ui_console.h"

bool *NewCamera::onCamera;

Console::Console(QWidget *parent) :
  QMainWindow(parent),
  whichCompany(PEM),
  onCloud(true),
  onColorize(true),
  onMovables(true),
  onVehicle(true),
  onSkybox(true),
  onChassis(true),
  onChassisRebounding(true),
  onAxesRotation(true),
  onAxes(true),
  onGround(false),
  onStreets(false),
  onSignalization(false),
  onTrajectory(true),
  onAxesTexture(false),
  onBox(false),
  onFrameCoordinates(true),
  isStaticCamera(false),
  isCameraLocked(true),
  onMenu(true),
  onCamera(true),
  onLabels(true),
  onGUI(true),
  controls(nullptr),
  ui(new Ui::Console),
  onOptions(false),
  createdControl(false),
  connected(false),
  previousOnGUI(onGUI),
  vehiclePositionX(nullptr),
  vehiclePositionY(nullptr),
  vehiclePositionZ(nullptr),
  vehicle_latitude(nullptr),
  vehicle_longitude(nullptr),
  vehiclePitch(nullptr),
  vehicleYaw(nullptr),
  vehicleRoll(nullptr),
  vehicleSpeed(nullptr),
  vehicleSteeringAngle(nullptr),
  vehicleForwardAcceleration(nullptr),
  vehicleLeftwardAcceleration(nullptr),
  vehicleUpwardAcceleration(nullptr),
  frameLatitude(nullptr),
  frameLongitude(nullptr),
  frameAltitude(nullptr),
  framePitch(nullptr),
  frameYaw(nullptr),
  frameRoll(nullptr),
  previousFrameLatitude(0),
  previousFrameLongitude(0),
  lastFixedPitch(0),
  lastFixedYaw(0),
  lastFixedRoll(0),
  bckPer(100, 100),
  bckCamera(1242, 375),
  bckIcon(":/images/i_install2"),
  bckIcon2(24, 30),
  iconAngle(0),
  previousDataset(0),
  gradient(100, 0, 0, 100),
  show_alternative(nullptr),
  previous_alternative(false)
{
  database = new databaseConnection();
  ui->setupUi(this);

  ui->scenary->whichCompany = &whichCompany;
  ui->scenary->onCloud = &onCloud;
  ui->scenary->onColorize = &onColorize;
  ui->scenary->onMovables = &onMovables;
  ui->scenary->onVehicle = &onVehicle;
  ui->scenary->onSkybox = &onSkybox;
  ui->scenary->onChassis = &onChassis;
  ui->scenary->onChassisRebounding = &onChassisRebounding;
  ui->scenary->onAxesRotation = &onAxesRotation;
  ui->scenary->onAxes = &onAxes;
  ui->scenary->onGround = &onGround;
  ui->scenary->onStreets = &onStreets;
  ui->scenary->onSignalization = &onSignalization;
  ui->scenary->onTrajectory = &onTrajectory;
  ui->scenary->onAxesTexture = &onAxesTexture;
  ui->scenary->onBox = &onBox;
  ui->scenary->onFrameCoordinates = &onFrameCoordinates;
  ui->scenary->isStaticCamera = &isStaticCamera;
  ui->scenary->isCameraLocked = &isCameraLocked;
  ui->scenary->isLaserActive = &laserActives;
  ui->scenary->isObstacleActive = &objectActives;
  ui->scenary->isCylinderActive = &cylinderActives;
  ui->scenary->isGroundActive = &groundActives;
  ui->scenary->isTrajectoryActive = &trajectoryActives;

  NewCamera::onCamera = &onCamera;

  //set fonts reads external typographies and applies to the GUI elements
  setFonts();
  //set the options menu
  setOptions();

  host = ui->f_host;
  scenaryRotationY = &ui->scenary->scenaryRotationY;

  ui->scenary->vehicleTransformationMatrix = &vehicleTransformationMatrix;
  ui->scenary->fixedFrameModel = &fixedFrameModel;
  ui->scenary->navigationTransformationMatrix = &navigationTransformationMatrix;
  ui->scenary->plus90VehicleTransformationMatrix = &plus90VehicleTransformationMatrix;
  ui->scenary->fixedFrameTransformationMatrix = &fixedFrameTransformationMatrix;

  installator = new InstallMaps(this, database->getDB());
  ui->scenary->isInstalling = &installator->occupied;

  f_extra = new QFrame();
  f_extra->setStyleSheet(".QFrame{ border:none; background:none; }");
  f_extra->resize(180, 200);

  s_extra = new QScrollArea(ui->cen_opt);
  s_extra->move(1010, 55);
  s_extra->resize(200,345);
  s_extra->setWidget(f_extra);
  s_extra->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  s_extra->setFrameShape(QFrame::NoFrame);
  s_extra->setFrameShadow(QFrame::Plain);
  s_extra->setLineWidth(0);

#ifdef ROS_DATA
  ROStimer = new QTimer();
  connect(ROStimer, SIGNAL(timeout()), this, SLOT(updateROSdata()));
#endif
}

Console::~Console(){
  for(int i = 0; i < fonts.size(); i++)
    delete fonts[i];
  fonts.clear();

  for(int i = 0; i < simpleElements.size(); i++)
    if(simpleElements[i] != nullptr)
      delete simpleElements[i];

  for(int i = 0; i < speedos.size(); i++)
    if(speedos[i] != nullptr)
      delete speedos[i];

  for(int i = 0; i < gearboxes.size(); i++)
    if(gearboxes[i] != nullptr)
      delete gearboxes[i];

  for(int i = 0; i < textlists.size(); i++)
    if(textlists[i] != nullptr)
      delete textlists[i];

  for(int i = 0; i < iconlists.size(); i++)
    if(iconlists[i] != nullptr)
      delete iconlists[i];

  for(int i = 0; i < messagelists.size(); i++)
    if(messagelists[i] != nullptr)
      delete messagelists[i];

  for(int i = 0; i < signs.size(); i++)
    if(signs[i] != nullptr)
      delete signs[i];

  for(int i = 0; i < clocks.size(); i++)
    if(clocks[i] != nullptr)
      delete clocks[i];

  for(int i = 0; i < compasses.size(); i++)
    if(compasses[i] != nullptr)
      delete compasses[i];

  for(int i = 0; i < cameras.size(); i++)
    if(cameras[i] != nullptr)
      delete cameras[i];

  for(int i = 0; i < labels.size(); i++)
    if(labels[i] != nullptr)
      delete labels[i];

  if(createdControl)
    delete controls;

  for(auto button : laserButtons)
    if(button != nullptr)
      delete button;

  for(auto button : objectButtons)
    if(button != nullptr)
      delete button;

  for(auto button : cylinderButtons)
    if(button != nullptr)
      delete button;

  delete installator;
  delete database;
  delete f_extra;
  delete s_extra;
  delete ui;
}

const NewControls *const Console::Controls(){
  return controls;
}

const NewCamera *const Console::Cameras(unsigned int _ID){
  if(cameras.size() > _ID)
    if(cameras[_ID] != nullptr)
      return cameras[_ID];
  else
    return nullptr;
}

const QMatrix4x4 *const Console::VehicleTransformationMatrix(){
  return &vehicleTransformationMatrix;
}

const QMatrix4x4 *const Console::NavigationFrameTransformationMatrix(){
  return &navigationTransformationMatrix;
}

const QMatrix4x4 *const Console::Plus90VehicleFrameTransformationMatrix(){
  return &plus90VehicleTransformationMatrix;
}

const QMatrix4x4 *const Console::FixedFrameTransformationMatrix(){
  return &fixedFrameTransformationMatrix;
}

const QMatrix4x4 *const Console::FixedPositionTransformationMatrix(){
  return &fixedPositionTransformationMatrix;
}

void Console::resizeEvent(QResizeEvent *event){
  setInterface();
}

void Console::setInterface(){
  ui->f_host->resize(ui->centralWidget->size());
  ui->f_views->move(0, this->height()/2 - 230);
  ui->f_options->resize(this->width(), this->height());
  if(this->height() > 400)
    ui->f_options_contents->resize(this->width(), this->height());
  int theight = 0;
  if(this->height() > ui->cen_opt->height())
    theight = (this->height() - ui->cen_opt->height())/2 - 60;
  ui->cen_opt->move((this->width() - ui->cen_opt->width())/2 + 20, theight);
  if(!onOptions)
    ui->f_options->move(0, this->height() + 20);
  ui->f_labels->resize(this->width(), this->height());
  for(int i = 0; i < labels.size(); i++)
    if(labels[i] != nullptr)
      labels[i]->updateLabels();
}

void Console::setFonts(){
  //setting the custom fonts
  iQuantico = QFontDatabase::addApplicationFont(":/fonts/quantico");
  iUnica = QFontDatabase::addApplicationFont(":/fonts/unica");
  iMontserrat = QFontDatabase::addApplicationFont(":/fonts/montserrat");
  iRoboto = QFontDatabase::addApplicationFont(":/fonts/roboto");

  fonts << new QFont("Quantico", 9, QFont::Bold);
  fonts[0]->setStyleStrategy(QFont::PreferAntialias);
  fonts << new QFont("Quantico", 7);
  fonts[1]->setStyleStrategy(QFont::PreferAntialias);
  fonts << new QFont("Unica One", 13, QFont::Bold);
  fonts[2]->setStyleStrategy(QFont::PreferAntialias);
  fonts << new QFont("Quantico", 7, QFont::Bold);
  fonts[3]->setStyleStrategy(QFont::PreferAntialias);
  fonts << new QFont("Unica One", 9, QFont::Bold);
  fonts[4]->setStyleStrategy(QFont::PreferAntialias);
  fonts << new QFont("Quantico", 8, QFont::Bold);
  fonts[5]->setStyleStrategy(QFont::PreferAntialias);
  fonts << new QFont("Montserrat", 8);
  fonts[6]->setStyleStrategy(QFont::PreferAntialias);
  fonts << new QFont("Roboto", 15, QFont::Bold);
  fonts[7]->setStyleStrategy(QFont::PreferAntialias);
  fonts << new QFont("Unica One", 17);
  fonts[8]->setStyleStrategy(QFont::PreferAntialias);
  fonts << new QFont("Unica One", 11, QFont::Bold);
  fonts[9]->setStyleStrategy(QFont::PreferAntialias);
  fonts << new QFont("Unica One", 10, QFont::Bold);
  fonts[10]->setStyleStrategy(QFont::PreferAntialias);
  fonts << new QFont("Unica One", 24, QFont::Bold);
  fonts[11]->setStyleStrategy(QFont::PreferAntialias);
  fonts << new QFont("Montserrat", 7, QFont::Bold);
  fonts[12]->setStyleStrategy(QFont::PreferAntialias);
}

void Console::setOptions(){
  ui->f_options->move(0, this->height() + 20);

  onCss = "QPushButton{"
       "background:#000 url(:/images/on_off) right %1 no-repeat;"
       "border-radius:4px;"
       "color:#fff;"
       "padding:0 0 0 10px;"
       "text-align:left;"
      "}"
      "QPushButton:pressed{ background:#fff url(:/images/on_off) right %1 no-repeat; color:#000; }"
      "QPushButton:click{ background:#fff url(:/images/on_off) right %1 no-repeat; color:#000; }"
      "QPushButton:hover:!pressed{ background:#8ec1e7 url(:/images/on_off) right %1 no-repeat; color:#000; }";

  const QString on2Css = "QPushButton{ background:#000 url(:/images/i_maps) right center no-repeat; border-radius:4px; color:#fff; padding:0 0 0 10px; text-align:left; }"
      "QPushButton:pressed{ background:#fff url(:/images/i_maps) right center no-repeat; color:#000; }"
      "QPushButton:click{ background:#fff url(:/images/i_maps) right center no-repeat; color:#000; }"
      "QPushButton:hover:!pressed{ background:#8ec1e7 url(:/images/i_maps) right bottom no-repeat; color:#000; }";

  btnCss = "QPushButton{ background:url(:/images/b_menu%2) left %1 no-repeat; }"
      "QPushButton:pressed{ background:rgba(0,0,0,0) url(:/images/b_menu%2) right %1 no-repeat; }"
      "QPushButton:click{ background:rgba(0,0,0,0) url(:/images/b_menu%2) right %1 no-repeat; }"
      "QPushButton:hover:!pressed{ background:url(:/images/b_menu%2) center %1 no-repeat; }";

  insCss = "color:%2; background:url(:/images/i_install) left %1 no-repeat; padding-left:28px;";

  ui->lb_laser->setFont(*fonts[5]);
  ui->lb_laser_2->setFont(*fonts[5]);
  ui->lb_maps->setFont(*fonts[5]);
  ui->lb_movables->setFont(*fonts[5]);
  ui->lb_skybox->setFont(*fonts[5]);
  ui->lb_vehicle->setFont(*fonts[5]);
  ui->lb_title->setFont(*fonts[11]);
  ui->b_lsc->setFont(*fonts[10]);
  ui->b_ls_color->setFont(*fonts[10]);
  ui->b_movables->setFont(*fonts[10]);
  ui->b_vehicle->setFont(*fonts[10]);
  ui->b_streets->setFont(*fonts[10]);
  ui->b_signals->setFont(*fonts[10]);
  ui->b_trajectory->setFont(*fonts[10]);
  ui->b_ground->setFont(*fonts[10]);
  ui->b_skybox->setFont(*fonts[10]);
  ui->b_chassis->setFont(*fonts[10]);
  ui->b_rebound->setFont(*fonts[10]);
  ui->b_axes->setFont(*fonts[10]);
  ui->b_texture->setFont(*fonts[10]);
  ui->b_rotation->setFont(*fonts[10]);
  ui->b_box->setFont(*fonts[10]);
  ui->lb_controls->setFont(*fonts[5]);
  ui->lb_frame->setFont(*fonts[5]);
  ui->b_frame->setFont(*fonts[10]);
  ui->lb_install->setFont(*fonts[5]);
  ui->b_install->setFont(*fonts[10]);
  ui->lb_installmaps->setFont(*fonts[5]);
  ui->lb_installmaps_2->setFont(*fonts[5]);
  ui->lb_percentage->setFont(*fonts[11]);
  ui->lb_errors->setFont(*fonts[5]);
  ui->lb_cameras->setFont(*fonts[5]);
  ui->b_cameras->setFont(*fonts[10]);
  ui->b_gui->setFont(*fonts[10]);
  ui->lb_gui->setFont(*fonts[5]);
  ui->b_tags->setFont(*fonts[10]);

  checkOptions();
  ui->b_install->setStyleSheet(on2Css);

  ui->f_options_contents->setStyleSheet("QWidget{ background:rgba(0,0,0,0); }");
  ui->lb_installmaps_2->setStyleSheet(insCss.arg("center").arg("#FF4040"));
  ui->lb_errors->hide();
  ui->bck_icon->hide();
  ui->lb_installmaps->hide();
  ui->lb_installmaps_2->hide();
  ui->lb_percentage->hide();
  ui->bck_percentage->hide();

  gradient.setColorAt(0, QColor(43, 209, 199));
  gradient.setColorAt(0.5, QColor(49, 253, 223));
  gradient.setColorAt(1, QColor(38, 172, 180));

  onMenu = !onMenu;
  on_b_hop_released();
}

void Console::checkOptions(){
  if(!onCloud) ui->b_lsc->setStyleSheet(onCss.arg("bottom"));
  else ui->b_lsc->setStyleSheet(onCss.arg("top"));
  if(!onColorize) ui->b_ls_color->setStyleSheet(onCss.arg("bottom"));
  else ui->b_ls_color->setStyleSheet(onCss.arg("top"));
  if(!onMovables) ui->b_movables->setStyleSheet(onCss.arg("bottom"));
  else ui->b_movables->setStyleSheet(onCss.arg("top"));
  if(!onVehicle) ui->b_vehicle->setStyleSheet(onCss.arg("bottom"));
  else ui->b_vehicle->setStyleSheet(onCss.arg("top"));
  if(!onStreets) ui->b_streets->setStyleSheet(onCss.arg("bottom"));
  else ui->b_streets->setStyleSheet(onCss.arg("top"));
  if(!onSignalization) ui->b_signals->setStyleSheet(onCss.arg("bottom"));
  else ui->b_signals->setStyleSheet(onCss.arg("top"));
  if(!onTrajectory) ui->b_trajectory->setStyleSheet(onCss.arg("bottom"));
  else ui->b_trajectory->setStyleSheet(onCss.arg("top"));
  if(!onGround) ui->b_ground->setStyleSheet(onCss.arg("bottom"));
  else ui->b_ground->setStyleSheet(onCss.arg("top"));
  if(!onSkybox) ui->b_skybox->setStyleSheet(onCss.arg("bottom"));
  else ui->b_skybox->setStyleSheet(onCss.arg("top"));
  if(!onChassis) ui->b_chassis->setStyleSheet(onCss.arg("bottom"));
  else ui->b_chassis->setStyleSheet(onCss.arg("top"));
  if(!onChassisRebounding) ui->b_rebound->setStyleSheet(onCss.arg("bottom"));
  else ui->b_rebound->setStyleSheet(onCss.arg("top"));
  if(!onAxesTexture) ui->b_texture->setStyleSheet(onCss.arg("bottom"));
  else ui->b_texture->setStyleSheet(onCss.arg("top"));
  if(!onAxes) ui->b_axes->setStyleSheet(onCss.arg("bottom"));
  else ui->b_axes->setStyleSheet(onCss.arg("top"));
  if(!onAxesRotation) ui->b_rotation->setStyleSheet(onCss.arg("bottom"));
  else ui->b_rotation->setStyleSheet(onCss.arg("top"));
  if(!onBox) ui->b_box->setStyleSheet(onCss.arg("bottom"));
  else ui->b_box->setStyleSheet(onCss.arg("top"));
  if(!onFrameCoordinates) ui->b_frame->setStyleSheet(onCss.arg("bottom"));
  else ui->b_frame->setStyleSheet(onCss.arg("top"));
  if(!isCameraLocked) ui->b_svw->setStyleSheet(btnCss.arg("bottom").arg("2"));
  else ui->b_svw->setStyleSheet(btnCss.arg("top").arg("2"));
  if(isStaticCamera) ui->b_fvw->setStyleSheet(btnCss.arg("bottom").arg("3"));
  else ui->b_fvw->setStyleSheet(btnCss.arg("top").arg("3"));
  if(!onMenu) ui->b_hop->setStyleSheet(btnCss.arg("bottom").arg("4"));
  if(!onCamera) ui->b_cameras->setStyleSheet(onCss.arg("bottom"));
  else ui->b_cameras->setStyleSheet(onCss.arg("top"));
  if(!onLabels) ui->b_tags->setStyleSheet(onCss.arg("bottom"));
  else ui->b_tags->setStyleSheet(onCss.arg("top"));
  if(!onGUI) ui->b_gui->setStyleSheet(onCss.arg("bottom"));
  else ui->b_gui->setStyleSheet(onCss.arg("top"));
}

void Console::hideShowCameras(){
  for(int i = 0; i < cameras.size(); i++)
    if(cameras.size() > i)
      if(cameras[i] != nullptr)
      if(onCamera){
        cameras[i]->show();
        cameras[i]->update();
      }else
        cameras[i]->hide();
}

void Console::hideShowGUIElements(){
  for(int i = 0; i < simpleElements.size(); i++)
    if(simpleElements.size() > i)
      if(simpleElements[i] != nullptr)
        if(onGUI)
          simpleElements[i]->show();
        else
          simpleElements[i]->hide();
  for(int i = 0; i < speedos.size(); i++)
    if(speedos.size() > i)
      if(speedos[i] != nullptr)
        if(onGUI)
          speedos[i]->show();
        else
          speedos[i]->hide();
  for(int i = 0; i < gearboxes.size(); i++)
    if(gearboxes.size() > i)
      if(gearboxes[i] != nullptr)
        if(onGUI)
          gearboxes[i]->show();
        else
          gearboxes[i]->hide();
  for(int i = 0; i < textlists.size(); i++)
    if(textlists.size() > i)
      if(textlists[i] != nullptr)
        if(onGUI)
          textlists[i]->show();
        else
          textlists[i]->hide();
  for(int i = 0; i < iconlists.size(); i++)
    if(iconlists.size() > i)
      if(iconlists[i] != nullptr)
        if(onGUI)
          iconlists[i]->show();
        else
          iconlists[i]->hide();
  for(int i = 0; i < messagelists.size(); i++)
    if(messagelists.size() > i)
      if(messagelists[i] != nullptr)
        if(onGUI)
          messagelists[i]->show();
        else
          messagelists[i]->hide();
  for(int i = 0; i < signs.size(); i++)
    if(signs.size() > i)
      if(signs[i] != nullptr)
        if(onGUI)
          signs[i]->show();
        else
          signs[i]->hide();
}

void Console::raiseF(){
  ui->f_options->raise();
  ui->f_views->raise();
}

void Console::createNewLaserButton(const unsigned int _id, const QString _name, const bool _visible){
  OptionButtons *newButton = new OptionButtons(_name, f_extra, _id);
  newButton->setFont(*fonts[10]);
  if(_visible) newButton->setStyleSheet(onCss.arg("top"));
  else newButton->setStyleSheet(onCss.arg("bottom"));
  connect(newButton, SIGNAL(buttonReleased(uint)), this, SLOT(laserButtonReleased(uint)));
  laserButtons[_id] = newButton;
  laserActives[_id] = _visible;
  rearrangeButtons();
}

void Console::deleteLaserButton(const unsigned int _id){
  if(laserButtons.contains(_id))
    if(laserButtons[_id] != nullptr){
      delete laserButtons[_id];
      laserButtons.remove(_id);
    }
  if(laserActives.contains(_id))
    laserActives.remove(_id);

  rearrangeButtons();
}

void Console::purgeLaserButtons(){
  for(auto button : laserButtons){
    if(button != nullptr)
      delete button;
  }
  laserButtons.clear();
  laserActives.clear();

  rearrangeButtons();
}

void Console::createNewObstacleButton(const unsigned int _id, const QString _name, const bool _visible){
  OptionButtons *newButton = new OptionButtons(_name, f_extra, _id);
  newButton->setFont(*fonts[10]);
  if(_visible) newButton->setStyleSheet(onCss.arg("top"));
  else newButton->setStyleSheet(onCss.arg("bottom"));
  connect(newButton, SIGNAL(buttonReleased(uint)), this, SLOT(obstacleButtonReleased(uint)));
  objectButtons[_id] = newButton;
  objectActives[_id] = _visible;

  rearrangeButtons();
}

void Console::deleteObstacleButton(const unsigned int _id){
  if(objectButtons.contains(_id))
    if(objectButtons[_id] != nullptr){
      delete objectButtons[_id];
      objectButtons.remove(_id);
    }
  if(objectActives.contains(_id))
    objectActives.remove(_id);

  rearrangeButtons();
}

void Console::purgeObstacleButtons(){
  for(auto button : objectButtons){
    if(button != nullptr)
      delete button;
  }
  objectButtons.clear();
  objectActives.clear();

  rearrangeButtons();
}

void Console::createNewCylinderButton(const unsigned int _id, const QString _name, const bool _visible){
  OptionButtons *newButton = new OptionButtons(_name, f_extra, _id);
  newButton->setFont(*fonts[10]);
  if(_visible) newButton->setStyleSheet(onCss.arg("top"));
  else newButton->setStyleSheet(onCss.arg("bottom"));
  connect(newButton, SIGNAL(buttonReleased(uint)), this, SLOT(cylinderButtonReleased(uint)));
  cylinderButtons[_id] = newButton;
  cylinderActives[_id] = _visible;

  rearrangeButtons();
}

void Console::deleteCylinderButton(const unsigned int _id){
  if(cylinderButtons.contains(_id))
    if(cylinderButtons[_id] != nullptr){
      delete cylinderButtons[_id];
      cylinderButtons.remove(_id);
    }
  if(cylinderActives.contains(_id))
    cylinderActives.remove(_id);

  rearrangeButtons();
}

void Console::purgeCylinderButtons(){
  for(auto button : cylinderButtons){
    if(button != nullptr)
      delete button;
  }
  cylinderButtons.clear();
  cylinderActives.clear();

  rearrangeButtons();
}

void Console::createNewLabelsButton(const unsigned int _id, const QString _name, const bool _visible){
  OptionButtons *newButton = new OptionButtons(_name, f_extra, _id);
  newButton->setFont(*fonts[10]);
  if(_visible) newButton->setStyleSheet(onCss.arg("top"));
  else newButton->setStyleSheet(onCss.arg("bottom"));
  connect(newButton, SIGNAL(buttonReleased(uint)), this, SLOT(labelButtonReleased(uint)));
  labelButtons[_id] = newButton;
  labelActives[_id] = _visible;

  rearrangeButtons();
}

void Console::deleteLabelsButton(const unsigned int _id){
  if(labelButtons.contains(_id))
    if(labelButtons[_id] != nullptr){
      delete labelButtons[_id];
      labelButtons.remove(_id);
    }
  if(labelActives.contains(_id))
    labelActives.remove(_id);

  rearrangeButtons();
}

void Console::purgeLabelsButtons(){
  for(auto button : labelButtons){
    if(button != nullptr)
      delete button;
  }
  labelButtons.clear();
  labelActives.clear();

  rearrangeButtons();
}

void Console::createNewGroundButton(const unsigned int _id, const QString _name, const bool _visible){
  OptionButtons *newButton = new OptionButtons(_name, f_extra, _id);
  newButton->setFont(*fonts[10]);
  if(_visible) newButton->setStyleSheet(onCss.arg("top"));
  else newButton->setStyleSheet(onCss.arg("bottom"));
  connect(newButton, SIGNAL(buttonReleased(uint)), this, SLOT(groundButtonReleased(uint)));
  groundButtons[_id] = newButton;
  groundActives[_id] = _visible;

  rearrangeButtons();
}

void Console::deleteGroundButton(const unsigned int _id){
  if(groundButtons.contains(_id))
    if(groundButtons[_id] != nullptr){
      delete groundButtons[_id];
      groundButtons.remove(_id);
    }
  if(groundActives.contains(_id))
    groundActives.remove(_id);

  rearrangeButtons();
}

void Console::purgeGroundButtons(){
  for(auto button : groundButtons){
    if(button != nullptr)
      delete button;
  }
  groundButtons.clear();
  groundActives.clear();

  rearrangeButtons();
}

void Console::createNewTrajectoryButton(const unsigned int _id, const QString _name, const bool _visible){
  OptionButtons *newButton = new OptionButtons(_name, f_extra, _id);
  newButton->setFont(*fonts[10]);
  if(_visible) newButton->setStyleSheet(onCss.arg("top"));
  else newButton->setStyleSheet(onCss.arg("bottom"));
  connect(newButton, SIGNAL(buttonReleased(uint)), this, SLOT(trajectoryButtonReleased(uint)));
  trajectoryButtons[_id] = newButton;
  trajectoryActives[_id] = _visible;

  rearrangeButtons();
}

void Console::deleteTrajectoryButton(const unsigned int _id){
  if(trajectoryButtons.contains(_id))
    if(trajectoryButtons[_id] != nullptr){
      delete trajectoryButtons[_id];
      trajectoryButtons.remove(_id);
    }
  if(groundActives.contains(_id))
    trajectoryActives.remove(_id);

  rearrangeButtons();
}

void Console::purgeTrajectoryButtons(){
  for(auto button : trajectoryButtons){
    if(button != nullptr)
      delete button;
  }
  trajectoryButtons.clear();
  trajectoryActives.clear();

  rearrangeButtons();
}

void Console::rearrangeButtons(){
  uint i = 0, l = 0, o = 0, c = 0, b = 0, g = 0, t = 0;
  for(auto button : laserButtons){
    button->move(10, 35 * i++);
    if(l < 9){
      button->setText("C" + QString::number(l + 1) + " : " + button->name);
      button->setShortcut(QKeySequence("Ctrl+"+QString::number(l + 1)));
    }else if(l == 9){
      button->setText("C0 : " + button->name);
      button->setShortcut(QKeySequence("Ctrl+0"));
    }else{
      button->setText(button->name);
      button->setShortcut(QKeySequence());
    }
    l++;
  }
  for(auto button : objectButtons){
    button->move(10, 35 * i++);
    if(o < 9){
      button->setText("A" + QString::number(o + 1) + " : " + button->name);
      button->setShortcut(QKeySequence("Alt+"+QString::number(o + 1)));
    }else if(o == 9){
      button->setText("A0 : " + button->name);
      button->setShortcut(QKeySequence("Alt+0"));
    }else{
      button->setText(button->name);
      button->setShortcut(QKeySequence());
    }
    o++;
  }
  for(auto button : cylinderButtons){
    button->move(10, 35 * i++);
    if(c < 9){
      button->setText("CA" + QString::number(c + 1) + " : " + button->name);
      button->setShortcut(QKeySequence("Ctrl+Alt+"+QString::number(c + 1)));
    }else if(c == 9){
      button->setText("CA0 : " + button->name);
      button->setShortcut(QKeySequence("Ctrl+Alt+0"));
    }else{
      button->setText(button->name);
      button->setShortcut(QKeySequence());
    }
    c++;
  }
  for(auto button : labelButtons){
    button->move(10, 35 * i++);
    if(b < 9){
      button->setText("S" + QString::number(b + 1) + " : " + button->name);
      button->setShortcut(QKeySequence("Shift+"+QString::number(b + 1)));
    }else if(b == 9){
      button->setText("S0 : " + button->name);
      button->setShortcut(QKeySequence("Shift+0"));
    }else{
      button->setText(button->name);
      button->setShortcut(QKeySequence());
    }
    b++;
  }
  for(auto button : groundButtons){
    button->move(10, 35 * i++);
    if(g < 9){
      button->setText("CS" + QString::number(g + 1) + " : " + button->name);
      button->setShortcut(QKeySequence("Ctrl+Shift+"+QString::number(g + 1)));
    }else if(g == 9){
      button->setText("CS0 : " + button->name);
      button->setShortcut(QKeySequence("Ctrl+Shift+0"));
    }else{
      button->setText(button->name);
      button->setShortcut(QKeySequence());
    }
    g++;
  }
  for(auto button : trajectoryButtons){
    button->move(10, 35 * i++);
    if(t < 9){
      button->setText("AS" + QString::number(t + 1) + " : " + button->name);
      button->setShortcut(QKeySequence("Alt+Shift+"+QString::number(t + 1)));
    }else if(t == 9){
      button->setText("AS0 : " + button->name);
      button->setShortcut(QKeySequence("Alt+Shift+0"));
    }else{
      button->setText(button->name);
      button->setShortcut(QKeySequence());
    }
    t++;
  }
  f_extra->resize(180, i * 35);
}

void Console::laserButtonReleased(unsigned int _id){
  bool state = !laserActives[_id];
  laserActives[_id] = state;
  if(state)
    laserButtons[_id]->setStyleSheet(onCss.arg("top"));
  else
    laserButtons[_id]->setStyleSheet(onCss.arg("bottom"));
  ui->scenary->update();
}

void Console::obstacleButtonReleased(unsigned int _id){
  bool state = !objectActives[_id];
  objectActives[_id] = state;
  if(state)
    objectButtons[_id]->setStyleSheet(onCss.arg("top"));
  else
    objectButtons[_id]->setStyleSheet(onCss.arg("bottom"));
  ui->scenary->update();
}

void Console::cylinderButtonReleased(unsigned int _id){
  bool state = !cylinderActives[_id];
  cylinderActives[_id] = state;
  if(state)
    cylinderButtons[_id]->setStyleSheet(onCss.arg("top"));
  else
    cylinderButtons[_id]->setStyleSheet(onCss.arg("bottom"));
  ui->scenary->update();
}

void Console::labelButtonReleased(unsigned int _id){
  bool state = !labelActives[_id];
  labelActives[_id] = state;
  if(state)
    labelButtons[_id]->setStyleSheet(onCss.arg("top"));
  else
    labelButtons[_id]->setStyleSheet(onCss.arg("bottom"));
  if(labels[_id] != nullptr)
    labels[_id]->updateLabels();
}

void Console::groundButtonReleased(unsigned int _id){
  bool state = !groundActives[_id];
  groundActives[_id] = state;
  if(state)
    groundButtons[_id]->setStyleSheet(onCss.arg("top"));
  else
    groundButtons[_id]->setStyleSheet(onCss.arg("bottom"));
  ui->scenary->update();
}

void Console::trajectoryButtonReleased(unsigned int _id){
  bool state = !trajectoryActives[_id];
  trajectoryActives[_id] = state;
  if(state)
    trajectoryButtons[_id]->setStyleSheet(onCss.arg("top"));
  else
    trajectoryButtons[_id]->setStyleSheet(onCss.arg("bottom"));
  ui->scenary->update();
}

#ifdef ROS_DATA
void Console::closeEvent(QCloseEvent *event){
  ros::shutdown();
  event->accept();
}
#endif

// --------------------------------------------------- Lasers --------------------------------------------------- //

unsigned int Console::addLaser(const vector<LaserCloudData> *const _laserCloud, const QString _name, const bool _visible,
                               const QMatrix4x4 *const _transformationMatrix, const float _pointSize, const bool _gradient,
                               const bool _colorized, const bool _isIntensityHigherThanOne){
  uint id = ui->scenary->addLaser(_laserCloud, _transformationMatrix, _pointSize, _gradient, _colorized, _isIntensityHigherThanOne);
  createNewLaserButton(id, _name, _visible);
  return id;
}

unsigned int Console::addLaser(const vector<LaserCloudData> *const _laserCloud, const QString _name, const bool _visible,
                               const QMatrix4x4 *const _transformationMatrix, const float _pointSize, const float _R, const float _G,
                               const float _B, const bool _gradient, const bool _colorized, const bool _isIntensityHigherThanOne){
  float _solidColor[3] = {_R, _G, _B};
  uint id = ui->scenary->addLaser(_laserCloud, _transformationMatrix, _pointSize, _solidColor, _gradient, _colorized, _isIntensityHigherThanOne);
  createNewLaserButton(id, _name, _visible);
  return id;
}

unsigned int Console::addLaser(const vector<LaserCloudData> *const _laserCloud, const QString _name, const bool _visible,
                               const QMatrix4x4 *const _transformationMatrix, const float _pointSize, vector<array<float, 3> > _palette,
                               const bool _gradient, const bool _colorized, const bool _isIntensityHigherThanOne){
  uint id = ui->scenary->addLaser(_laserCloud, _transformationMatrix, _pointSize, _palette, _gradient, _colorized, _isIntensityHigherThanOne);
  createNewLaserButton(id, _name, _visible);
  return id;
}

unsigned int Console::addLaser(const vector<pointRGB> *const _laserCloud, const QString _name, const bool _visible,
                               const QMatrix4x4 *const _transformationMatrix, const float _pointSize, const bool _gradient,
                               const bool _colorized, const bool _isIntensityHigherThanOne){
  uint id = ui->scenary->addLaser(_laserCloud, _transformationMatrix, _pointSize, _gradient, _colorized, _isIntensityHigherThanOne);
  createNewLaserButton(id, _name, _visible);
  return id;
}

unsigned int Console::addLaser(const vector<pointRGB> *const _laserCloud, const QString _name, const bool _visible,
                               const QMatrix4x4 *const _transformationMatrix, const float _pointSize, const float _R,
                               const float _G, const float _B, const bool _gradient, const bool _colorized,
                               const bool _isIntensityHigherThanOne){
  float _solidColor[3] = {_R, _G, _B};
  uint id = ui->scenary->addLaser(_laserCloud, _transformationMatrix, _pointSize, _solidColor, _gradient, _colorized, _isIntensityHigherThanOne);
  createNewLaserButton(id, _name, _visible);
  return id;
}

unsigned int Console::addLaser(const vector<pointRGB> *const _laserCloud, const QString _name, const bool _visible,
                               const QMatrix4x4 *const _transformationMatrix, const float _pointSize, vector<array<float, 3> > _palette,
                               const bool _gradient, const bool _colorized, const bool _isIntensityHigherThanOne){
  uint id = ui->scenary->addLaser(_laserCloud, _transformationMatrix, _pointSize, _palette, _gradient, _colorized, _isIntensityHigherThanOne);
  createNewLaserButton(id, _name, _visible);
  return id;
}

unsigned int Console::addLaser(const vector<pointDAI> *const _laserCloud, const QString _name, const float _pointSize,
                               const float _angularResolutionDEGREES,
                               const float _verticalAngleBetweenLayersDEGREES, const bool _visible,
                               const QMatrix4x4 *const _transformationMatrix, const bool _gradient,
                               const bool _colorized, const bool _isIntensityHigherThanOne){
  uint id = ui->scenary->addLaser(_laserCloud, _pointSize, _angularResolutionDEGREES,
                                  _verticalAngleBetweenLayersDEGREES, _transformationMatrix,
                                  _gradient, _colorized, _isIntensityHigherThanOne);
  createNewLaserButton(id, _name, _visible);
  return id;
}

unsigned int Console::addLaser(const vector<pointDAI> *const _laserCloud, const QString _name, const float _pointSize,
                               const float _angularResolutionDEGREES,
                               const float _verticalAngleBetweenLayersDEGREES, const bool _visible,
                               const QMatrix4x4 *const _transformationMatrix, const float _R,
                               const float _G, const float _B, const bool _gradient,
                               const bool _colorized, const bool _isIntensityHigherThanOne){
  float _solidColor[3] = {_R, _G, _B};
  uint id = ui->scenary->addLaser(_laserCloud, _pointSize, _angularResolutionDEGREES,
                                  _verticalAngleBetweenLayersDEGREES, _transformationMatrix,
                                  _solidColor, _gradient, _colorized, _isIntensityHigherThanOne);
  createNewLaserButton(id, _name, _visible);
  return id;
}

unsigned int Console::addLaser(const vector<pointDAI> *const _laserCloud, const QString _name, const float _pointSize,
                               const float _angularResolutionDEGREES,
                               const float _verticalAngleBetweenLayersDEGREES, const bool _visible,
                               const QMatrix4x4 *const _transformationMatrix,
                               vector<array<float, 3> > _palette, const bool _gradient,
                               const bool _colorized, const bool _isIntensityHigherThanOne){
  uint id = ui->scenary->addLaser(_laserCloud, _pointSize, _angularResolutionDEGREES,
                                  _verticalAngleBetweenLayersDEGREES, _transformationMatrix,
                                  _palette, _gradient, _colorized, _isIntensityHigherThanOne);
  createNewLaserButton(id, _name, _visible);
  return id;
}

bool Console::setLaserColorPalette(unsigned int _id, vector<array<float, 3> > _palette, const bool _gradient){
  return ui->scenary->setLaserColorPalette(_id, _palette, _gradient);
}

bool Console::setLaserSolidColor(unsigned int _id, const float _R, const float _G, const float _B, const bool _gradient){
  float _SolidColor[3] = {_R, _G, _B};
  return ui->scenary->setLaserSolidColor(_id, _SolidColor, _gradient);
}

bool Console::ignoreSolidColor(unsigned int _id, const bool _ignore){
  return ui->scenary->ignoreSolidColor(_id, _ignore);
}

bool Console::setLaserOptions(unsigned int _id, const bool _gradient, const bool _colorized,
                              const bool _ignoreSolidColor, const bool _isIntensityHigherThanOne){
  return ui->scenary->setLaserOptions(_id, _gradient, _colorized, _ignoreSolidColor, _isIntensityHigherThanOne);
}

bool Console::setLaserTransformationMatrix(unsigned int _id, const QMatrix4x4 *const _transformationMatrix){
  return ui->scenary->setLaserTransformationMatrix(_id, _transformationMatrix);
}

bool Console::translateLaserMatrix(unsigned int _id, const float _translationX, const float _translationY, const float _translationZ){
  return ui->scenary->translateLaserMatrix(_id, _translationX, _translationY, _translationZ);
}

bool Console::rotateLaserMatrix(unsigned int _id, const float _pitch, const float _yaw, const float _roll){
  return ui->scenary->rotateLaserMatrix(_id, _pitch, _yaw, _roll);
}

bool Console::updateCloud(unsigned int _id){
  return ui->scenary->updateCloud(_id);
}

bool Console::deleteLaser(unsigned int _id){
  bool ok =  ui->scenary->deleteLaser(_id);
  if(ok) deleteLaserButton(_id);
  return ok;
}

void Console::purgeLaserClouds(){
  ui->scenary->purgeLaserClouds();
  purgeLaserButtons();
}

// --------------------------------------------------- Obstacles --------------------------------------------------- //

unsigned int Console::addObstacles(const vector<Obstacle> *const _obstacles, const QString _name, const bool _visible,
                                   const QMatrix4x4 *const _transformationMatrix){
  unsigned int _id = ui->scenary->addObstacles(_obstacles, _transformationMatrix);
  createNewObstacleButton(_id, _name, _visible);
  return _id;
}

bool Console::setObstacleTransformationMatrix(unsigned int _id, QMatrix4x4 *_transformationMatrix){
  return ui->scenary->setObstacleTransformationMatrix(_id, _transformationMatrix);
}

bool Console::translateObstacleMatrix(unsigned int _id, const float _translationX, const float _translationY,
                                      const float _translationZ){
  return ui->scenary->translateObstacleMatrix(_id, _translationX, _translationY, _translationZ);
}

bool Console::rotateObstacleMatrix(unsigned int _id, const float _pitch, const float _yaw, const float _roll){
  return ui->scenary->rotateObstacleMatrix(_id, _pitch, _yaw, _roll);
}

bool Console::deleteObstacles(unsigned int _id){
  bool ok = ui->scenary->deleteObstacles(_id);
  if(ok) deleteObstacleButton(_id);
  return ok;
}

void Console::purgeObstacles(){
  ui->scenary->purgeObstacles();
  purgeObstacleButtons();
}

// --------------------------------------------------- Cylinders --------------------------------------------------- //

unsigned int Console::addCylinder(const vector<Obstacle> *const _cylinders, const QString _name, const bool _visible,
                                  const QMatrix4x4 *const _transformationMatrix){
  unsigned int _id =  ui->scenary->addCylinders(_cylinders, _transformationMatrix);
  createNewCylinderButton(_id, _name, _visible);
  return _id;
}

bool Console::setCylinderTransformationMatrix(unsigned int _id, QMatrix4x4 *_transformationMatrix){
  return ui->scenary->setCylinderTransformationMatrix(_id, _transformationMatrix);
}

bool Console::translateCylinderMatrix(unsigned int _id, const float _translationX, const float _translationY,
                                      const float _translationZ){
  return ui->scenary->translateCylinderMatrix(_id, _translationX, _translationY, _translationZ);
}

bool Console::rotateCylinderMatrix(unsigned int _id, const float _pitch, const float _yaw, const float _roll){
  return ui->scenary->rotateCylinderMatrix(_id, _pitch, _yaw, _roll);
}

bool Console::deleteCylinder(unsigned int _id){
  bool ok = ui->scenary->deleteCylinder(_id);
  if(ok) deleteCylinderButton(_id);
  return ok;
}

void Console::purgeCylinders(){
  ui->scenary->purgeCylinders();
  purgeCylinderButtons();
}

// --------------------------------------------------- Trajectory --------------------------------------------------- //

unsigned int Console::addTrajectory(const vector<vector<trajectoryData> > *const _trajectory, const QString _name,
                                    const bool _visible, const QMatrix4x4 *const _transformationMatrix){
  uint _id = ui->scenary->addTrajectory(_trajectory, _transformationMatrix);
  createNewTrajectoryButton(_id, _name, _visible);
  return _id;
}

bool Console::setTrajectoryTransformationMatrix(unsigned int _id, const QMatrix4x4 *const _transformationMatrix){
  return ui->scenary->setTrajectoryTransformationMatrix(_id, _transformationMatrix);
}

bool Console::translateTrajectoryMatrix(unsigned int _id, const float _translationX, const float _translationY,
                                        const float _translationZ){
  return ui->scenary->translateTrajectoryMatrix(_id, _translationX, _translationY, _translationZ);
}

bool Console::rotateTrajectoryMatrix(unsigned int _id, const float _pitch, const float _yaw, const float _roll){
  return ui->scenary->rotateTrajectoryMatrix(_id, _pitch, _yaw, _roll);
}

bool Console::updateTrajectory(unsigned int _id){
  return ui->scenary->updateTrajectory(_id);
}

bool Console::updateAllTrajectories(){
  return ui->scenary->updateAllTrajectories();
}

bool Console::deleteTrajectory(unsigned int _id){
  bool ok = ui->scenary->deleteTrajectory(_id);
  if(ok) deleteTrajectoryButton(_id);
}

void Console::purgeTrajectories(){
  ui->scenary->purgeTrajectories();
  purgeTrajectoryButtons();
}

// --------------------------------------------------- Ground --------------------------------------------------- //

unsigned int Console::addGround(const vector<Boxes> *const _boxes, const QString _name, const bool _visible,
                                const bool _drawLines, const QMatrix4x4 *const _transformationMatrix){
  uint _id = ui->scenary->addGround(_boxes, _drawLines, _transformationMatrix);
  createNewGroundButton(_id, _name, _visible);
  return _id;
}

bool Console::setGroundTransformationMatrix(unsigned int _id, const QMatrix4x4 *const _transformationMatrix){
  return ui->scenary->setGroundTransformationMatrix(_id, _transformationMatrix);
}

bool Console::setGroundWithLines(unsigned int _id, const bool _drawLines){
  return ui->scenary->setGroundWithLines(_id, _drawLines);
}

bool Console::translateGround(unsigned int _id, const float _translationX,
                              const float _translationY, const float _translationZ){
  return ui->scenary->translateGround(_id, _translationX, _translationY, _translationZ);
}

bool Console::rotateGround(unsigned int _id, const float _pitch, const float _yaw, const float _roll){
  return ui->scenary->rotateGround(_id, _pitch, _yaw, _roll);
}

bool Console::updateGround(unsigned int _id){
  return ui->scenary->updateGround(_id);
}

bool Console::updateAllGround(){
  return ui->scenary->updateAllGround();
}

bool Console::deleteGround(unsigned int _id){
  bool ok = ui->scenary->deleteGround(_id);
  if(ok) deleteGroundButton(_id);
  return ok;
}

void Console::purgeGround(){
  ui->scenary->purgeGround();
  purgeGroundButtons();
}

//----------------------------------- Text labels --------------------------------------//

unsigned int Console::addLabels(const vector<XYZLabel> *const _labels, const QString _name, const bool _visible){
  uint _id = labels.length();
  createNewLabelsButton(_id, _name, _visible);
  TextDraftsman *newLabel = new TextDraftsman(ui->f_labels, _labels, ui->scenary->masterMatrix(),
                                              fonts[12], &onLabels, &labelActives[_id]);
  newLabel->setTransformationMatrix(VehicleTransformationMatrix());
  connect(ui->scenary, SIGNAL(mouseMoving()), newLabel, SLOT(updateLabels()));
  labels.push_back(newLabel);
  return _id;
}

bool Console::setLabelTransformationMatrix(unsigned int _id, const QMatrix4x4 *const _transformationMatrix){
  if(labels.size() < _id) return false;
  else if(labels[_id] == nullptr) return false;
  else{
    labels[_id]->setTransformationMatrix(_transformationMatrix);
    return true;
  }
}

bool Console::translateLabelMatrix(unsigned int _id, const float _translationX, const float _translationY, const float _translationZ){
  if(labels.size() < _id) return false;
  else if(labels[_id] == nullptr) return false;
  else{
    labels[_id]->translateMatrix(_translationX, _translationY, _translationZ);
    return true;
  }
}

bool Console::rotateLabelMatrix(unsigned int _id, const float _pitch, const float _yaw, const float _roll){
  if(labels.size() < _id) return false;
  else if(labels[_id] == nullptr) return false;
  else{
    labels[_id]->rotateMatrix(_pitch, _yaw, _roll);
    return true;
  }
}

bool Console::updateLabel(unsigned int _id){
  if(labels.size() < _id) return false;
  else if(labels[_id] == nullptr) return false;
  else{
    labels[_id]->updateLabels();
    return true;
  }
}

bool Console::deleteLabel(unsigned int _id){
  if(labels.size() < _id) return false;
  else if(labels[_id] == nullptr) return false;
  else{
    delete labels[_id];
    labels[_id] = nullptr;
    deleteLabelsButton(_id);
    return true;
  }
}

void Console::purgeLabels(){
  for(int i = 0; i < labels.size(); i++)
    if(labels[i] != nullptr)
      delete labels[i];
  labels.clear();
  purgeLabelsButtons();
}

// --------------------------------------------------- Maps --------------------------------------------------- //

bool Console::setMapPosition(double *_latitude, double *_longitude){
  return ui->scenary->setMapPosition(_latitude, _longitude);
}

bool Console::setMapRange(unsigned int _width, unsigned int _length){
  return ui->scenary->setMapRange(_width, _length);
}

void Console::setMapTransformationMatrix(QMatrix4x4 *_transformationMatrix){
  ui->scenary->setMapTransformationMatrix(_transformationMatrix);
}

void Console::translateMap(const float _x, const float _y, const float _z){
  ui->scenary->translateMap(_x, _y, _z);
}

void Console::rotateMap(const float _pitch, const float _yaw, const float _roll){
  ui->scenary->rotateMap(_pitch, _yaw, _roll);
}

void Console::checkForNewMapsFiles(){
  on_b_install_released();
}

QString Console::seeMapFolderPath(){
  return ui->scenary->seeMapFolderPath();
}

// --------------------------------------------------- Vertical bar --------------------------------------------------- //

unsigned int Console::addVerticalBar(const QString _name, const int _decimals, const double _max, const double _min,
                                     double *_input, const float _positionX, const float _positionY,
                                     const float _horizontalAlignment, const float _verticalAlignment){
  NewSimpleElement *VerticalBar = new NewSimpleElement(host, VERTICALBAR, _name, _decimals, _max, _min, _input, &fonts,
                                                       _positionX, _positionY, _horizontalAlignment, _verticalAlignment);
  simpleElements.append(VerticalBar);
  raiseF();
  return simpleElements.size() - 1;
}

bool Console::changeVerticalBarName(const unsigned int _ID, const QString _name){
  if(simpleElements.size() < _ID) return false;
  else if(simpleElements[_ID] == nullptr) return false;
  else{
    simpleElements[_ID]->changeName(_name);
    return true;
  }
}

bool Console::changeVerticalBarInput(const unsigned int _ID, double *_input){
  if(simpleElements.size() < _ID) return false;
  else if(simpleElements[_ID] == nullptr) return false;
  else{
    simpleElements[_ID]->changeInput(_input);
    return true;
  }
}

bool Console::changeVerticalBarPosition(const unsigned int _ID, const float _positionX, const float _positionY){
  if(simpleElements.size() < _ID) return false;
  else if(simpleElements[_ID] == nullptr) return false;
  else{
    simpleElements[_ID]->changePosition(_positionX, _positionY);
    return true;
  }
}

bool Console::changeVerticalBarAlignment(const unsigned int _ID, const float _horizontalAlignment, const float _verticalAlignment){
  if(simpleElements.size() < _ID) return false;
  else if(simpleElements[_ID] == nullptr) return false;
  else{
    simpleElements[_ID]->changeAlignment(_horizontalAlignment, _verticalAlignment);
    return true;
  }
}

bool Console::changeVerticalBarMaxMin(const unsigned int _ID, const double _max, const double _min){
  if(simpleElements.size() < _ID) return false;
  else if(simpleElements[_ID] == nullptr) return false;
  else{
    simpleElements[_ID]->changeMaxMin(_max, _min);
    return true;
  }
}

bool Console::updateVerticalBar(const unsigned int _ID){
  if(simpleElements.size() < _ID) return false;
  else if(simpleElements[_ID] == nullptr) return false;
  else{
    simpleElements[_ID]->update();
    return true;
  }
}

bool Console::deleteVerticalBar(const unsigned int _ID){
  if(simpleElements.size() < _ID) return false;
  else if(simpleElements[_ID] == nullptr) return false;
  else{
    delete simpleElements[_ID];
    simpleElements[_ID] = nullptr;
    return true;
  }
}

// --------------------------------------------------- Steering wheel --------------------------------------------------- //

unsigned int Console::addSteering(const QString _name, const int _decimals, double *_input, const float _positionX,
                                  const float _positionY, const float _horizontalAlignment, const float _verticalAlignment){
  NewSimpleElement *Steering = new NewSimpleElement(host, STEERING, _name, _decimals, 1, 0, _input, &fonts,
                                                       _positionX, _positionY, _horizontalAlignment, _verticalAlignment);
  simpleElements.append(Steering);
  raiseF();
  return simpleElements.size() - 1;
}

bool Console::changeSteeringName(const unsigned int _ID, const QString _name){
  if(simpleElements.size() < _ID) return false;
  else if(simpleElements[_ID] == nullptr) return false;
  else{
    simpleElements[_ID]->changeName(_name);
    return true;
  }
}

bool Console::changeSteeringInput(const unsigned int _ID, double *_input){
  if(simpleElements.size() < _ID) return false;
  else if(simpleElements[_ID] == nullptr) return false;
  else{
    simpleElements[_ID]->changeInput(_input);
    return true;
  }
}

bool Console::changeSteeringPosition(const unsigned int _ID, const float _positionX, const float _positionY){
  if(simpleElements.size() < _ID) return false;
  else if(simpleElements[_ID] == nullptr) return false;
  else{
    simpleElements[_ID]->changePosition(_positionX, _positionY);
    return true;
  }
}

bool Console::changeSteeringAlignment(const unsigned int _ID, const float _horizontalAlignment, const float _verticalAlignment){
  if(simpleElements.size() < _ID) return false;
  else if(simpleElements[_ID] == nullptr) return false;
  else{
    simpleElements[_ID]->changeAlignment(_horizontalAlignment, _verticalAlignment);
    return true;
  }
}

bool Console::changeSteeringMaxMin(const unsigned int _ID, const double _max, const double _min){
  if(simpleElements.size() < _ID) return false;
  else if(simpleElements[_ID] == nullptr) return false;
  else{
    simpleElements[_ID]->changeMaxMin(_max, _min);
    return true;
  }
}

bool Console::updateSteering(const unsigned int _ID){
  if(simpleElements.size() < _ID) return false;
  else if(simpleElements[_ID] == nullptr) return false;
  else{
    simpleElements[_ID]->update();
    return true;
  }
}

bool Console::deleteSteering(const unsigned int _ID){
  if(simpleElements.size() < _ID) return false;
  else if(simpleElements[_ID] == nullptr) return false;
  else{
    delete simpleElements[_ID];
    simpleElements[_ID] = nullptr;
    return true;
  }
}


// ----------------------------------------------------- Battery ----------------------------------------------------- //

unsigned int Console::addBattery(const QString _name, const int _decimals, const double _max, const double _min,
                                  double *_input, const float _positionX, const float _positionY,
                                  const float _horizontalAlignment, const float _verticalAlignment){
  NewSimpleElement *Battery = new NewSimpleElement(host, BATTERY, _name, _decimals, _max, _min, _input, &fonts,
                                                       _positionX, _positionY, _horizontalAlignment, _verticalAlignment);
  simpleElements.append(Battery);
  raiseF();
  return simpleElements.size() - 1;
}

bool Console::changeBatteryName(const unsigned int _ID, const QString _name){
  if(simpleElements.size() < _ID) return false;
  else if(simpleElements[_ID] == nullptr) return false;
  else{
    simpleElements[_ID]->changeName(_name);
    return true;
  }
}

bool Console::changeBatteryInput(const unsigned int _ID, double *_input){
  if(simpleElements.size() < _ID) return false;
  else if(simpleElements[_ID] == nullptr) return false;
  else{
    simpleElements[_ID]->changeInput(_input);
    return true;
  }
}

bool Console::changeBatteryPosition(const unsigned int _ID, const float _positionX, const float _positionY){
  if(simpleElements.size() < _ID) return false;
  else if(simpleElements[_ID] == nullptr) return false;
  else{
    simpleElements[_ID]->changePosition(_positionX, _positionY);
    return true;
  }
}

bool Console::changeBatteryAlignment(const unsigned int _ID, const float _horizontalAlignment, const float _verticalAlignment){
  if(simpleElements.size() < _ID) return false;
  else if(simpleElements[_ID] == nullptr) return false;
  else{
    simpleElements[_ID]->changeAlignment(_horizontalAlignment, _verticalAlignment);
    return true;
  }
}

bool Console::changeBatteryMaxMin(const unsigned int _ID, const double _max, const double _min){
  if(simpleElements.size() < _ID) return false;
  else if(simpleElements[_ID] == nullptr) return false;
  else{
    simpleElements[_ID]->changeMaxMin(_max, _min);
    return true;
  }
}

bool Console::updateBattery(const unsigned int _ID){
  if(simpleElements.size() < _ID) return false;
  else if(simpleElements[_ID] == nullptr) return false;
  else{
    simpleElements[_ID]->update();
    return true;
  }
}

bool Console::deleteBattery(const unsigned int _ID){
  if(simpleElements.size() < _ID) return false;
  else if(simpleElements[_ID] == nullptr) return false;
  else{
    delete simpleElements[_ID];
    simpleElements[_ID] = nullptr;
    return true;
  }
}

// ----------------------------------------------------- Speedometer ----------------------------------------------------- //

unsigned int Console::addSpeedometer(const QString _name, const double _maxSpeed, const double _minSpeed, const double _maxRPM,
                                     const double _minRPM, double *_inputSpeed, double *_inputRPM, const float _positionX,
                                     const float _positionY, const float _horizontalAlignment, const float _verticalAlignment){
  NewSpeedo *Speedo = new NewSpeedo(host, _name, _maxSpeed, _minSpeed, _maxRPM, _minRPM, _inputSpeed, _inputRPM, &fonts,
                                    _positionX, _positionY, _horizontalAlignment, _verticalAlignment);
  speedos.append(Speedo);
  raiseF();
  return speedos.size() - 1;
}

bool Console::changeSpeedometerName(const unsigned int _ID, const QString _name){
  if(speedos.size() < _ID) return false;
  else if(speedos[_ID] == nullptr) return false;
  else{
    speedos[_ID]->changeName(_name);
    return true;
  }
}

bool Console::changeSpeedometerInput(const unsigned int _ID, double *_inputSpeed, double *_inputRPM){
  if(speedos.size() < _ID) return false;
  else if(speedos[_ID] == nullptr) return false;
  else{
    speedos[_ID]->changeInputs(_inputSpeed, _inputRPM);
    return true;
  }
}

bool Console::changeSpeedometerPosition(const unsigned int _ID, const float _positionX, const float _positionY){
  if(speedos.size() < _ID) return false;
  else if(speedos[_ID] == nullptr) return false;
  else{
    speedos[_ID]->changePosition(_positionX, _positionY);
    return true;
  }
}

bool Console::changeSpeedometerAlignment(const unsigned int _ID, const float _horizontalAlignment, const float _verticalAlignment){
  if(speedos.size() < _ID) return false;
  else if(speedos[_ID] == nullptr) return false;
  else{
    speedos[_ID]->changeAlignment(_horizontalAlignment, _verticalAlignment);
    return true;
  }
}

bool Console::changeSpeedometerMaxMin(const unsigned int _ID, const double _maxSpeed, const double _minSpeed,
                                      const double _maxRPM, const double _minRPM){
  if(speedos.size() < _ID) return false;
  else if(speedos[_ID] == nullptr) return false;
  else{
    speedos[_ID]->changeMaxMin(_maxSpeed, _minSpeed, _maxRPM, _minRPM);
    return true;
  }
}

bool Console::updateSpeedometer(const unsigned int _ID){
  if(speedos.size() < _ID) return false;
  else if(speedos[_ID] == nullptr) return false;
  else{
    speedos[_ID]->update();
    return true;
  }
}

bool Console::deleteSpeedometer(const unsigned int _ID){
  if(speedos.size() < _ID) return false;
  else if(speedos[_ID] == nullptr) return false;
  else{
    delete speedos[_ID];
    speedos[_ID] = nullptr;
    return true;
  }
}

// ------------------------------------------------------- Gearbox ------------------------------------------------------- //

unsigned int Console::addGearbox(const int _type, const QString _name, vector<nameANDvalue> *_values, double *_input,
                                 const float _positionX, const float _positionY, const float _horizontalAlignment,
                                 const float _verticalAlignment){
  NewGearbox *Gearboxes = new NewGearbox(host, _type, _name, _values, _input, &fonts, _positionX, _positionY,
                                         _horizontalAlignment, _verticalAlignment);
  gearboxes.append(Gearboxes);
  raiseF();
  return gearboxes.size() - 1;
}

bool Console::changeGearboxType(const unsigned int _ID, const int _type){
  if(gearboxes.size() < _ID) return false;
  else if(gearboxes[_ID] == nullptr) return false;
  else{
    gearboxes[_ID]->changeType(_type);
    return true;
  }
}

bool Console::changeGearboxName(const unsigned int _ID, const QString _name){
  if(gearboxes.size() < _ID) return false;
  else if(gearboxes[_ID] == nullptr) return false;
  else{
    gearboxes[_ID]->changeName(_name);
    return true;
  }
}

bool Console::changeGearboxInput(const unsigned int _ID, double *_input){
  if(gearboxes.size() < _ID) return false;
  else if(gearboxes[_ID] == nullptr) return false;
  else{
    gearboxes[_ID]->changeInput(_input);
    return true;
  }
}

bool Console::changeGearboxPosition(const unsigned int _ID, const float _positionX, const float _positionY){
  if(gearboxes.size() < _ID) return false;
  else if(gearboxes[_ID] == nullptr) return false;
  else{
    gearboxes[_ID]->changePosition(_positionX, _positionY);
    return true;
  }
}

bool Console::changeGearboxAlignment(const unsigned int _ID, const float _horizontalAlignment, const float _verticalAlignment){
  if(gearboxes.size() < _ID) return false;
  else if(gearboxes[_ID] == nullptr) return false;
  else{
    gearboxes[_ID]->changeAlignment(_horizontalAlignment, _verticalAlignment);
    return true;
  }
}

bool Console::changeGearboxLabels(const unsigned int _ID, vector<nameANDvalue> *_values){
  if(gearboxes.size() < _ID) return false;
  else if(gearboxes[_ID] == nullptr) return false;
  else{
    gearboxes[_ID]->changeLabels(_values);
    return true;
  }
}

bool Console::updateGearbox(const unsigned int _ID){
  if(gearboxes.size() < _ID) return false;
  else if(gearboxes[_ID] == nullptr) return false;
  else{
    gearboxes[_ID]->update();
    return true;
  }
}

bool Console::deleteGearbox(const unsigned int _ID){
  if(gearboxes.size() < _ID) return false;
  else if(gearboxes[_ID] == nullptr) return false;
  else{
    delete gearboxes[_ID];
    gearboxes[_ID] = nullptr;
    return true;
  }
}

// ------------------------------------------------------- Text List ------------------------------------------------------- //

unsigned int Console::addTextList(const QString _name, vector<nameANDdata> *_values, const float _positionX,
                                  const float _positionY, const float _width, const float _height,
                                  const float _horizontalAlignment, const float _verticalAlignment){
  NewTextList *Textlist = new NewTextList(host, _name, _values, &fonts, _positionX, _positionY, _width,
                                          _height, _horizontalAlignment, _verticalAlignment);
  textlists.append(Textlist);
  raiseF();
  return textlists.size() - 1;
}

bool Console::changeTextListName(const unsigned int _ID, const QString _name){
  if(textlists.size() < _ID) return false;
  else if(textlists[_ID] == nullptr) return false;
  else{
    textlists[_ID]->changeName(_name);
    return true;
  }
}

bool Console::changeTextListInputs(const unsigned int _ID, vector<nameANDdata> *_values){
  if(textlists.size() < _ID) return false;
  else if(textlists[_ID] == nullptr) return false;
  else{
    textlists[_ID]->changeInputs(_values);
    return true;
  }
}

bool Console::changeTextListPosition(const unsigned int _ID, const float _positionX, const float _positionY){
  if(textlists.size() < _ID) return false;
  else if(textlists[_ID] == nullptr) return false;
  else{
    textlists[_ID]->changePosition(_positionX, _positionY);
    return true;
  }
}

bool Console::changeTextListAlignment(const unsigned int _ID, const float _horizontalAlignment, const float _verticalAlignment){
  if(textlists.size() < _ID) return false;
  else if(textlists[_ID] == nullptr) return false;
  else{
    textlists[_ID]->changeAlignment(_horizontalAlignment, _verticalAlignment);
    return true;
  }
}

bool Console::changeTextListSize(const unsigned int _ID, const float _width, const float _height){
  if(textlists.size() < _ID) return false;
  else if(textlists[_ID] == nullptr) return false;
  else{
    textlists[_ID]->changeSize(_width, _height);
    return true;
  }
}

bool Console::updateTextList(const unsigned int _ID){
  if(textlists.size() < _ID) return false;
  else if(textlists[_ID] == nullptr) return false;
  else{
    textlists[_ID]->update();
    return true;
  }
}

bool Console::deleteTextList(const unsigned int _ID){
  if(textlists.size() < _ID) return false;
  else if(textlists[_ID] == nullptr) return false;
  else{
    delete textlists[_ID];
    textlists[_ID] = nullptr;
    return true;
  }
}

// ------------------------------------------------------- Icon List ------------------------------------------------------- //

unsigned int Console::addIconList(const QString _name, vector<nameANDboolen> *_values,
                                  const float _positionX, const float _positionY, const float _width,
                                  const float _height, const float _horizontalAlignment,
                                  const float _verticalAlignment){
  NewIconList *Iconlist = new NewIconList(host, _name, _values, &fonts, _positionX, _positionY, _width,
                                          _height, _horizontalAlignment, _verticalAlignment);
  iconlists.append(Iconlist);
  raiseF();
  return iconlists.size() - 1;
}

bool Console::changeIconListName(const unsigned int _ID, const QString _name){
  if(iconlists.size() < _ID) return false;
  else if(iconlists[_ID] == nullptr) return false;
  else{
    iconlists[_ID]->changeName(_name);
    return true;
  }
}

bool Console::changeIconListInputs(const unsigned int _ID, vector<nameANDboolen> *_values){
  if(iconlists.size() < _ID) return false;
  else if(iconlists[_ID] == nullptr) return false;
  else{
    iconlists[_ID]->changeInputs(_values);
    return true;
  }
}

bool Console::changeIconListPosition(const unsigned int _ID, const float _positionX, const float _positionY){
  if(iconlists.size() < _ID) return false;
  else if(iconlists[_ID] == nullptr) return false;
  else{
    iconlists[_ID]->changePosition(_positionX, _positionY);
    return true;
  }
}

bool Console::changeIconListAlignment(const unsigned int _ID, const float _horizontalAlignment,
                                      const float _verticalAlignment){
  if(iconlists.size() < _ID) return false;
  else if(iconlists[_ID] == nullptr) return false;
  else{
    iconlists[_ID]->changeAlignment(_horizontalAlignment, _verticalAlignment);
    return true;
  }
}

bool Console::changeIconListSize(const unsigned int _ID, const float _width, const float _height){
  if(iconlists.size() < _ID) return false;
  else if(iconlists[_ID] == nullptr) return false;
  else{
    iconlists[_ID]->changeSize(_width, _height);
    return true;
  }
}

bool Console::updateIconList(const unsigned int _ID){
  if(iconlists.size() < _ID) return false;
  else if(iconlists[_ID] == nullptr) return false;
  else{
    iconlists[_ID]->update();
    return true;
  }
}

bool Console::deleteIconList(const unsigned int _ID){
  if(iconlists.size() < _ID) return false;
  else if(iconlists[_ID] == nullptr) return false;
  else{
    delete iconlists[_ID];
    iconlists[_ID] = nullptr;
    return true;
  }
}

// ------------------------------------------------------- Message List ------------------------------------------------------- //

unsigned int Console::addMessageList(const QString _name, const float _positionX, const float _positionY, const float _width,
                                     const float _height, const float _horizontalAlignment, const float _verticalAlignment){
  NewMessages *Messagelist = new NewMessages(host, _name, &fonts, _positionX, _positionY, _width, _height, _horizontalAlignment,
                                             _verticalAlignment);
  messagelists.append(Messagelist);
  raiseF();
  return messagelists.size() - 1;
}

bool Console::changeMessageListName(const unsigned int _ID, const QString _name){
  if(messagelists.size() < _ID) return false;
  else if(messagelists[_ID] == nullptr) return false;
  else{
    messagelists[_ID]->changeName(_name);
    return true;
  }
}

bool Console::changeMessageListPosition(const unsigned int _ID, const float _positionX, const float _positionY){
  if(messagelists.size() < _ID) return false;
  else if(messagelists[_ID] == nullptr) return false;
  else{
    messagelists[_ID]->changePosition(_positionX, _positionY);
    return true;
  }
}

bool Console::changeMessageListAlignment(const unsigned int _ID, const float _horizontalAlignment, const float _verticalAlignment){
  if(messagelists.size() < _ID) return false;
  else if(messagelists[_ID] == nullptr) return false;
  else{
    messagelists[_ID]->changeAlignment(_horizontalAlignment, _verticalAlignment);
    return true;
  }
}

bool Console::changeMessageListSize(const unsigned int _ID, const float _width, const float _height){
  if(messagelists.size() < _ID) return false;
  else if(messagelists[_ID] == nullptr) return false;
  else{
    messagelists[_ID]->changeSize(_width, _height);
    return true;
  }
}

bool Console::addMessageToList(const unsigned int _ID, const QString _message, const int _type){
  if(messagelists.size() < _ID) return false;
  else if(messagelists[_ID] == nullptr) return false;
  else{
    messagelists[_ID]->addMessage(_message, _type);
    return true;
  }
}

bool Console::ClearMessagesAtList(const unsigned int _ID){
  if(messagelists.size() < _ID) return false;
  else if(messagelists[_ID] == nullptr) return false;
  else{
    messagelists[_ID]->clearMessages();
    return true;
  }
}

bool Console::deleteMessageList(const unsigned int _ID){
  if(messagelists.size() < _ID) return false;
  else if(messagelists[_ID] == nullptr) return false;
  else{
    delete messagelists[_ID];
    messagelists[_ID] = nullptr;
    return true;
  }
}

// ------------------------------------------------------- Signaling ------------------------------------------------------- //

unsigned int Console::addSignaling(const float _positionX, const float _positionY, const float _width, const float _height,
                                   const float _horizontalAlignment, const float _verticalAlignment){
  NewSignaling *Signaling = new NewSignaling(host, &fonts, _positionX, _positionY, _width, _height, _horizontalAlignment,
                                             _verticalAlignment);
  signs.append(Signaling);
  raiseF();
  return signs.size() - 1;
}

bool Console::changeSignalingPosition(const unsigned int _ID, const float _positionX, const float _positionY){
  if(signs.size() < _ID) return false;
  else if(signs[_ID] == nullptr) return false;
  else{
    signs[_ID]->changePosition(_positionX, _positionY);
    return true;
  }
}

bool Console::changeSignalingAlignment(const unsigned int _ID, const float _horizontalAlignment, const float _verticalAlignment){
  if(signs.size() < _ID) return false;
  else if(signs[_ID] == nullptr) return false;
  else{
    signs[_ID]->changeAlignment(_horizontalAlignment, _verticalAlignment);
    return true;
  }
}

bool Console::changeSignalingSize(const unsigned int _ID, const float _width, const float _height){
  if(signs.size() < _ID) return false;
  else if(signs[_ID] == nullptr) return false;
  else{
    signs[_ID]->changeSize(_width, _height);
    return true;
  }
}

bool Console::addSign(const unsigned int _ID, const int _type, const int _numericalValue){
  if(signs.size() < _ID) return false;
  else if(signs[_ID] == nullptr) return false;
  else{
    signs[_ID]->addSignal(_type, _numericalValue);
    return true;
  }
}

bool Console::ClearSigns(const unsigned int _ID){
  if(signs.size() < _ID) return false;
  else if(signs[_ID] == nullptr) return false;
  else{
    signs[_ID]->clearSignals();
    return true;
  }
}

bool Console::deleteSignaling(const unsigned int _ID){
  if(signs.size() < _ID) return false;
  else if(signs[_ID] == nullptr) return false;
  else{
    delete signs[_ID];
    signs[_ID] = nullptr;
    return true;
  }
}

// ------------------------------------------------ Clock ------------------------------------------------ //

unsigned int Console::addClock(const QString *const _timestamp, const float _positionX, const float _positionY, const float _width,
                               const float _height, const float _horizontalAlignment, const float _verticalAlignment){
  NewClock *Clock = new NewClock(host, &whichCompany, _timestamp, &fonts, _positionX, _positionY, _width, _height,
                                 _horizontalAlignment, _verticalAlignment);
  clocks.append(Clock);
  return clocks.size() - 1;
}

bool Console::changeClockPosition(const unsigned int _ID, const float _positionX, const float _positionY){
  if(clocks.size() < _ID) return false;
  else if(clocks[_ID] == nullptr) return false;
  else{
    clocks[_ID]->changePosition(_positionX, _positionY);
    return true;
  }
}

bool Console::changeClockAlignment(const unsigned int _ID, const float _horizontalAlignment, const float _verticalAlignment){
  if(clocks.size() < _ID) return false;
  else if(clocks[_ID] == nullptr) return false;
  else{
    clocks[_ID]->changeAlignment(_horizontalAlignment, _verticalAlignment);
    return true;
  }
}

bool Console::changeClockSize(const unsigned int _ID, const float _width, const float _height){
  if(clocks.size() < _ID) return false;
  else if(clocks[_ID] == nullptr) return false;
  else{
    clocks[_ID]->changeSize(_width, _height);
    return true;
  }
}

bool Console::updateClock(const unsigned int _ID){
  if(clocks.size() < _ID) return false;
  else if(clocks[_ID] == nullptr) return false;
  else{
    clocks[_ID]->update();
    return true;
  }
}

bool Console::deleteClock(const unsigned int _ID){
  if(clocks.size() < _ID) return false;
  else if(clocks[_ID] == nullptr) return false;
  else{
    delete clocks[_ID];
    clocks[_ID] = nullptr;
    return true;
  }
}

// ---------------------------------------------------- Compass ---------------------------------------------------- //

unsigned int Console::addCompass(const float _positionX, const float _positionY, const float _width,
                                 const float _height, const float _horizontalAlignment, const float _verticalAlignment){
  NewCompass *Compass = new NewCompass(host, &ui->scenary->staticVehicleYaw, frameYaw, scenaryRotationY, &fonts,
                                       _positionX, _positionY, _width, _height, _horizontalAlignment, _verticalAlignment);
  compasses.append(Compass);
  return compasses.size() - 1;
}

bool Console::changeCompassPosition(const unsigned int _ID, const float _positionX, const float _positionY){
  if(compasses.size() < _ID) return false;
  else if(compasses[_ID] == nullptr) return false;
  else{
    compasses[_ID]->changePosition(_positionX, _positionY);
    return true;
  }
}

bool Console::changeCompassAlignment(const unsigned int _ID, const float _horizontalAlignment, const float _verticalAlignment){
  if(compasses.size() < _ID) return false;
  else if(compasses[_ID] == nullptr) return false;
  else{
    compasses[_ID]->changeAlignment(_horizontalAlignment, _verticalAlignment);
    return true;
  }
}

bool Console::changeCompassSize(const unsigned int _ID, const float _width, const float _height){
  if(compasses.size() < _ID) return false;
  else if(compasses[_ID] == nullptr) return false;
  else{
    compasses[_ID]->changeSize(_width, _height);
    return true;
  }
}

bool Console::deleteCompass(const unsigned int _ID){
  if(compasses.size() < _ID) return false;
  else if(compasses[_ID] == nullptr) return false;
  else{
    delete compasses[_ID];
    compasses[_ID] = nullptr;
    return true;
  }
}

// ---------------------------------------------------- Controls ---------------------------------------------------- //

bool Console::addControl(const unsigned int _initialDataset, const unsigned int _initialFrame, const unsigned int _initialDelayment){
  if(createdControl)
    return false;
  else{
    controls = new NewControls(host, _initialDataset, _initialFrame, _initialDelayment, &onOptions, &fonts);
    createdControl = true;
    return true;
  }
}

bool Console::gotoFrame(const unsigned int _frame){
  return controls->gotoFrame(_frame);
}

bool Console::gotoDataset(const unsigned int _dataset){
  ui->scenary->streets->restartData();
  return controls->gotoDataset(_dataset);
}

void Console::firstFrame(){
  controls->firstFrame();
}

void Console::nextFrame(){
  controls->nextFrame();
}

void Console::previousFrame(){
  controls->previousFrame();
}

void Console::lastFrame(){
  controls->lastFrame();
}

void Console::setDelayment(const unsigned int _delay){
  controls->setDelayment(_delay);
}

void Console::play(){
  controls->play();
}

void Console::pause(){
  controls->pause();
}

bool Console::deleteControl(){
  if(controls != nullptr){
    delete controls;
    controls = nullptr;
    return true;
  }else{
    return false;
  }
}

// ---------------------------------------------------- Cameras ---------------------------------------------------- //

unsigned int Console::addCamera(const QString _name, const unsigned int _initialDataset,
                                const unsigned int _initialFrame, const unsigned int _cameraSet,
                                const float _width, const float _height, const float _positionX,
                                const float _positionY, const float _horizontalAlignment,
                                const float _verticalAlignment){
  NewCamera *camera = new NewCamera(host, _name, _initialDataset, _initialFrame, _cameraSet, _width,
                                    _height, _positionX, _positionY, _horizontalAlignment, _verticalAlignment, &fonts);
  cameras.append(camera);
  raiseF();
  return cameras.size() - 1;
}

bool Console::changeCameraName(unsigned int _ID, const QString _name){
  if(cameras.size() < _ID) return false;
  else if(cameras[_ID] == nullptr) return false;
  else{
    cameras[_ID]->changeName(_name);
    return true;
  }
}

bool Console::changeCameraSize(unsigned int _ID, const float _width, const float _height){
  if(cameras.size() < _ID) return false;
  else if(cameras[_ID] == nullptr) return false;
  else{
    cameras[_ID]->changeSize(_width, _height);
    return true;
  }
}

bool Console::changeCameraPosition(unsigned int _ID, const float _positionX, const float _positionY){
  if(cameras.size() < _ID) return false;
  else if(cameras[_ID] == nullptr) return false;
  else{
    cameras[_ID]->changePosition(_positionX, _positionY);
    return true;
  }
}

bool Console::changeCameraAlignment(unsigned int _ID, const float _horizontalAlignment,
                                    const float _verticalAlignment){
  if(cameras.size() < _ID) return false;
  else if(cameras[_ID] == nullptr) return false;
  else{
    cameras[_ID]->changeAlignment(_horizontalAlignment, _verticalAlignment);
    return true;
  }
}

bool Console::updateCamera(unsigned int _ID){
  if(cameras.size() < _ID) return false;
  else if(cameras[_ID] == nullptr) return false;
  else{
    cameras[_ID]->update();
    return true;
  }
}

bool Console::deleteCamera(unsigned int _ID){
  if(cameras.size() < _ID) return false;
  else if(cameras[_ID] == nullptr) return false;
  else{
    delete cameras[_ID];
    cameras[_ID] = nullptr;
    return true;
  }
}

void Console::setCoordinateConversor(CoordinateConversor *_conversor){
  ui->scenary->setCoordinateConversor(_conversor);
}

void Console::setAlternative(bool _alternative){
  updateAllTrajectories();
}

#ifdef ROS_DATA
void Console::setROSspinner(int _ROS_cycle_time_milliseconds){
  ROStimer->start(_ROS_cycle_time_milliseconds);
}
#endif

// ----------------------------------------------------- Vehicle ----------------------------------------------------- //

bool Console::addVehicle(const unsigned int _model){
  return ui->scenary->addVehicle(_model);
}

void Console::addVehicleRotation(double *_pitch, double *_yaw, double *_roll){
  vehiclePitch = _pitch;
  vehicleYaw = _yaw;
  vehicleRoll = _roll;

  ui->scenary->vehiclePitch = _pitch;
  ui->scenary->vehicleYaw = _yaw;
  ui->scenary->vehicleRoll = _roll;

  ui->scenary->staticVehicleYaw = *_yaw;
}

void Console::addVehiclePosition(double *_positionX, double *_positionY, double *_positionZ,
                                 double *_latitude, double *_longitude){
  vehiclePositionX = _positionX;
  vehiclePositionY = _positionY;
  vehiclePositionZ = _positionZ;
  vehicle_latitude = _latitude;
  vehicle_longitude = _longitude;
  ui->scenary->vehiclePositionX = _positionX;
  ui->scenary->vehiclePositionY = _positionY;
  ui->scenary->vehiclePositionZ = _positionZ;
  ui->scenary->vehicle_latitude = _latitude;
  ui->scenary->vehicle_longitude = _longitude;
}

void Console::addVehicleAccelerations(double *_forwardAcceleration, double *_leftwardAcceleration, double *_upwardAcceleration){
  vehicleForwardAcceleration = _forwardAcceleration;
  vehicleLeftwardAcceleration = _leftwardAcceleration;
  vehicleUpwardAcceleration = _upwardAcceleration;

  if(vehicleSteeringAngle == nullptr){
    vehicleSteeringAngle = _leftwardAcceleration;
    ui->scenary->vehicleSteeringAngle = _leftwardAcceleration;
  }

  ui->scenary->vehicleForwardAcceleration = _forwardAcceleration;
  ui->scenary->vehicleLeftwardAcceleration = _leftwardAcceleration;
  ui->scenary->vehicleUpwardAcceleration = _upwardAcceleration;
}

void Console::addVehicleSpeed(double *_speed){
  vehicleSpeed = _speed;
  ui->scenary->vehicleSpeed = _speed;
}

void Console::addVehicleSteeringWheelAngle(double *_steeringAngle){
  vehicleSteeringAngle = _steeringAngle;
  ui->scenary->vehicleSteeringAngle = _steeringAngle;
}

void Console::setLateralAxesLength(const float _length){
  ui->scenary->setLateralAxesLength(_length);
}

void Console::setTireSize(const float _diameter, const float _width){
  ui->scenary->setTireSize(_diameter, _width);
}

void Console::setVehicleDimensions(const float _A, const float _B, const float _C, const float _D,
                                   const float _E, const float _F, const float _height){
  ui->scenary->setVehicleDimensions(_A, _B, _C, _D, _E, _F, _height);
}

void Console::changeChassisPosition(const float _x, const float _y, const float _z){
  ui->scenary->changeChassisPosition(_x, _y, _z);
}

void Console::moveVehicleOriginsCoordinateSystem(const float _x, const float _y, const float _z){
  ui->scenary->moveVehicleOriginsCoordinateSystem(_x, _y, _z);
}

// ------------------------------------------------ Coordinate system ------------------------------------------------ //

unsigned int Console::addFixedFrame(const float _x, const float _y, const float _z,
                                    const float _pitch, const float _yaw, const float _roll){
  return ui->scenary->addFixedFrame(_x, _y, _z, _pitch, _yaw, _roll);
}

bool Console::deleteFixedFrame(const unsigned int _id){
  return ui->scenary->deleteFixedFrame(_id);
}

void Console::purgeFixedFrames(){
  ui->scenary->purgeFixedFrames();
}

void Console::setFixedFrameOrigin(double *_latitude, double *_longitude, double *_altitude,
                                  double *_pitch, double *_yaw, double *_roll){
  frameLatitude = _latitude;
  frameLongitude = _longitude;
  frameAltitude = _altitude;
  framePitch = _pitch;
  frameYaw = _yaw;
  frameRoll = _roll;

  updateFixedFrameTransformationMatrix();

  ui->scenary->addFixedFrame();
  ui->scenary->framePitch = framePitch;
  ui->scenary->frameYaw = frameYaw;
  ui->scenary->frameRoll = frameRoll;

  for(int i = 0; i < compasses.size(); i++)
    if(compasses[i] != nullptr)
      compasses[i]->setFrameYaw(_yaw);
}

void Console::updateFixedFrameTransformationMatrix(){
  fixedFrameTransformationMatrix.setToIdentity();
  fixedFrameTransformationMatrix.rotate(QQuaternion::fromEulerAngles(*framePitch * toDEGREES, *frameYaw * toDEGREES, *frameRoll * toDEGREES));
  fixedPositionTransformationMatrix.setToIdentity();
  fixedPositionTransformationMatrix.translate(0, *vehiclePositionZ, 0);
//  fixedPositionTransformationMatrix.rotate(QQuaternion::fromEulerAngles(0, *frameYaw * toDEGREES, 0));
}

void Console::updateVehicleTransformationMatrix(){
  fixedFrameTransformationMatrix.setToIdentity();
  fixedFrameTransformationMatrix.rotate(QQuaternion::fromEulerAngles(*framePitch * toDEGREES, *frameYaw * toDEGREES, *frameRoll * toDEGREES));
  fixedPositionTransformationMatrix.setToIdentity();
  fixedPositionTransformationMatrix.translate(0, *vehiclePositionZ, 0);
//  fixedPositionTransformationMatrix.rotate(QQuaternion::fromEulerAngles(0, *frameYaw * toDEGREES, 0));

  vehicleTransformationMatrix.setToIdentity();
  vehicleTransformationMatrix.translate(-*vehiclePositionY, *vehiclePositionZ, -*vehiclePositionX);
  navigationTransformationMatrix = vehicleTransformationMatrix;
  navigationTransformationMatrix.rotate(QQuaternion::fromEulerAngles(-*framePitch * toDEGREES, -*frameYaw * toDEGREES, -*frameRoll * toDEGREES));
  plus90VehicleTransformationMatrix = vehicleTransformationMatrix;
  plus90VehicleTransformationMatrix.rotate(QQuaternion::fromEulerAngles((-*vehiclePitch + *framePitch) * toDEGREES, 90 - *frameYaw * toDEGREES, (-*vehicleRoll + *frameRoll) * toDEGREES));
  vehicleTransformationMatrix.rotate(QQuaternion::fromEulerAngles((-*vehiclePitch + *framePitch) * toDEGREES, (*vehicleYaw - *frameYaw) * toDEGREES, (-*vehicleRoll + *frameRoll) * toDEGREES));
/*
  fixedFrameTransformationMatrix.setToIdentity();
//  fixedFrameTransformationMatrix.rotate(QQuaternion::fromEulerAngles(*framePitch * toDEGREES, *frameYaw * toDEGREES, *frameRoll * toDEGREES));

  vehicleTransformationMatrix.setToIdentity();
//  vehicleTransformationMatrix.rotate(QQuaternion::fromEulerAngles((-*vehiclePitch + *framePitch) * toDEGREES, (*vehicleYaw - *frameYaw) * toDEGREES, (-*vehicleRoll + *frameRoll) * toDEGREES));
  vehicleTransformationMatrix.translate(-*vehiclePositionY, *vehiclePositionZ, -*vehiclePositionX);
  navigationTransformationMatrix.setToIdentity();
  navigationTransformationMatrix.rotate(QQuaternion::fromEulerAngles(-*framePitch * toDEGREES, -*frameYaw * toDEGREES, -*frameRoll * toDEGREES));
  navigationTransformationMatrix.translate(-*vehiclePositionY, *vehiclePositionZ, -*vehiclePositionX);
*/
}

// ------------------------------------------------ Button event handler ------------------------------------------------ //

void Console::on_b_op_released(){
  onOptions = !onOptions;
  if(onOptions)
    ui->f_options->move(0, 0);
  else
    ui->f_options->move(0, this->height() + 20);

  checkOptions();

  emit options_trigger();
}

void Console::on_b_tv_released(){
  ui->scenary->setTopView();
}

void Console::on_b_sv_released(){
  ui->scenary->setStandardView();
}

void Console::on_b_lsc_released(){
  onCloud = !onCloud;
  if(onCloud)
    ui->b_lsc->setStyleSheet(onCss.arg("top"));
  else
    ui->b_lsc->setStyleSheet(onCss.arg("bottom"));
  ui->scenary->update();
}

void Console::on_b_ls_color_released(){
  onColorize = !onColorize;
  if(onColorize)
    ui->b_ls_color->setStyleSheet(onCss.arg("top"));
  else
    ui->b_ls_color->setStyleSheet(onCss.arg("bottom"));
  ui->scenary->update();
}

void Console::on_b_movables_released(){
  onMovables = !onMovables;
  if(onMovables)
    ui->b_movables->setStyleSheet(onCss.arg("top"));
  else
    ui->b_movables->setStyleSheet(onCss.arg("bottom"));
  ui->scenary->update();
}

void Console::on_b_vehicle_released(){
  onVehicle = !onVehicle;
  if(onVehicle)
    ui->b_vehicle->setStyleSheet(onCss.arg("top"));
  else
    ui->b_vehicle->setStyleSheet(onCss.arg("bottom"));
  ui->scenary->update();
}

void Console::on_b_streets_released(){
  onStreets = !onStreets;
  if(onStreets)
    ui->b_streets->setStyleSheet(onCss.arg("top"));
  else
    ui->b_streets->setStyleSheet(onCss.arg("bottom"));
  ui->scenary->update();
}

void Console::on_b_signals_released(){
  onSignalization = !onSignalization;
  if(onSignalization)
    ui->b_signals->setStyleSheet(onCss.arg("top"));
  else
    ui->b_signals->setStyleSheet(onCss.arg("bottom"));
  ui->scenary->update();
}

void Console::on_b_trajectory_released(){
  onTrajectory = !onTrajectory;
  if(onTrajectory)
    ui->b_trajectory->setStyleSheet(onCss.arg("top"));
  else
    ui->b_trajectory->setStyleSheet(onCss.arg("bottom"));
  ui->scenary->update();
}

void Console::on_b_ground_released(){
  onGround = !onGround;
  if(onGround)
    ui->b_ground->setStyleSheet(onCss.arg("top"));
  else
    ui->b_ground->setStyleSheet(onCss.arg("bottom"));
  ui->scenary->update();
}

void Console::on_b_skybox_released(){
  onSkybox = !onSkybox;
  if(onSkybox)
    ui->b_skybox->setStyleSheet(onCss.arg("top"));
  else
    ui->b_skybox->setStyleSheet(onCss.arg("bottom"));
  ui->scenary->update();
}

void Console::on_b_chassis_released(){
  onChassis = !onChassis;
  if(onChassis)
    ui->b_chassis->setStyleSheet(onCss.arg("top"));
  else
    ui->b_chassis->setStyleSheet(onCss.arg("bottom"));
  ui->scenary->update();
}

void Console::on_b_rebound_released(){
  onChassisRebounding = !onChassisRebounding;
  if(onChassisRebounding)
    ui->b_rebound->setStyleSheet(onCss.arg("top"));
  else
    ui->b_rebound->setStyleSheet(onCss.arg("bottom"));
  ui->scenary->update();
}

void Console::on_b_axes_released(){
  onAxes = !onAxes;
  if(onAxes)
    ui->b_axes->setStyleSheet(onCss.arg("top"));
  else
    ui->b_axes->setStyleSheet(onCss.arg("bottom"));
  ui->scenary->update();
}

void Console::on_b_texture_released(){
  onAxesTexture = !onAxesTexture;
  if(onAxesTexture)
    ui->b_texture->setStyleSheet(onCss.arg("top"));
  else
    ui->b_texture->setStyleSheet(onCss.arg("bottom"));
  ui->scenary->update();
}

void Console::on_b_rotation_released(){
  onAxesRotation = !onAxesRotation;
  if(onAxesRotation)
    ui->b_rotation->setStyleSheet(onCss.arg("top"));
  else
    ui->b_rotation->setStyleSheet(onCss.arg("bottom"));
  ui->scenary->update();
}

void Console::on_b_box_released(){
  onBox = !onBox;
  if(onBox)
    ui->b_box->setStyleSheet(onCss.arg("top"));
  else
    ui->b_box->setStyleSheet(onCss.arg("bottom"));
  ui->scenary->update();
}

void Console::on_b_svw_released(){
  isCameraLocked = !isCameraLocked;
  if(!isCameraLocked) ui->b_svw->setStyleSheet(btnCss.arg("bottom").arg("2"));
  else ui->b_svw->setStyleSheet(btnCss.arg("top").arg("2"));
}

void Console::on_b_fvw_released(){
  isStaticCamera = !isStaticCamera;
  if(isStaticCamera) ui->b_fvw->setStyleSheet(btnCss.arg("bottom").arg("3"));
  else ui->b_fvw->setStyleSheet(btnCss.arg("top").arg("3"));
}

void Console::on_b_tags_released(){
  onLabels = !onLabels;
  if(onLabels)
    ui->b_tags->setStyleSheet(onCss.arg("top"));
  else
    ui->b_tags->setStyleSheet(onCss.arg("bottom"));
  for(int i = 0; i < labels.size(); i++)
    if(labels[i] != nullptr)
      labels[i]->updateLabels();
}


// ------------------------------------------------ Camera handler ------------------------------------------------ //

void Console::ChangeCameraPosition(const float _x, const float _y, const float _z){
  ui->scenary->ChangeCameraPosition(_x, _y, _z);
}

void Console::ChangeCameraUpVector(const float _x, const float _y, const float _z){
  ui->scenary->ChangeCameraUpVector(_x, _y, _z);
}

void Console::ChangeCameraLookingAt(const float _x, const float _y, const float _z){
  ui->scenary->ChangeCameraLookingAt(_x, _y, _z);
}

void Console::ChangeScenaryRotation(const float _rotationX, const float _rotationY, const float _rotationZ){
  ui->scenary->ChangeScenaryRotation(_rotationX, _rotationY, _rotationZ);
}

void Console::ChangeScenaryZoom(const float _zoom){
  ui->scenary->ChangeScenaryZoom(_zoom);
}

void Console::setTopView(){
  ui->scenary->setTopView();
}

void Console::setStandardView(){
  ui->scenary->setStandardView();
}

QFrame *const Console::innkeeper(){
  return host;
}

void Console::updateData(){
  if(onGUI != previousOnGUI)
    hideShowGUIElements();
  updateVehicleTransformationMatrix();
  if(controls != nullptr)
    if(previousDataset != controls->getDataset()){
      previousDataset = controls->getDataset();
      ui->scenary->streets->restartData();
    }
  ui->scenary->nextFrame();

  if(onGUI){
    for(int i = 0; i < simpleElements.size(); i++)
      if(simpleElements[i] != nullptr)
        simpleElements[i]->update();
    for(int i = 0; i < speedos.size(); i++)
      if(speedos[i] != nullptr)
        speedos[i]->update();
    for(int i = 0; i < gearboxes.size(); i++)
      if(gearboxes[i] != nullptr)
        gearboxes[i]->update();
    for(int i = 0; i < textlists.size(); i++)
      if(textlists[i] != nullptr)
        textlists[i]->update();
    for(int i = 0; i < iconlists.size(); i++)
      if(iconlists[i] != nullptr)
        iconlists[i]->update();
  }

  for(int i = 0; i < clocks.size(); i++)
    if(clocks[i] != nullptr)
      clocks[i]->update();
  for(int i = 0; i < compasses.size(); i++)
    if(compasses[i] != nullptr)
      compasses[i]->update();
  for(int i = 0; i < labels.size(); i++)
    if(labels[i] != nullptr)
      labels[i]->updateLabels();

  if(*frameLatitude != previousFrameLatitude || *frameLongitude != previousFrameLongitude){
    previousFrameLatitude = *frameLatitude;
    previousFrameLongitude = *frameLongitude;
    emit frameChanged();
    updateAllTrajectories();
  }
}

void Console::on_b_hop_released(){
  onMenu = !onMenu;
  if(onMenu){
    ui->b_tv->move(10, 0);
    ui->b_sv->move(10, 55);
    ui->b_op->move(10, 110);
    ui->b_fvw->move(10, 165);
    ui->b_svw->move(10, 220);
    ui->b_zoomin->move(10, 275);
    ui->b_zoomout->move(10, 330);
    ui->b_hop->setStyleSheet(btnCss.arg("top").arg("4"));
  }else{
    ui->b_tv->move(-100, 0);
    ui->b_sv->move(-100, 55);
    ui->b_op->move(-100, 110);
    ui->b_fvw->move(-100, 165);
    ui->b_svw->move(-100, 220);
    ui->b_zoomin->move(-100, 275);
    ui->b_zoomout->move(-100, 330);
    ui->b_hop->setStyleSheet(btnCss.arg("bottom").arg("4"));
  }
}

void Console::on_b_frame_released(){
  onFrameCoordinates = !onFrameCoordinates;
  if(!onFrameCoordinates)
    ui->b_frame->setStyleSheet(onCss.arg("bottom"));
  else
    ui->b_frame->setStyleSheet(onCss.arg("top"));
  ui->scenary->update();
}

void Console::on_b_cameras_released(){
  onCamera = !onCamera;
  if(!onCamera)
    ui->b_cameras->setStyleSheet(onCss.arg("bottom"));
  else
    ui->b_cameras->setStyleSheet(onCss.arg("top"));
  hideShowCameras();
}

void Console::on_b_gui_released(){
  onGUI = !onGUI;
  if(!onGUI)
    ui->b_gui->setStyleSheet(onCss.arg("bottom"));
  else
    ui->b_gui->setStyleSheet(onCss.arg("top"));
  hideShowGUIElements();
}

// ------------------------------------------------ OSM Files installation ------------------------------------------------ //

void Console::on_b_install_released(){
  if(!connected){
    connect(installator, SIGNAL(osmFilesFound(uint)), this, SLOT(osmFilesFoundText(uint)));
    connect(installator, SIGNAL(osmReadingPercentage(uint)), this, SLOT(updatePercentage(uint)));
    connect(installator, SIGNAL(allFilesInstalled()), this, SLOT(allFilesInstalledActuator()));
    connect(installator, SIGNAL(osmFileError(QString,QString)), this, SLOT(osmFilesErrorFound(QString,QString)));
    connect(installator, SIGNAL(movementFeeling()), this, SLOT(rotateInstallingImage()));
    connected = true;
  }

  if(!installator->occupied){
    ui->scenary->streets->mapping->stopMapLoading();

    ui->lb_installmaps->setText("No OSM Files found.");
    ui->lb_installmaps->show();
    ui->lb_installmaps_2->hide();
    ui->lb_percentage->hide();
    ui->lb_errors->hide();
    ui->bck_percentage->hide();

    installator->installMaps();
  }
}

void Console::osmFilesFoundText(uint _quantity){
  if(_quantity > 0){
    ui->bck_icon->show();
    ui->lb_installmaps_2->show();
    ui->lb_percentage->show();
    ui->bck_percentage->show();
    ui->lb_installmaps->setText(QString("OSM Files found: %1").arg(_quantity));
    ui->lb_installmaps_2->setText("INSTALLING...");
    ui->lb_installmaps_2->setStyleSheet(insCss.arg("center").arg("#FF4040"));
    ui->lb_percentage->setText("0%");
  }
}

void Console::allFilesInstalledActuator(){
  ui->bck_icon->hide();
  ui->lb_installmaps_2->setText("SUCCESS...");
  ui->lb_installmaps_2->setStyleSheet(insCss.arg("top").arg("#78d7e1"));
}

void Console::updatePercentage(uint _percentage){
  if(_percentage < 100)
    ui->bck_icon->show();
  ui->lb_percentage->show();
  ui->bck_percentage->show();
  ui->lb_errors->hide();

  qreal angle = _percentage/100.0f * 360;
  QPainterPath pth;
  pth.moveTo(100, 50);
  pth.arcTo(0, 0, 100, 100, 0, angle);
  pth.arcTo(10, 10, 80, 80, angle, -angle);
  pth.lineTo(100, 50);

  bckPer.fill(Qt::transparent);
  ptr.begin(&bckPer);
  ptr.setRenderHint(QPainter::Antialiasing);
  ptr.fillPath(pth, QBrush(gradient));
  ptr.end();
  ui->bck_percentage->setPixmap(bckPer);

  ui->lb_percentage->setText(QString("%1%").arg(_percentage));
}

void Console::osmFilesErrorFound(QString _fileName, QString _errorText){
  ui->bck_icon->hide();
  ui->lb_installmaps_2->setText("ERROR...");
  ui->lb_installmaps_2->setStyleSheet(insCss.arg("bottom").arg("#FF4040"));
  ui->lb_percentage->hide();
  ui->bck_percentage->hide();
  ui->lb_errors->show();
  ui->lb_errors->setText("Errors:<br/>- <i>File:</i> <span style='color:#E6B8B8'>" + _fileName +
                         "</span> <br/>- <i>Description:</i> <span style='color:#E6B8B8'>" + _errorText + "</span>");
}

void Console::rotateInstallingImage(){
  bckIcon2.fill(Qt::transparent);
  ptr.begin(&bckIcon2);
  ptr.setRenderHint(QPainter::SmoothPixmapTransform);
  ptr.translate(12, 15);
  iconAngle += 20;
  ptr.rotate(iconAngle);
  ptr.translate(-12,-15);
  ptr.drawPixmap(0, 0, 24, 30, bckIcon);
  ptr.end();
  ui->bck_icon->setPixmap(bckIcon2);
}

void Console::on_b_zoomin_released(){
  ui->scenary->zooming();
}

void Console::on_b_zoomout_released(){
  ui->scenary->zooming(false);
}

void Console::updateROSdata(){
#ifdef ROS_DATA
  if(ros::ok()){
    ros::spinOnce();
    updateData();
  }
#endif
}
