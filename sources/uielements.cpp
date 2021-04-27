#include "headers/uielements.h"

NewSimpleElement::NewSimpleElement(QWidget *_parent, const int _type, const QString _name, const int _decimals,
                                   const double _max, const double _min, double *_input, QVector<QFont*> *_fonts,
                                   const float _positionX, const float _positionY, const float _horizontalAlignment,
                                   const float _verticalAlignment) :
  QFrame(_parent),
  parent(_parent),
  type(_type),
  positionX(_positionX),
  positionY(_positionY),
  relativePositionX(_positionX),
  relativePositionY(_positionY),
  horizontalAlignment(_horizontalAlignment),
  verticalAlignment(_verticalAlignment),
  decimals(_decimals),
  max(_max),
  min(_min),
  input(_input),
  name(_name),
  labels(0),
  fonts(_fonts),
  x_steering(":/images/i_steering"),
  o_steering(82, 82),
  a_steering(0),
  v_bar(0)
{
  switch(type){
  case STEERING:
    width = 110;
  break;
  case BATTERY:
    width = 60;
  break;
  default:
    width = 70;
  break;
  }
  height = 150;

  if(horizontalAlignment == RIGHT)
    relativePositionX = _parent->width() - width - positionX;
  else if(horizontalAlignment == CENTER)
    relativePositionX = _parent->width()/2.0f - width/2.0f - positionX;

  if(verticalAlignment == BOTTOM)
    relativePositionY = _parent->height() - height - positionY;
  else if(verticalAlignment == CENTER)
    relativePositionY = _parent->height()/2.0f - height/2.0f - positionY;

  setElement();
}

NewSimpleElement::~NewSimpleElement(){
  for(int i = 0; i < labels.size(); i++)
    delete labels[i];
  labels.clear();
}

void NewSimpleElement::setElement(){
  this->resize(width, height);
  this->move(relativePositionX, relativePositionY);
  this->setFrameShape(QFrame::NoFrame);
  this->setLineWidth(0);
  this->setStyleSheet(".QLabel{ color:#78d7e1; }");

  switch(type){
  case STEERING:
    setSteering();
  break;
  case BATTERY:
    setBattery();
  break;
  default:
    setVerticalBar();
  break;
  }
  connect(parent->parentWidget(), SIGNAL(resizing()), this, SLOT(setPosition()));
  this->show();
}

void NewSimpleElement::setSteering(){
  QLabel *l_name = new QLabel(name, this);
  l_name->setAlignment(Qt::AlignHCenter);
  modLabel(l_name, 0, 5, width, 20, 0);

  QLabel *l_input = new QLabel(this);
  l_input->setAlignment(Qt::AlignHCenter);
  modLabel(l_input, 0, 112, width, 25, 2, "color:#9bd1ff;");

  QLabel *l_background = new QLabel(this);
  modLabel(l_background, 14, 28, 82, 82, -1);

  QLabel *l_indicator = new QLabel(this);
  modLabel(l_indicator, 14, 28, 82, 82, -1, "background:url(\":/images/i_isteering\") no-repeat");

  update();
}

void NewSimpleElement::setBattery(){
  QLabel *l_name = new QLabel(name, this);
  l_name->setAlignment(Qt::AlignHCenter);
  modLabel(l_name, 0, 5, width, 20, 0);

  QLabel *l_input = new QLabel(QString("%1").arg(*input, 1, 'f', decimals, '0'), this);
  l_input->setAlignment(Qt::AlignRight);
  modLabel(l_input, 0, 112, 37, 35, 2, "color:#9bd1ff;");

  QLabel *l_percentage = new QLabel("%", this);
  modLabel(l_percentage, 39, 113, 21, 20, 0, "color:#5eb5ff;");

  QLabel *l_background = new QLabel(this);

  int newSize = (*input - min)/(max - min) * 68;
  QLabel *l_level = new QLabel(this);

  if(*input < 15){
    modLabel(l_background, 8, 25, 43, 84, -1, "background:url(\":/images/i_battery\") right bottom no-repeat");
    modLabel(l_level, 13, 104 - newSize, 33, newSize, -1, "background:url(\":/images/i_lvlbattery\") right bottom no-repeat");
  }else{
    modLabel(l_background, 8, 25, 43, 84, -1, "background:url(\":/images/i_battery\") left bottom no-repeat");
    modLabel(l_level, 13, 104 - newSize, 33, newSize, -1, "background:url(\":/images/i_lvlbattery\") left bottom no-repeat");
  }
}

void NewSimpleElement::setVerticalBar(){
  QLabel *l_name = new QLabel(name, this);
  modLabel(l_name, 0, 5, width, 20, 0);

  QLabel *l_max = new QLabel(QString("%1").arg(max, 1, 'f', decimals, '0'), this);
  l_max->setAlignment(Qt::AlignRight);
  modLabel(l_max, 0, 26, 30, 20, 1);

  QLabel *l_med = new QLabel(QString("%1").arg((max - min)/2.0f, 1, 'f', decimals, '0'), this);
  l_med->setAlignment(Qt::AlignRight);
  modLabel(l_med, 0, 64, 30, 20, 1);

  QLabel *l_min = new QLabel(QString("%1").arg(min, 1, 'f', decimals, '0'), this);
  l_min->setAlignment(Qt::AlignRight);
  modLabel(l_min, 0, 102, 30, 20, 1);

  QLabel *l_input = new QLabel(QString("%1").arg(*input, 1, 'f', decimals, '0'), this);
  modLabel(l_input, 35, 110, width, 35, 2, "color:#9bd1ff;");

  QLabel *l_background = new QLabel(this);
  modLabel(l_background, 33, 28, 28, 84, -1, "background:url(\":/images/i_vBar\") no-repeat");

  int newSize = (*input - min)/(max - min) * 76;
  QLabel *l_level = new QLabel(this);
  modLabel(l_level, 46, 108 - newSize, 11, newSize, -1, "background:url(\":/images/i_lvlVBar\") left bottom no-repeat");
}

void NewSimpleElement::modLabel(QLabel *lbl, int x, int y, int w, int h, int i, QString s){
  lbl->move(x, y);
  lbl->resize(w, h);
  if(i >= 0)
    lbl->setFont(*fonts->at(i));
  if(s != NULL)
    lbl->setStyleSheet(s);
  labels.append(lbl);
}

void NewSimpleElement::changeName(const QString _name){
  labels[0]->setText(_name);
}

void NewSimpleElement::changeType(const int _type){
  for(int i = 0; i < labels.size(); i++)
    delete labels[i];
  labels.clear();

  type = _type;

  switch(type){
  case STEERING:
    width = 110;
    setSteering();
  break;
  case BATTERY:
    width = 80;
    setBattery();
  break;
  default:
    width = 70;
    setVerticalBar();
  break;
  }
  this->resize(width, height);
  setPosition();

  update();
}

void NewSimpleElement::changeInput(double *_input){
  input = _input;
  update();
}

void NewSimpleElement::changePosition(const float _positionX, const float _positionY){
  if(_positionX == _positionX)
    positionX = _positionX;
  if(_positionY == _positionY)
    positionY = _positionY;
  setPosition();
}

void NewSimpleElement::changeAlignment(const float _horizontalAlignment, const float _verticalAlignment){
  if(_horizontalAlignment == _horizontalAlignment)
    horizontalAlignment = _horizontalAlignment;
  if(_verticalAlignment == _verticalAlignment)
    verticalAlignment = _verticalAlignment;
  setPosition();
}

void NewSimpleElement::changeMaxMin(const double _max, const double _min){
  if(_max == _max)
    max = _max;
  if(_min == _min)
    min = _min;
  float med = (max - min) / 2.0f;
  labels[1]->setText(QString("%1").arg(max, 1, 'f', decimals, '0'));
  labels[2]->setText(QString("%1").arg(med, 1, 'f', decimals, '0'));
  labels[3]->setText(QString("%1").arg(min, 1, 'f', decimals, '0'));
  update();
}

void NewSimpleElement::update(){
  int newSize;

  switch(type){
  case STEERING:
    a_steering += (*input - a_steering)/3.0f;
//    a_steering += (*input * toDEGREES - a_steering)/3.0f;
    labels[1]->setText(QString("%1°").arg(a_steering, 1, 'f', decimals, '0'));
    //a_steering = *input;
    o_steering.fill(Qt::transparent);
    ptr.begin(&o_steering);
    ptr.setRenderHint(QPainter::SmoothPixmapTransform);
    ptr.translate(41,41);
    ptr.rotate(a_steering);
    ptr.translate(-41,-41);
    ptr.drawPixmap(0, 0, x_steering);
    ptr.end();
    labels[2]->setPixmap(o_steering);
  break;
  case BATTERY:
    labels[1]->setText(QString("%1").arg(*input, 1, 'f', decimals, '0'));
    if(*input < 15){
      labels[3]->setStyleSheet("background:url(\":/images/i_battery\") right bottom no-repeat");
      labels[4]->setStyleSheet("background:url(\":/images/i_lvlbattery\") right bottom no-repeat");
    }else{
      labels[3]->setStyleSheet("background:url(\":/images/i_battery\") left bottom no-repeat");
      labels[4]->setStyleSheet("background:url(\":/images/i_lvlbattery\") left bottom no-repeat");
    }
    newSize = (*input - min)/(max - min) * 68;
    labels[4]->resize(33, newSize);
    labels[4]->move(13, 104 - newSize);
  break;
  default:
    v_bar += (*input - v_bar)/3.0f;
    newSize = (v_bar - min)/(max - min) * 76;
    labels[6]->move(46, 108 - newSize);
    labels[6]->resize(11, newSize);
    labels[4]->setText(QString("%1").arg(v_bar, 1, 'f', decimals, '0'));
  break;
  }
}

void NewSimpleElement::setPosition(){
  if(horizontalAlignment == RIGHT)
    relativePositionX = parent->width() - width - positionX;
  else if(horizontalAlignment == CENTER)
    relativePositionX = parent->width()/2.0f - width/2.0f - positionX;
  else
    relativePositionX = positionX;
  if(verticalAlignment == BOTTOM)
    relativePositionY = parent->height() - height - positionY;
  else if(verticalAlignment == CENTER)
    relativePositionY = parent->height()/2.0f - height/2.0f - positionY;
  else
    relativePositionY = positionY;

  this->move(relativePositionX, relativePositionY);
}

//-------------------------------------------------------------------------------------------------------------//
//                                          speedometer UI element                                             //
//-------------------------------------------------------------------------------------------------------------//

NewSpeedo::NewSpeedo(QWidget *_parent, const QString _name, const double _maxSpeed,
                     const double _minSpeed, const double _maxRPM, const double _minRPM,
                     double *_inputSpeed, double *_inputRPM, QVector<QFont*> *_fonts,
                     const float _positionX, const float _positionY, const float _horizontalAlignment,
                     const float _verticalAlignment) :
  QFrame(_parent),
  parent(_parent),
  positionX(_positionX),
  positionY(_positionY),
  relativePositionX(_positionX),
  relativePositionY(_positionY),
  width(125),
  height(150),
  horizontalAlignment(_horizontalAlignment),
  verticalAlignment(_verticalAlignment),
  maxSpeed(_maxSpeed),
  minSpeed(_minSpeed),
  maxRPM(_maxRPM),
  minRPM(_minRPM),
  inputSpeed(_inputSpeed),
  inputRPM(_inputRPM),
  name(_name),
  labels(0),
  fonts(_fonts),
  x_speed(90, 90),
  x_rpm(44, 44),
  i_speed(":/images/i_ispeed"),
  i_rpm(":/images/i_irpm"),
  a_speed(0),
  a_rpm(0)
{
  if(horizontalAlignment == RIGHT)
    relativePositionX = _parent->width() - width - positionX;
  else if(horizontalAlignment == CENTER)
    relativePositionX = _parent->width()/2.0f - width/2.0f - positionX;
  if(verticalAlignment == BOTTOM)
    relativePositionY = _parent->height() - height - positionY;
  else if(verticalAlignment == CENTER)
    relativePositionY = _parent->height()/2.0f - height/2.0f - positionY;

  relationSpeed = 180.0f/(maxSpeed - minSpeed);
  relationRPM = 240.0f/(maxRPM - minRPM);

  setElement();
}

NewSpeedo::~NewSpeedo(){
  for(int i = 0; i < labels.size(); i++)
    delete labels[i];
  labels.clear();
}

void NewSpeedo::setElement(){
  this->resize(width, height);
  this->move(relativePositionX, relativePositionY);
  this->setFrameShape(QFrame::NoFrame);
  this->setLineWidth(0);
  this->setStyleSheet(".QLabel{ color:#78d7e1; }");

  QLabel *l_name = new QLabel(name, this);
  l_name->setAlignment(Qt::AlignHCenter);
  modLabel(l_name, 0, 5, 125, 20, 0);

  QLabel *l_max = new QLabel(QString("%1").arg(maxSpeed, 1, 'f', 0, '0'), this);
  modLabel(l_max, 95, 46, 30, 20, 1);

  QLabel *l_min = new QLabel(QString("%1").arg(minSpeed, 1, 'f', 0, '0'), this);
  modLabel(l_min, 0, 105, 25, 20, 1);

  QLabel *l_inputSpeed = new QLabel(this);
  l_inputSpeed->setAlignment(Qt::AlignRight);
  modLabel(l_inputSpeed, 10, 100, 61, 20, 2, "color:#9bd1ff;");

  QLabel *l_inputRPM = new QLabel(this);
  l_inputRPM->setAlignment(Qt::AlignRight);
  modLabel(l_inputRPM, 1, 115, 70, 20, 2, "color:#8becff;");

  QLabel *l_Speed = new QLabel("KM / H", this);
  modLabel(l_Speed, 74, 102, 74, 20, 0, "color:#5eb5ff;");

  QLabel *l_RPM = new QLabel("R P M", this);
  modLabel(l_RPM, 74, 116, 74, 20, 0, "color:#5eb5ff;");

  QLabel *l_background = new QLabel(this);
  modLabel(l_background, 0, 12, 125, 100, -1, "background:url(\":/images/i_speedo\") no-repeat");

  QLabel *l_lspeed = new QLabel(this);
  modLabel(l_lspeed, 4, 37, 90, 90);

  QLabel *l_lrpm = new QLabel(this);
  modLabel(l_lrpm, 78, 69, 44, 44);

  connect(parent->parentWidget(), SIGNAL(resizing()), this, SLOT(setPosition()));
  this->show();
  update();
}

void NewSpeedo::modLabel(QLabel *lbl, int x, int y, int w, int h, int i, QString s){
  lbl->move(x, y);
  lbl->resize(w, h);
  if(i >= 0)
    lbl->setFont(*fonts->at(i));
  if(s != NULL)
    lbl->setStyleSheet(s);
  labels.append(lbl);
}

void NewSpeedo::changeName(const QString _name){
  labels[0]->setText(_name);
}

void NewSpeedo::changeInputs(double *_inputSpeed, double *_inputRPM){
  if(_inputSpeed != nullptr)
    inputSpeed = _inputSpeed;
  if(_inputRPM != nullptr)
    inputRPM = _inputRPM;
  update();
}

void NewSpeedo::changePosition(const float _positionX, const float _positionY){
  if(_positionX == _positionX)
    positionX = _positionX;
  if(_positionY == _positionY)
    positionY = _positionY;
  setPosition();
}

void NewSpeedo::changeAlignment(const float _horizontalAlignment, const float _verticalAlignment){
  if(_horizontalAlignment == _horizontalAlignment)
    horizontalAlignment = _horizontalAlignment;
  if(_verticalAlignment == _verticalAlignment)
    verticalAlignment = _verticalAlignment;
  setPosition();
}

void NewSpeedo::changeMaxMin(const double _maxSpeed, const double _minSpeed, const double _maxRPM, const double _minRPM){
  if(_maxSpeed == _maxSpeed)
    maxSpeed = _maxSpeed;
  if(_minSpeed == _minSpeed)
    minSpeed = _minSpeed;
  if(_maxRPM == _maxRPM)
    maxRPM = _maxRPM;
  if(_minRPM == _minRPM)
    minRPM = _minRPM;
  labels[1]->setText(QString("%1").arg(maxSpeed, 3, 'f', 2, '0'));
  labels[2]->setText(QString("%1").arg(minSpeed, 3, 'f', 2, '0'));
  relationSpeed = 180.0f/(maxSpeed - minSpeed);
  relationRPM = 240.0f/(maxRPM - minRPM);
  update();
}

void NewSpeedo::update(){
  labels[3]->setText(QString("%1").arg(*inputSpeed, 3, 'f', 2, '0'));
  labels[4]->setText(QString("%1").arg(*inputRPM, 3, 'f', 2, '0'));

  a_speed += ((*inputSpeed - minSpeed) * relationSpeed - 30 - a_speed)/3.0f;

  x_speed.fill(Qt::transparent);
  ptr.begin(&x_speed);
  ptr.setRenderHint(QPainter::SmoothPixmapTransform);
  ptr.translate(45,45);
  ptr.rotate(a_speed);
  ptr.translate(-45,-45);
  ptr.drawPixmap(0, 43, i_speed);
  ptr.end();
  labels[8]->setPixmap(x_speed);

  a_rpm += ((*inputRPM - minRPM) * relationRPM - 30 - a_rpm)/3.0f;

  x_rpm.fill(Qt::transparent);
  ptr.begin(&x_rpm);
  ptr.setRenderHint(QPainter::SmoothPixmapTransform);
  ptr.translate(22,22);
  ptr.rotate(a_rpm);
  ptr.translate(-22,-22);
  ptr.drawPixmap(0, 18, i_rpm);
  ptr.end();
  labels[9]->setPixmap(x_rpm);
}

void NewSpeedo::setPosition(){
  if(horizontalAlignment == RIGHT)
    relativePositionX = parent->width() - width - positionX;
  else if(horizontalAlignment == CENTER)
    relativePositionX = parent->width()/2.0f - width/2.0f - positionX;
  else
    relativePositionX = positionX;
  if(verticalAlignment == BOTTOM)
    relativePositionY = parent->height() - height - positionY;
  else if(verticalAlignment == CENTER)
    relativePositionY = parent->height()/2.0f - height/2.0f - positionY;
  else
    relativePositionY = positionY;

  this->move(relativePositionX, relativePositionY);
}

//-------------------------------------------------------------------------------------------------------------//
//                                            gearbox UI element                                               //
//-------------------------------------------------------------------------------------------------------------//

NewGearbox::NewGearbox(QWidget *_parent, const int _type, const QString _name, vector<nameANDvalue> *_values,
                       double *_input, QVector<QFont*> *_fonts, const float _positionX, const float _positionY,
                       const float _horizontalAlignment, const float _verticalAlignment) :
  QFrame(_parent),
  parent(_parent),
  type(_type),
  positionX(_positionX),
  positionY(_positionY),
  relativePositionX(_positionX),
  relativePositionY(_positionY),
  width(65),
  height(150),
  horizontalAlignment(_horizontalAlignment),
  verticalAlignment(_verticalAlignment),
  input(_input),
  name(_name),
  labelValues(_values),
  labels(0),
  fonts(_fonts)
{

  if(horizontalAlignment == RIGHT)
    relativePositionX = _parent->width() - width - positionX;
  else if(horizontalAlignment == CENTER)
    relativePositionX = _parent->width()/2.0f - width/2.0f - positionX;

  if(verticalAlignment == BOTTOM)
    relativePositionY = _parent->height() - height - positionY;
  else if(verticalAlignment == CENTER)
    relativePositionY = _parent->height()/2.0f - height/2.0f - positionY;

  setElement();
}

NewGearbox::~NewGearbox(){
  for(int i = 0; i < labels.size(); i++)
    delete labels[i];
  labels.clear();
}

void NewGearbox::setElement(){
  this->resize(width, height);
  this->move(relativePositionX, relativePositionY);
  this->setFrameShape(QFrame::NoFrame);
  this->setLineWidth(0);
  this->setStyleSheet(".QLabel{ color:#78d7e1; }");

  switch(type){
  case MANUAL:
    setManual();
  break;
  default:
    setAutomatic();
  break;
  }
  connect(parent->parentWidget(), SIGNAL(resizing()), this, SLOT(setPosition()));
  this->show();
}

void NewGearbox::setManual(){
  QLabel *l_name = new QLabel(name, this);
  l_name->setAlignment(Qt::AlignHCenter);
  modLabel(l_name, 0, 5, width, 20, 0);

  QLabel *l_background = new QLabel(this);
  modLabel(l_background, 7, 28, 50, 84, -1, "background:url(\":/images/i_gearbox\") left bottom no-repeat");

  int sLV = ceil(labelValues->size()/2.0f) - 1;
  int ctr = 0;
  verticalSeparation = 61.0f/sLV;
  sLV = 0;
  int posX = 0, posY = 0, pX = 0, pY = 0;
  QString lab_input, iCss;

  for(int i = 0; i < labelValues->size(); i++){
    QString tCss;
    if(ctr == 0)
      tCss = "  ";
    else
      tCss = "      ";
    QLabel *l_step = new QLabel(tCss + labelValues->at(i).label, this);
    l_step->setAlignment(Qt::AlignVCenter);
    if(i == 0)
      tCss = "color:#333; background:url(\":/images/i_manualGearbox\") left top no-repeat";
    else if((i == (labelValues->size() - 1)) && (ctr == 0))
      tCss = "color:#333; background:url(\":/images/i_manualGearbox\") center top no-repeat";
    else if((i == (labelValues->size() - 2)) && (ctr == 0))
      tCss = "color:#333; background:url(\":/images/i_manualGearbox\") center top no-repeat";
    else if((i == (labelValues->size() - 1)) && (ctr == 1))
      tCss = "color:#333; background:url(\":/images/i_manualGearbox\") right bottom no-repeat";
    else if((i == 1) && (ctr == 1))
      tCss = "color:#333; background:url(\":/images/i_manualGearbox\") right top no-repeat";
    else if(ctr == 0)
      tCss = "color:#333; background:url(\":/images/i_manualGearbox\") left bottom no-repeat";
    else if(ctr == 1)
      tCss = "color:#333; background:url(\":/images/i_manualGearbox\") center bottom no-repeat";

    pX = 7 + ctr * 25;
    pY = 28 + sLV * verticalSeparation;
    if(labelValues->at(i).value == *input){
      posX = pX;
      posY = pY;
      lab_input = labelValues->at(i).label;
      if(ctr == 0)
        iCss = "background:url(\":/images/i_pivotGear\") left center no-repeat";
      else
        iCss = "background:url(\":/images/i_pivotGear\") right center no-repeat";
    }

    modLabel(l_step, pX, pY, 25, 21, 3, tCss);
    sLV = (ctr == 1)? sLV + 1 : sLV;
    ctr = (ctr > 0)? 0 : 1;
  }

  QLabel *l_place = new QLabel(this);
  modLabel(l_place, posX, posY, 25, 21, -1, iCss);

  QLabel *l_gear = new QLabel(lab_input, this);
  l_gear->setAlignment(Qt::AlignHCenter);
  modLabel(l_gear, 0, 113, width, 20, 2, "color:#9bd1ff;");
}

void NewGearbox::setAutomatic(){
  QLabel *l_name = new QLabel(name, this);
  l_name->setAlignment(Qt::AlignHCenter);
  modLabel(l_name, 0, 5, width, 20, 0);

  QLabel *l_background = new QLabel(this);
  modLabel(l_background, 7, 28, 50, 84, -1, "background:url(\":/images/i_gearbox\") right bottom no-repeat");

  int sLV = labelValues->size() - 1;
  int ps = 0;
  verticalSeparation = 55/sLV;
  for(int i = 0; i < labelValues->size(); i++){
    QLabel *l_step = new QLabel(" " + labelValues->at(i).label, this);
    modLabel(l_step, 38, 35 + i * verticalSeparation, 19, 15, 3, "background:url(\":/images/i_gearbox\") right bottom no-repeat");
    if(labelValues->at(i).value == *input)
      ps = i;
  }

  QLabel *l_place = new QLabel(this);
  modLabel(l_place, 11, 36 + verticalSeparation * ps, 26, 9, -1, "background:url(\":/images/i_igearbox\") no-repeat");

  QLabel *l_gear = new QLabel(labelValues->at(ps).label, this);
  l_gear->setAlignment(Qt::AlignHCenter);
  modLabel(l_gear, 0, 113, width, 20, 2, "color:#9bd1ff;");
}

void NewGearbox::modLabel(QLabel *lbl, int x, int y, int w, int h, int i, QString s){
  lbl->move(x, y);
  lbl->resize(w, h);
  if(i >= 0)
    lbl->setFont(*fonts->at(i));
  if(s != NULL)
    lbl->setStyleSheet(s);
  labels.append(lbl);
}

void NewGearbox::changeName(const QString _name){
  labels[0]->setText(_name);
}

void NewGearbox::changeType(const int _type){
  for(int i = 0; i < labels.size(); i++)
    delete labels[i];
  labels.clear();

  type = _type;

  switch(type){
  case MANUAL:
    setManual();
  break;
  default:
    setAutomatic();
  break;
  }
}

void NewGearbox::changeInput(double *_input){
  input = _input;
  update();
}

void NewGearbox::changePosition(const float _positionX, const float _positionY){
  if(_positionX == _positionX)
    positionX = _positionX;
  if(_positionY == _positionY)
    positionY = _positionY;
  setPosition();
}

void NewGearbox::changeAlignment(const float _horizontalAlignment, const float _verticalAlignment){
  if(_horizontalAlignment == _horizontalAlignment)
    horizontalAlignment = _horizontalAlignment;
  if(_verticalAlignment == _verticalAlignment)
    verticalAlignment = _verticalAlignment;
  setPosition();
}

void NewGearbox::changeLabels(vector<nameANDvalue> *_values){
  if(_values != nullptr){
    labelValues->clear();
    labelValues = _values;
  }
  changeType(type);
}

void NewGearbox::update(){
  int sLV = 0, ctr = 0;
  switch(type){
  case MANUAL:
    for(int i = 0; i < labelValues->size(); i++){
      if(labelValues->at(i).value == *input){
        labels[labels.size() - 1]->setText(labelValues->at(i).label);
        if(ctr == 0)
          labels[labels.size() - 2]->setStyleSheet("background:url(\":/images/i_pivotGear\") left center no-repeat");
        else
          labels[labels.size() - 2]->setStyleSheet("background:url(\":/images/i_pivotGear\") right center no-repeat");
        labels[labels.size() - 2]->move(7 + ctr * 25, 28 + sLV * verticalSeparation);
      }
      sLV = (ctr == 1)? sLV + 1 : sLV;
      ctr = (ctr > 0)? 0 : 1;
    }
  break;
  default:
    for(int i = 0; i < labelValues->size(); i++){
      if(labelValues->at(i).value == *input){
        labels[labels.size() - 1]->setText(labelValues->at(i).label);
        labels[labels.size() - 2]->move(11, 36 + i * verticalSeparation);
      }
    }
  break;
  }
}

void NewGearbox::setPosition(){
  if(horizontalAlignment == RIGHT)
    relativePositionX = parent->width() - width - positionX;
  else if(horizontalAlignment == CENTER)
    relativePositionX = parent->width()/2.0f - width/2.0f - positionX;
  else
    relativePositionX = positionX;
  if(verticalAlignment == BOTTOM)
    relativePositionY = parent->height() - height - positionY;
  else if(verticalAlignment == CENTER)
    relativePositionY = parent->height()/2.0f - height/2.0f - positionY;
  else
    relativePositionY = positionY;

  this->move(relativePositionX, relativePositionY);
}

//-------------------------------------------------------------------------------------------------------------//
//                                           text list UI element                                              //
//-------------------------------------------------------------------------------------------------------------//

NewTextList::NewTextList(QWidget *_parent, const QString _name, vector<nameANDdata> *_values, QVector<QFont*> *_fonts, const float _positionX,
                         const float _positionY, const float _width, const float _height,
                         const float _horizontalAlignment, const float _verticalAlignment) :
  QFrame(_parent),
  parent(_parent),
  positionX(_positionX),
  positionY(_positionY),
  relativePositionX(_positionX),
  relativePositionY(_positionY),
  horizontalAlignment(_horizontalAlignment),
  verticalAlignment(_verticalAlignment),
  width2(_width),
  height2(_height),
  name(_name),
  labelValues(_values),
  labels(0),
  fonts(_fonts)
{
  setAbsoluteSize();
  setElement();
}

NewTextList::~NewTextList(){
  for(int i = 0; i < labels.size(); i++)
    delete labels[i];
  labels.clear();
  delete f_container;
  delete s_container;
}

void NewTextList::setElement(){
  QString css = ".QScrollBar:vertical{"
                "border:0;"
                "background:rgba(255,255,255,0.2);"
                "width: 13px;"
                "margin: 15px 0 15px 0;"
                "border-radius:6px;"
                "}"
                "QScrollBar::handle:vertical{"
                "background:rgba(160,160,160,1);"
                "border:0;"
                "border-radius:6px;"
                "min-height:25px;"
                "}"
                "QScrollBar::add-line:vertical{"
                "border:0;"
                "background:none;"
                "height: 13px;"
                "subcontrol-position: bottom;"
                "subcontrol-origin: margin;"
                "}"
                "QScrollBar::sub-line:vertical{"
                "border:0;"
                "background:none;"
                "height:13px;"
                "subcontrol-position: top;"
                "subcontrol-origin: margin;"
                "}"
                "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
                "background: none;"
                "}"
                "QScrollBar::up-arrow:vertical{"
                "image: url(:/images/i_up);"
                "}"
                "QScrollBar::down-arrow:vertical{"
                "image: url(:/images/i_down);"
                "}"
                ".QLabel{ color:#78d7e1; }";

  this->resize(width, height);
  this->move(relativePositionX, relativePositionY);
  this->setFrameShape(QFrame::NoFrame);
  this->setLineWidth(0);
  this->setStyleSheet(css);

  setList();
  connect(parent->parentWidget(), SIGNAL(resizing()), this, SLOT(setPosition()));
  this->show();
}
void NewTextList::setList(){
  QLabel *l_name = new QLabel(name, this);
  modLabel(l_name, 0, 5, width, 20, 0);

  f_container = new QFrame(this);
  f_container->setStyleSheet(".QFrame{ border:none; background:none; }");
  f_container->resize(width, 23 * labelValues->size());

  s_container = new QScrollArea(this);
  s_container->setStyleSheet(".QScrollArea{ border:2px solid #4eb4ff; background:rgba(0,0,0,0.4); border-radius:7px; }");
  s_container->move(0, 28);
  s_container->resize(width, height - 45);
  s_container->setWidget(f_container);
  s_container->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  for(int i = 0; i < labelValues->size(); i++){
    QLabel *l_step = new QLabel(labelValues->at(i).label + "  ", f_container);
    l_step->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    modLabel(l_step, 0, 23 * i, 60, 23, 4, "border:1px solid #314d60; border-top:0; border-left:0;");
  }

  for(int i = 0; i < labelValues->size(); i++){
    QLabel *l_step2 = new QLabel(QString("  %1%2").arg(*labelValues->at(i).input, 2, 'f',
                                                       labelValues->at(i).decimal_places, '0')
                                 .arg(labelValues->at(i).label_termination), f_container);
    l_step2->setAlignment(Qt::AlignVCenter);
    modLabel(l_step2, 60, 23 * i, width - 64, 23, 5, "color:#5eb5ff; border:1px solid #314d60; border-top:0; border-left:0; border-right:0;");
  }
}

void NewTextList::modLabel(QLabel *lbl, int x, int y, int w, int h, int i, QString s){
  lbl->move(x, y);
  lbl->resize(w, h);
  if(i >= 0)
    lbl->setFont(*fonts->at(i));
  if(s != NULL)
    lbl->setStyleSheet(s);
  labels.append(lbl);
}

void NewTextList::setAbsoluteSize(){
  if(width2 <= 0)
    width = parent->width() + width2;
  else if((width2 > 0) && (width2 <= 1))
    width = parent->width() * width2;
  else
    width = width2;

  if(height2 <= 0)
    height = parent->height() + height2;
  else if((height2 > 0) && (height2 <= 1))
    height = parent->height() * height2;
  else
    height = height2;


  if(horizontalAlignment == RIGHT)
    relativePositionX = parent->width() - width - positionX;
  else if(horizontalAlignment == CENTER)
    relativePositionX = parent->width()/2.0f - width/2.0f - positionX;
  else
    relativePositionX = positionX;
  if(verticalAlignment == BOTTOM)
    relativePositionY = parent->height() - height - positionY;
  else if(verticalAlignment == CENTER)
    relativePositionY = parent->height()/2.0f - height/2.0f - positionY;
  else
    relativePositionY = positionY;
}

void NewTextList::changeName(const QString _name){
  labels[0]->setText(_name);
}

void NewTextList::changePosition(const float _positionX, const float _positionY){
  if(_positionX == _positionX)
    positionX = _positionX;
  if(_positionY == _positionY)
    positionY = _positionY;

  setPosition();
}

void NewTextList::changeAlignment(const float _horizontalAlignment, const float _verticalAlignment){
  if(_horizontalAlignment == _horizontalAlignment)
    horizontalAlignment = _horizontalAlignment;
  if(_verticalAlignment == _verticalAlignment)
    verticalAlignment = _verticalAlignment;

  setPosition();
}

void NewTextList::changeSize(const float _width, const float _height){
  if(_width == _width)
    width2 = _width;
  if(_height == _height)
    height2 = _height;

  for(int i = 0; i < labels.size(); i++)
    delete labels[i];
  labels.clear();
  delete f_container;
  delete s_container;

  setAbsoluteSize();
  setList();
  changePosition();
}

void NewTextList::changeInputs(vector<nameANDdata> *_values){
  if(_values != nullptr){
    labelValues->clear();
    labelValues = _values;
  }
  for(int i = 0; i < labels.size(); i++)
    delete labels[i];
  labels.clear();
  delete f_container;
  delete s_container;
  setList();
}

void NewTextList::update(){
  int ttl = labels.size() - labelValues->size();
  for(int i = 0; i < labelValues->size(); i++){
    labels[ttl + i]->setText(QString("  %1%2").arg(*labelValues->at(i).input, 2, 'f',
                                                   labelValues->at(i).decimal_places, '0')
                             .arg(labelValues->at(i).label_termination));
  }
}

void NewTextList::setPosition(){
  setAbsoluteSize();

  this->move(relativePositionX, relativePositionY);
  this->resize(width, height);
}

//-------------------------------------------------------------------------------------------------------------//
//                                           icon list UI element                                              //
//-------------------------------------------------------------------------------------------------------------//

NewIconList::NewIconList(QWidget *_parent, const QString _name, vector<nameANDboolen> *_values, QVector<QFont*> *_fonts, const float _positionX,
                         const float _positionY, const float _width, const float _height,
                         const float _horizontalAlignment, const float _verticalAlignment) :
  QFrame(_parent),
  parent(_parent),
  positionX(_positionX),
  positionY(_positionY),
  relativePositionX(_positionX),
  relativePositionY(_positionY),
  horizontalAlignment(_horizontalAlignment),
  verticalAlignment(_verticalAlignment),
  width2(_width),
  height2(_height),
  name(_name),
  labelValues(_values),
  labels(0),
  fonts(_fonts)
{
  setAbsoluteSize();
  setElement();
}

NewIconList::~NewIconList(){
  for(int i = 0; i < labels.size(); i++)
    delete labels[i];
  labels.clear();
  delete f_container;
  delete s_container;
}

void NewIconList::setElement(){
  QString css = ".QScrollBar:vertical{"
                "border:0;"
                "background:rgba(255,255,255,0.2);"
                "width: 13px;"
                "margin: 15px 0 15px 0;"
                "border-radius:6px;"
                "}"
                "QScrollBar::handle:vertical{"
                "background:rgba(160,160,160,1);"
                "border:0;"
                "border-radius:6px;"
                "min-height:25px;"
                "}"
                "QScrollBar::add-line:vertical{"
                "border:0;"
                "background:none;"
                "height: 13px;"
                "subcontrol-position: bottom;"
                "subcontrol-origin: margin;"
                "}"
                "QScrollBar::sub-line:vertical{"
                "border:0;"
                "background:none;"
                "height:13px;"
                "subcontrol-position: top;"
                "subcontrol-origin: margin;"
                "}"
                "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
                "background: none;"
                "}"
                "QScrollBar::up-arrow:vertical{"
                "image: url(:/images/i_up);"
                "}"
                "QScrollBar::down-arrow:vertical{"
                "image: url(:/images/i_down);"
                "}"
                ".QLabel{ color:#78d7e1; }";

  this->resize(width, height);
  this->move(relativePositionX, relativePositionY);
  this->setFrameShape(QFrame::NoFrame);
  this->setLineWidth(0);
  this->setStyleSheet(css);

  setList();
  connect(parent->parentWidget(), SIGNAL(resizing()), this, SLOT(setPosition()));
  this->show();
}
void NewIconList::setList(){
  QLabel *l_name = new QLabel(name, this);
  modLabel(l_name, 0, 5, width, 20, 0);

  f_container = new QFrame(this);
  f_container->setStyleSheet(".QFrame{ border:none; background:none; }");
  f_container->resize(width, 23 * labelValues->size());

  s_container = new QScrollArea(this);
  s_container->setStyleSheet(".QScrollArea{ border:2px solid #4eb4ff; background:rgba(0,0,0,0.4); border-radius:7px; }");
  s_container->move(0, 28);
  s_container->resize(width, height - 45);
  s_container->setWidget(f_container);
  s_container->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  for(int i = 0; i < labelValues->size(); i++){
    QLabel *l_step = new QLabel(labelValues->at(i).label, f_container);
    l_step->setAlignment(Qt::AlignVCenter);
    modLabel(l_step, 30, 23 * i, width - 30, 23, 5, "color:#5eb5ff;");
  }

  for(int i = 0; i < labelValues->size(); i++){
    QLabel *l_step2 = new QLabel(f_container);
    if(*labelValues->at(i).input == true)
      modLabel(l_step2, 0, 4 + 23 * i, 30, 15, 5, "background:url(:/images/i_istatus) top center no-repeat;");
    else
      modLabel(l_step2, 0, 4 + 23 * i, 30, 15, 5, "background:url(:/images/i_istatus) bottom center no-repeat;");
  }
}

void NewIconList::modLabel(QLabel *lbl, int x, int y, int w, int h, int i, QString s){
  lbl->move(x, y);
  lbl->resize(w, h);
  if(i >= 0)
    lbl->setFont(*fonts->at(i));
  if(s != NULL)
    lbl->setStyleSheet(s);
  labels.append(lbl);
}

void NewIconList::setAbsoluteSize(){
  if(width2 <= 0)
    width = parent->width() + width2;
  else if((width2 > 0) && (width2 <= 1))
    width = parent->width() * width2;
  else
    width = width2;

  if(height2 <= 0)
    height = parent->height() + height2;
  else if((height2 > 0) && (height2 <= 1))
    height = parent->height() * height2;
  else
    height = height2;


  if(horizontalAlignment == RIGHT)
    relativePositionX = parent->width() - width - positionX;
  else if(horizontalAlignment == CENTER)
    relativePositionX = parent->width()/2.0f - width/2.0f - positionX;
  else
    relativePositionX = positionX;
  if(verticalAlignment == BOTTOM)
    relativePositionY = parent->height() - height - positionY;
  else if(verticalAlignment == CENTER)
    relativePositionY = parent->height()/2.0f - height/2.0f - positionY;
  else
    relativePositionY = positionY;
}

void NewIconList::changeName(const QString _name){
  labels[0]->setText(_name);
}

void NewIconList::changePosition(const float _positionX, const float _positionY){
  if(_positionX == _positionX)
    positionX = _positionX;
  if(_positionY == _positionY)
    positionY = _positionY;

  setPosition();
}

void NewIconList::changeAlignment(const float _horizontalAlignment, const float _verticalAlignment){
  if(_horizontalAlignment == _horizontalAlignment)
    horizontalAlignment = _horizontalAlignment;
  if(_verticalAlignment == _verticalAlignment)
    verticalAlignment = _verticalAlignment;

  setPosition();
}

void NewIconList::changeSize(const float _width, const float _height){
  if(_width == _width)
    width2 = _width;
  if(_height == _height)
    height2 = _height;

  for(int i = 0; i < labels.size(); i++)
    delete labels[i];
  labels.clear();
  delete f_container;
  delete s_container;

  setAbsoluteSize();
  setList();
  changePosition();
}

void NewIconList::changeInputs(vector<nameANDboolen> *_values){
  if(_values != nullptr){
    labelValues->clear();
    labelValues = _values;
  }
  for(int i = 0; i < labels.size(); i++)
    delete labels[i];
  labels.clear();
  delete f_container;
  delete s_container;
  setList();
}

void NewIconList::update(){
  int ttl = labelValues->size();
  for(int i = 0; i < ttl; i++){
    if(*labelValues->at(i).input == true)
      labels[labels.size() - ttl + i]->setStyleSheet("background:url(:/images/i_istatus) top center no-repeat;");
    else
      labels[labels.size() - ttl + i]->setStyleSheet("background:url(:/images/i_istatus) bottom center no-repeat;");
  }
}

void NewIconList::setPosition(){
  setAbsoluteSize();

  this->move(relativePositionX, relativePositionY);
  this->resize(width, height);
}

//-------------------------------------------------------------------------------------------------------------//
//                                            messages UI element                                              //
//-------------------------------------------------------------------------------------------------------------//

NewMessages::NewMessages(QWidget *_parent, const QString _name, QVector<QFont*> *_fonts,
                         const float _positionX, const float _positionY, const float _width,
                         const float _height, const float _horizontalAlignment, const float _verticalAlignment) :
  QFrame(_parent),
  parent(_parent),
  positionX(_positionX),
  positionY(_positionY),
  relativePositionX(_positionX),
  relativePositionY(_positionY),
  horizontalAlignment(_horizontalAlignment),
  verticalAlignment(_verticalAlignment),
  width2(_width),
  height2(_height),
  name(_name),
  labels(0),
  fonts(_fonts)
{
  setAbsoluteSize();
  setElement();
}

NewMessages::~NewMessages(){
  for(int i = 0; i < labels.size(); i++)
    delete labels[i];
  labels.clear();
  for(int i = 0; i < labels2.size(); i++)
    delete labels2[i];
  labels2.clear();
  for(int i = 0; i < labels3.size(); i++)
    delete labels3[i];
  labels3.clear();
  delete f_container;
  delete s_container;
}

void NewMessages::setElement(){
  QString css = "QScrollBar:vertical{"
                "border:0;"
                "background:rgba(255,255,255,0.2);"
                "width: 13px;"
                "margin: 15px 0 15px 0;"
                "border-radius:6px;"
                "}"
                "QScrollBar::handle:vertical{"
                "background:rgba(160,160,160,1);"
                "border:0;"
                "border-radius:6px;"
                "min-height:25px;"
                "}"
                "QScrollBar::add-line:vertical{"
                "border:0;"
                "background:none;"
                "height: 13px;"
                "subcontrol-position: bottom;"
                "subcontrol-origin: margin;"
                "}"
                "QScrollBar::sub-line:vertical{"
                "border:0;"
                "background:none;"
                "height:13px;"
                "subcontrol-position: top;"
                "subcontrol-origin: margin;"
                "}"
                "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
                "background: none;"
                "}"
                "QScrollBar::up-arrow:vertical{"
                "image: url(:/images/i_up);"
                "}"
                "QScrollBar::down-arrow:vertical{"
                "image: url(:/images/i_down);"
                "}"
                ".QLabel{ color:#78d7e1; }";

  this->resize(width, height);
  this->move(relativePositionX, relativePositionY);
  this->setFrameShape(QFrame::NoFrame);
  this->setLineWidth(0);
  this->setStyleSheet(css);

  setList();
  connect(parent->parentWidget(), SIGNAL(resizing()), this, SLOT(setPosition()));
  this->show();
}
void NewMessages::setList(){
  QLabel *l_name = new QLabel(name, this);
  modLabel(l_name, 0, 5, width, 20, 0, 0);

  f_container = new QFrame(this);
  f_container->setStyleSheet(".QFrame{ border:none; background:none; }");
  f_container->resize(width - 29, 30);

  s_container = new QScrollArea(this);
  s_container->setStyleSheet(".QScrollArea{ border:2px solid #4eb4ff; background:rgba(0,0,0,0.4); border-radius:7px; }");
  s_container->move(0, 28);
  s_container->resize(width - 15, height - 45);
  s_container->setWidget(f_container);
  s_container->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void NewMessages::modLabel(QLabel *lbl, int x, int y, int w, int h, int p, int i, QString s){
  lbl->move(x, y);
  lbl->resize(w, h);
  if(i >= 0)
    lbl->setFont(*fonts->at(i));
  if(s != NULL)
    lbl->setStyleSheet(s);
  if(p == 2)
    labels3.append(lbl);
  else if(p == 1)
    labels2.append(lbl);
  else
    labels.append(lbl);
}

void NewMessages::setAbsoluteSize(){
  if(width2 <= 0)
    width = parent->width() + width2;
  else if((width2 > 0) && (width2 <= 1))
    width = parent->width() * width2;
  else
    width = width2;

  if(height2 <= 0)
    height = parent->height() + height2;
  else if((height2 > 0) && (height2 <= 1))
    height = parent->height() * height2;
  else
    height = height2;

  if(horizontalAlignment == RIGHT)
    relativePositionX = parent->width() - width - positionX;
  else if(horizontalAlignment == CENTER)
    relativePositionX = parent->width()/2.0f - width/2.0f - positionX;
  else
    relativePositionX = positionX;
  if(verticalAlignment == BOTTOM)
    relativePositionY = parent->height() - height - positionY;
  else if(verticalAlignment == CENTER)
    relativePositionY = parent->height()/2.0f - height/2.0f - positionY;
  else
    relativePositionY = positionY;
}

void NewMessages::changeName(const QString _name){
  labels[0]->setText(_name);
}

void NewMessages::changePosition(const float _positionX, const float _positionY){
  if(_positionX == _positionX)
    positionX = _positionX;
  if(_positionY == _positionY)
    positionY = _positionY;

  setPosition();
}

void NewMessages::changeAlignment(const float _horizontalAlignment, const float _verticalAlignment){
  if(_horizontalAlignment == _horizontalAlignment)
    horizontalAlignment = _horizontalAlignment;
  if(_verticalAlignment == _verticalAlignment)
    verticalAlignment = _verticalAlignment;

  setPosition();
}

void NewMessages::changeSize(const float _width, const float _height){
  if(_width == _width)
    width2 = _width;
  if(_height == _height)
    height2 = _height;

  for(int i = 0; i < labels.size(); i++)
    delete labels[i];
  labels.clear();
  for(int i = 0; i < labels2.size(); i++)
    delete labels2[i];
  labels2.clear();
  for(int i = 0; i < labels3.size(); i++)
    delete labels3[i];
  labels3.clear();
  delete f_container;
  delete s_container;

  setAbsoluteSize();
  setList();
  changePosition();
}

void NewMessages::addMessage(const QString _message, const int _type){
  QString css;

  switch(_type){
  case WARNING_M:
    css = "left bottom";
  break;
  case ERROR_M:
    css = "right bottom";
  break;
  case PROPER_M:
    css = "right top";
  break;
  default:
    css = "left top";
  break;
  }

  QLabel *l_step = new QLabel(f_container);
  modLabel(l_step, 7, 5 + 30 * labels2.size(), 20, 20, 1, -1, "background:url(:/images/i_icons) " + css + " no-repeat;");
  l_step->show();

  QLabel *l_step2 = new QLabel(_message, f_container);
  l_step2->setWordWrap(true);
  modLabel(l_step2, 32, 30 * labels3.size(), width - 70, 30, 2, 6);
  l_step2->show();

  f_container->resize(width - 19, 30 * labels2.size());
}

void NewMessages::clearMessages(){
  for(int i = 0; i < labels2.size(); i++)
    delete labels2[i];
  labels2.clear();
  for(int i = 0; i < labels3.size(); i++)
    delete labels3[i];
  labels3.clear();
}

void NewMessages::setPosition(){
  setAbsoluteSize();

  this->move(relativePositionX, relativePositionY);
  this->resize(width, height);

  labels[0]->resize(width, 24);
  f_container->resize(width - 29, 30 * labels2.size());
  s_container->resize(width - 15, height - 45);
  for(int i = 0; i < labels3.size(); i++)
    labels3[i]->resize(width - 70, 30);
}

//-------------------------------------------------------------------------------------------------------------//
//                                            signaling UI element                                             //
//-------------------------------------------------------------------------------------------------------------//

NewSignaling::NewSignaling(QWidget *_parent, QVector<QFont*> *_fonts, const float _positionX, const float _positionY,
                           const float _width, const float _height, const float _horizontalAlignment,
                           const float _verticalAlignment) :
  QFrame(_parent),
  parent(_parent),
  positionX(_positionX),
  positionY(_positionY),
  relativePositionX(_positionX),
  relativePositionY(_positionY),
  width2(_width),
  height2(_height),
  horizontalAlignment(_horizontalAlignment),
  verticalAlignment(_verticalAlignment),
  relativeHeight(0),
  labels(0),
  fonts(_fonts)
{
  setAbsoluteSize();
  setElement();
}

void NewSignaling::setAbsoluteSize(){
  if(width2 <= 0)
    width = parent->width() + width2;
  else if((width2 > 0) && (width2 <= 1))
    width = parent->width() * width2;
  else
    width = width2;

  if(height2 <= 0)
    height = parent->height() + height2;
  else if((height2 > 0) && (height2 <= 1))
    height = parent->height() * height2;
  else
    height = height2;


  if(horizontalAlignment == RIGHT)
    relativePositionX = parent->width() - width - positionX;
  else if(horizontalAlignment == CENTER)
    relativePositionX = parent->width()/2.0f - width/2.0f - positionX;
  else
    relativePositionX = positionX;
  if(verticalAlignment == BOTTOM)
    relativePositionY = parent->height() - height - positionY;
  else if(verticalAlignment == CENTER)
    relativePositionY = parent->height()/2.0f - height/2.0f - positionY;
  else
    relativePositionY = positionY;
}

NewSignaling::~NewSignaling(){
  for(int i = 0; i < labels.size(); i++)
    delete labels[i];
  labels.clear();
}

void NewSignaling::setElement(){
  this->resize(width, height);
  this->move(relativePositionX, relativePositionY);
  this->setFrameShape(QFrame::NoFrame);
  this->setLineWidth(0);
  connect(parent->parentWidget(), SIGNAL(resizing()), this, SLOT(setPosition()));
  this->show();
}

void NewSignaling::modLabel(QLabel *lbl, int x, int y, int w, int h, int i, QString s){
  lbl->move(x, y);
  lbl->resize(w, h);
  if(i >= 0)
    lbl->setFont(*fonts->at(i));
  if(s != NULL)
    lbl->setStyleSheet(s);
  labels.append(lbl);
}

void NewSignaling::changePosition(const float _positionX, const float _positionY){
  if(_positionX == _positionX)
    positionX = _positionX;
  if(_positionY == _positionY)
    positionY = _positionY;

  setPosition();
}

void NewSignaling::changeAlignment(const float _horizontalAlignment, const float _verticalAlignment){
  if(_horizontalAlignment == _horizontalAlignment)
    horizontalAlignment = _horizontalAlignment;
  if(_verticalAlignment == _verticalAlignment)
    verticalAlignment = _verticalAlignment;

  setPosition();
}

void NewSignaling::changeSize(const float _width, const float _height){
  if(_width == _width)
    width2 = _width;
  if(_height == _height)
    height2 = _height;

  changePosition();
}

void NewSignaling::addSignal(const int _type, const int _numericalValue){
  QString css, css2 = "";
  int Height = 50;
  QLabel *l_step = new QLabel(this);

  switch(_type){
  case GREEN:
    css = "i_semaphore) left bottom";
    Height = 150;
  break;
  case YELLOW:
    css = "i_semaphore) center bottom";
    Height = 150;
  break;
  case RED:
    css = "i_semaphore) right bottom";
    Height = 150;
  break;
  case YELLOW_RED:
    css = "i_semaphore) right top";
    Height = 150;
  break;
  case YELLOW_GREEN:
    css = "i_semaphore) center top";
    Height = 150;
  break;
  case 267:
    css = "i_signals) left bottom";
  break;
  case 205:
    css = "i_signals) right top";
  break;
  case 306:
    css = "i_signals) left top";
  break;
  case 215:
    css = "i_signals) left center";
  break;
  case 350:
    css = "i_signals) center center";
  break;
  case 220:
    css = "i_signals) center bottom";
  break;
  case 108:
    css = "i_signals) right center";
  break;
  case 274:
    css = "i_signals) center top";
    css2 = " color:#000;";
    l_step->setText(QString("%1").arg(_numericalValue, 1, 'f', 0, '0'));
    l_step->setAlignment(Qt::AlignCenter);
  break;
  default:
    css = "i_semaphore) left top";
    Height = 150;
  break;
  }

  modLabel(l_step, 0, relativeHeight, 50, Height, 7, "background:url(:/images/" + css + " no-repeat;" + css2);
  l_step->show();
  relativeHeight += Height + 10;
}

void NewSignaling::clearSignals(){
  for(int i = 0; i < labels.size(); i++)
    delete labels[i];
  labels.clear();
  relativeHeight = 0;
}

void NewSignaling::setPosition(){
  setAbsoluteSize();

  this->move(relativePositionX, relativePositionY);
  this->resize(width, height);
}

//-------------------------------------------------------------------------------------------------------------//
//                                              clock UI element                                               //
//-------------------------------------------------------------------------------------------------------------//

NewClock::NewClock(QWidget *_parent, int *_whichCompany, const QString *const _timestamp, QVector<QFont*> *_fonts, const float _positionX,
                   const float _positionY, const float _width, const float _height, const float _horizontalAlignment,
                   const float _verticalAlignment) :
  QFrame(_parent),
  parent(_parent),
  positionX(_positionX),
  positionY(_positionY),
  relativePositionX(_positionX),
  relativePositionY(_positionY),
  horizontalAlignment(_horizontalAlignment),
  verticalAlignment(_verticalAlignment),
  width2(_width),
  height2(_height),
  whichCompany(_whichCompany),
  labels(0),
  fonts(_fonts),
  timestamp(_timestamp)
{
  setAbsoluteSize();
  setElement();
}

NewClock::~NewClock(){
  for(int i = 0; i < labels.size(); i++)
    delete labels[i];
  labels.clear();
  delete logo;
}

void NewClock::setElement(){
  this->resize(width, height);
  this->move(relativePositionX, relativePositionY);
  this->setFrameShape(QFrame::NoFrame);
  this->setLineWidth(0);
  this->setStyleSheet(".QLabel{ color:#78d7e1; }");

  //timestamp format : 0000-12-31 24:60:60.999
  dateTime = QDateTime::fromString(*timestamp, "yyyy-MM-dd HH:mm:ss.zzz");

  QLabel *l_date = new QLabel(dateTime.toString("dd MMMM, yyyy"), this);
  modLabel(l_date, 71, 12, 170, 20, 0);

  QLabel *l_time = new QLabel(dateTime.toString("HH:mm:ss.zzz"), this);
  modLabel(l_time, 69, 26, 170, 22, 8);

  logo = new QPushButton(this);
  logo->resize(64, 66);
  logo->setCursor(Qt::PointingHandCursor);
  logo->setFlat(true);
  logo->setFocusPolicy(Qt::NoFocus);

  change();
  //this will connect our logotype button to the functions to change the vehicle in scene
  connect(logo, SIGNAL(released()), parent->parentWidget(), SLOT(changeCompany()));
  //this will do it for the image in the button
  connect(logo, SIGNAL(released()), this, SLOT(change()));
  connect(parent->parentWidget(), SIGNAL(resizing()), this, SLOT(setPosition()));
  this->show();
}

void NewClock::modLabel(QLabel *lbl, int x, int y, int w, int h, int i, QString s){
  lbl->move(x, y);
  lbl->resize(w, h);
  if(i >= 0)
    lbl->setFont(*fonts->at(i));
  if(s != NULL)
    lbl->setStyleSheet(s);
  labels.append(lbl);
}

void NewClock::setAbsoluteSize(){
  if(width2 <= 0)
    width = parent->width() + width2;
  else if((width2 > 0) && (width2 <= 1))
    width = parent->width() * width2;
  else
    width = width2;

  if(height2 <= 0)
    height = parent->height() + height2;
  else if((height2 > 0) && (height2 <= 1))
    height = parent->height() * height2;
  else
    height = height2;


  if(horizontalAlignment == RIGHT)
    relativePositionX = parent->width() - width - positionX;
  else if(horizontalAlignment == CENTER)
    relativePositionX = parent->width()/2.0f - width/2.0f - positionX;
  else
    relativePositionX = positionX;
  if(verticalAlignment == BOTTOM)
    relativePositionY = parent->height() - height - positionY;
  else if(verticalAlignment == CENTER)
    relativePositionY = parent->height()/2.0f - height/2.0f - positionY;
  else
    relativePositionY = positionY;
}

void NewClock::changePosition(const float _positionX, const float _positionY){
  if(_positionX == _positionX)
    positionX = _positionX;
  if(_positionY == _positionY)
    positionY = _positionY;

  setPosition();
}

void NewClock::changeAlignment(const float _horizontalAlignment, const float _verticalAlignment){
  if(_horizontalAlignment == _horizontalAlignment)
    horizontalAlignment = _horizontalAlignment;
  if(_verticalAlignment == _verticalAlignment)
    verticalAlignment = _verticalAlignment;

  setPosition();
}

void NewClock::changeSize(const float _width, const float _height){
  if(_width == _width)
    width2 = _width;
  if(_height == _height)
    height2 = _height;

  changePosition();
}

void NewClock::update(){
  dateTime = QDateTime::fromString(*timestamp, "yyyy-MM-dd HH:mm:ss.zzz");
  labels[0]->setText(dateTime.toString("dd MMMM, yyyy"));
  labels[1]->setText(dateTime.toString("HH:mm:ss.zzz"));
}

void NewClock::setPosition(){
  setAbsoluteSize();

  this->move(relativePositionX, relativePositionY);
  this->resize(width, height);
}

void NewClock::change(){
  QString css;
  switch(*whichCompany){
  case 0:
    css = "l_pem";
  break;
  case 1:
    css = "l_dhl";
  break;
  default:
    css = "l_noone";
  break;
  }
  logo->setStyleSheet("QPushButton{ background:url(:/images/" + css + ") left top no-repeat; }"
                      "QPushButton:pressed, QPushButton:click{ background:rgba(0,0,0,0) url(:/images/" + css + ") center top no-repeat; }"
                      "QPushButton:hover:!pressed{ background:url(:/images/" + css + ") right top no-repeat; }");
}

//-------------------------------------------------------------------------------------------------------------//
//                                             compass UI element                                              //
//-------------------------------------------------------------------------------------------------------------//

NewCompass::NewCompass(QWidget *_parent, double *_angleVehicle, double *_angleFrame, int *_angleScenary, QVector<QFont*> *_fonts,
                       const float _positionX, const float _positionY, const float _width, const float _height,
                       const float _horizontalAlignment, const float _verticalAlignment) :
  QFrame(_parent),
  parent(_parent),
  positionX(_positionX),
  positionY(_positionY),
  relativePositionX(_positionX),
  relativePositionY(_positionY),
  horizontalAlignment(_horizontalAlignment),
  verticalAlignment(_verticalAlignment),
  width2(_width),
  height2(_height),
  angleVehicle(_angleVehicle),
  angleFrame(_angleFrame),
  angleScenary(_angleScenary),
  labels(0),
  fonts(_fonts)
{
  setAbsoluteSize();
  setElement();
}

NewCompass::~NewCompass(){
  for(int i = 0; i < labels.size(); i++)
    delete labels[i];
  labels.clear();
}

void NewCompass::setElement(){
  this->resize(width, height);
  this->move(relativePositionX, relativePositionY);
  this->setFrameShape(QFrame::NoFrame);
  this->setLineWidth(0);
  this->setStyleSheet(".QLabel{ color:#ffffff; }");

  QLabel *l_backgroung = new QLabel(this);
  modLabel(l_backgroung, 0, 10, width, 40, -1);

  QLabel *l_arrow = new QLabel(this);
  modLabel(l_arrow, width/2.0f - 10, 0, 20, 20, -1, "background:url(:/images/i_compass) no-repeat;");

  QLabel *l_c1 = new QLabel("W", this);
  l_c1->setAlignment(Qt::AlignHCenter);
  modLabel(l_c1, 0, 0, 20, 20, 5);

  QLabel *l_c2 = new QLabel("S", this);
  l_c2->setAlignment(Qt::AlignHCenter);
  modLabel(l_c2, 0, 0, 20, 20, 5);

  QLabel *l_c3 = new QLabel("E", this);
  l_c3->setAlignment(Qt::AlignHCenter);
  modLabel(l_c3, 0, 0, 20, 20, 5);

  QLabel *l_c4 = new QLabel("N", this);
  l_c4->setAlignment(Qt::AlignHCenter);
  modLabel(l_c4, 0, 0, 20, 20, 5);

  lcs3 = width/3.0f;
  lcs30 = lcs3/30.0f;
  lcs1 = width + lcs3;

  connect(parent->parentWidget(), SIGNAL(resizing()), this, SLOT(setPosition()));
  connect(parent->parentWidget(), SIGNAL(rotating()), this, SLOT(update()));
  this->show();
  update();
}

void NewCompass::modLabel(QLabel *lbl, int x, int y, int w, int h, int i, QString s){
  lbl->move(x, y);
  lbl->resize(w, h);
  if(i >= 0)
    lbl->setFont(*fonts->at(i));
  if(s != NULL)
    lbl->setStyleSheet(s);
  labels.append(lbl);
}

void NewCompass::setAbsoluteSize(){
  if(width2 <= 0)
    width = parent->width() + width2;
  else if((width2 > 0) && (width2 <= 1))
    width = parent->width() * width2;
  else
    width = width2;

  if(height2 <= 0)
    height = parent->height() + height2;
  else if((height2 > 0) && (height2 <= 1))
    height = parent->height() * height2;
  else
    height = height2;


  if(horizontalAlignment == RIGHT)
    relativePositionX = parent->width() - width - positionX;
  else if(horizontalAlignment == CENTER)
    relativePositionX = parent->width()/2.0f - width/2.0f - positionX;
  else
    relativePositionX = positionX;
  if(verticalAlignment == BOTTOM)
    relativePositionY = parent->height() - height - positionY;
  else if(verticalAlignment == CENTER)
    relativePositionY = parent->height()/2.0f - height/2.0f - positionY;
  else
    relativePositionY = positionY;
}

void NewCompass::changePosition(const float _positionX, const float _positionY){
  if(_positionX == _positionX)
    positionX = _positionX;
  if(_positionY == _positionY)
    positionY = _positionY;

  setPosition();
}

void NewCompass::changeAlignment(const float _horizontalAlignment, const float _verticalAlignment){
  if(_horizontalAlignment == _horizontalAlignment)
    horizontalAlignment = _horizontalAlignment;
  if(_verticalAlignment == _verticalAlignment)
    verticalAlignment = _verticalAlignment;

  setPosition();
}

void NewCompass::changeSize(const float _width, const float _height){
  if(_width == _width)
    width2 = _width;
  if(_height == _height)
    height2 = _height;

  changePosition();
}

void NewCompass::setVehicleYaw(double *_yawAngleVehicle){
  angleVehicle = _yawAngleVehicle;
}

void NewCompass::setFrameYaw(double *_yawAngleFrame){
  angleFrame = _yawAngleFrame;
}

void NewCompass::update(){
  int tlcs3 = 0, tlcs4, q, tgh;
  float ang2;
  if(angleVehicle != nullptr)
    if(angleFrame != nullptr)
      ang2 = -*angleVehicle - *angleFrame * toDEGREES - *angleScenary/anglePrecision;
    else
      ang2 = -*angleVehicle - *angleScenary/anglePrecision;
  else
    ang2 = -*angleScenary/anglePrecision;
  ang2 = (ang2 < -180)? ang2 + 360 : ang2;
  ang2 = (ang2 > 180)? ang2 - 360 : ang2;
  float ang = width/2.0f + ang2/90.0f * lcs3 - 5;

  labels[2]->move(ang - 10, 5);

  tgh = ang + lcs3;
  if(tgh > lcs1)
    tgh -= lcs1;
  else if(tgh < -lcs3)
    tgh += lcs1;
  labels[3]->move(tgh - 10, 5);

  tgh = ang + lcs3 * 2;
  if(tgh > lcs1)
    tgh -= lcs1;
  else if(tgh < -lcs3)
    tgh += lcs1;
  labels[4]->move(tgh - 10, 5);

  tgh = ang + lcs3 * 3;
  if(tgh > lcs1)
    tgh -= lcs1;
  else if(tgh < -lcs3)
    tgh += lcs1;
  labels[5]->move(tgh - 10, 5);

  pxm = QPixmap(width, 40);
  pxm.fill(Qt::transparent);
  ptr.begin(&pxm);
  ptr.setPen(QPen(QColor(255, 255, 255, 160), 1, Qt::SolidLine));
  for(int i = 0; i < 3; i++){
    tlcs3 = ang + lcs3 * i;
    if(tlcs3 > width)
      tlcs3 -= width;
    else if(tlcs3 < 0)
      tlcs3 += width;
    //ptr.drawLine(tlcs3, 0, tlcs3, 10);

    q = 0;
    for(int e = 1; e < 30; e++){
      tlcs4 = tlcs3 + lcs30 * e;

      if(tlcs4 > width)
        tlcs4 -= width;
      else if(tlcs4 < 0)
        tlcs4 += width;

      if(q > 8){
        ptr.drawLine(tlcs4, 1, tlcs4, 4);
        q = 0;
      }else{
        ptr.drawLine(tlcs4, 2, tlcs4, 3);
        q++;
      }
    }
  }
  ptr.end();
  labels[0]->setPixmap(pxm);
}

void NewCompass::setPosition(){
  setAbsoluteSize();

  lcs3 = width/3.0f;
  lcs30 = lcs3/30.0f;
  lcs1 = width + lcs3;

  labels[0]->resize(width, height);
  labels[1]->move(width/2.0f - 10, 0);

  this->move(relativePositionX, relativePositionY);
  this->resize(width, height);

  update();
}

//-------------------------------------------------------------------------------------------------------------//
//                                            controls UI element                                              //
//-------------------------------------------------------------------------------------------------------------//

NewControls::NewControls(QWidget *_parent, unsigned int _initialDataset, unsigned int _initialFrame,
                         unsigned int _initialDelayment, bool *_options, QVector<QFont*> *_fonts) :
  QFrame(_parent),
  parent(_parent),
  frame(_initialFrame),
  totalFrames(0),
  delayment(_initialDelayment),
  options(_options),
  isPlayed(false),
  exist(false),
  labels(0),
  buttons(0),
  editors(0),
  fonts(_fonts),
  datasets(0),
  positionDataset(0),
  sizeDatasets(0)
{
  cssFrame = "QLabel{ color:#CCC; }"
      "QLineEdit{ background:#333C43; border-radius:4px; color:#fff; text-align:center; padding:3px 5px 3px 5px; }"
      "QLineEdit:hover { background:#8ec1e7; color:#000; border-radius:4px; }";
  cssButton = "QPushButton{ background:#333C43 url(:/images/i_arrows) %1 no-repeat; border-radius:4px; %2}"
      "QPushButton:pressed { background:#B1BEC8 url(:/images/i_arrows) %1 no-repeat; border-radius:4px; %2}"
      "QPushButton:click { background:#B1BEC8 url(:/images/i_arrows) %1 no-repeat; border-radius:4px; %2}"
      "QPushButton:hover:!pressed { background:#8ec1e7 url(:/images/i_arrows) %1 no-repeat; border-radius:4px; %2}";

  validator = new QIntValidator(this);
  validator->setBottom(0);

  //checking all the existing data folders inside "raw/" folder
  QDir folder = E_KITTIFOLDER;
  if(folder.exists()){
    QStringList tlist = folder.entryList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::Name);
    for(int i = 0; i < tlist.size(); i++){
      QString tname = QString(tlist.at(i));
      tname.resize(4);
      datasets.append(tname.toUInt());
    }
    sizeDatasets = datasets.size();
    if(sizeDatasets > 0)
      dataset = datasets[0];
    else
      dataset = 0;
  }

  for(int i = 0; i < datasets.size(); i++){
    if(datasets[i] == dataset){
      positionDataset = i;
      exist = true;
      //opening the velodyne_points folder
      QDir cDir(E_KITTIFOLDER + QString("%1_sync/velodyne_points/data/")
          .arg(QString::number(dataset).rightJustified(4, '0')));
      if(cDir.exists()){
        //viewing all the files inside the folder
        cDir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
        //counting all the files
        totalFrames = cDir.count();
      }
    }
  }
  if(!exist){
    if(sizeDatasets > 0)
      dataset = datasets[0];
    else{
      dataset = 0;
      qWarning("no dataset was found");
    }
    positionDataset = 0;
  }

  setElement();

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame()));

  gotoDataset(_initialDataset);
  gotoFrame(_initialFrame);

  //when the program is first open the dataset is never read, because the connect is called
  //after the dataset is read, and the info is not updated , this will solve this problem
  //by creating a signal after 10 milisecond
  QTimer::singleShot(10, this, SLOT(firstShot()));
}

NewControls::~NewControls(){
  for(int i = 0; i < labels.size(); i++)
    delete labels[i];
  labels.clear();
  for(int i = 0; i < buttons.size(); i++)
    delete buttons[i];
  buttons.clear();
  for(int i = 0; i < editors.size(); i++)
    delete editors[i];
  editors.clear();
  delete validator;
}

void NewControls::setElement(){
  this->resize(210, 110);
  if(*options)
    this->move(parent->width()/2.0f - 360, parent->height()/2.0f - 145);
  else
    this->move(parent->width() - 215, 10);
  this->setFrameShape(QFrame::NoFrame);
  this->setLineWidth(0);
  this->setStyleSheet(cssFrame);

  QLabel *l_frame = new QLabel("FRAME:", this);
  modLabel(l_frame, 0, 0, 75, 17, 3);

  QLabel *l_dataset = new QLabel(QString("DATASET (%1):").arg(totalFrames), this);
  modLabel(l_dataset, 52, 0, 100, 17, 3);

  QLabel *l_delay = new QLabel("DELAY:", this);
  modLabel(l_delay, 152, 0, 70, 17, 3);

  QLineEdit *i_frame = new QLineEdit(QString("%1").arg(frame), this);
  modLabel(i_frame, 0, 18, 45, 32, 9);
  connect(i_frame, SIGNAL(editingFinished()), this, SLOT(gotoFrame()));

  QLineEdit *i_dataset = new QLineEdit(QString("%1").arg(dataset), this);
  modLabel(i_dataset, 73, 18, 56, 32, 9);
  connect(i_dataset, SIGNAL(editingFinished()), this, SLOT(gotoDataset()));

  QLineEdit *i_delay = new QLineEdit(QString("%1").arg(delayment), this);
  modLabel(i_delay, 152, 18, 50, 32, 9);
  connect(i_delay, SIGNAL(editingFinished()), this, SLOT(setDelayment()));

  QPushButton *b_plus = new QPushButton(this);
  b_plus->setShortcut(Qt::Key_Up);
  modLabel(b_plus, 120, 18, 25, 32, -1, cssButton.arg("left bottom").arg("border-top-left-radius:0px; border-bottom-left-radius:0px; "));
  connect(b_plus, SIGNAL(released()), this, SLOT(nextDataset()));

  QPushButton *b_minus = new QPushButton(this);
  b_minus->setShortcut(Qt::Key_Down);
  modLabel(b_minus, 52, 18, 25, 32, -1, cssButton.arg("center bottom").arg("border-top-right-radius:0px; border-bottom-right-radius:0px; "));
  connect(b_minus, SIGNAL(released()), this, SLOT(previousDataset()));

  QPushButton *b_first = new QPushButton(this);
  b_first->setShortcut(Qt::Key_Home);
  modLabel(b_first, 0, 56, 35, 32, -1, cssButton.arg("left center").arg(""));
  connect(b_first, SIGNAL(released()), this, SLOT(firstFrame()));

  QPushButton *b_previous = new QPushButton(this);
  b_previous->setShortcut(Qt::Key_Left);
  modLabel(b_previous, 42, 56, 35, 32, -1, cssButton.arg("center top").arg(""));
  connect(b_previous, SIGNAL(released()), this, SLOT(previousFrame()));

  QPushButton *b_next = new QPushButton(this);
  b_next->setShortcut(Qt::Key_Right);
  modLabel(b_next, 84, 56, 35, 32, -1, cssButton.arg("center center").arg(""));
  connect(b_next, SIGNAL(released()), this, SLOT(nextFrame()));

  QPushButton *b_last = new QPushButton(this);
  b_last->setShortcut(Qt::Key_End);
  modLabel(b_last, 126, 56, 35, 32, -1, cssButton.arg("left top").arg(""));
  connect(b_last, SIGNAL(released()), this, SLOT(lastFrame()));

  QPushButton *b_play = new QPushButton(this);
  b_play->setShortcut(Qt::Key_Space);
  modLabel(b_play, 167, 56, 35, 32, -1, cssButton.arg("right top").arg(""));
  connect(b_play, SIGNAL(released()), this, SLOT(play()));

  connect(parent->parentWidget(), SIGNAL(resizing()), this, SLOT(setPosition()));
  connect(parent->parentWidget()->parentWidget()->parentWidget(), SIGNAL(options_trigger()), this, SLOT(setPosition()));
  this->show();
  update();
}

void NewControls::modLabel(QLabel *lbl, int x, int y, int w, int h, int i, QString s){
  lbl->move(x, y);
  lbl->resize(w, h);
  if(i >= 0)
    lbl->setFont(*fonts->at(i));
  if(s != NULL)
    lbl->setStyleSheet(s);
  labels.append(lbl);
}

void NewControls::modLabel(QPushButton *lbl, int x, int y, int w, int h, int i, QString s){
  lbl->move(x, y);
  lbl->resize(w, h);
  if(i >= 0)
    lbl->setFont(*fonts->at(i));
  if(s != NULL)
    lbl->setStyleSheet(s);
  lbl->setFlat(true);
  lbl->setFocusPolicy(Qt::NoFocus);
  lbl->setCursor(Qt::PointingHandCursor);
  buttons.append(lbl);
}

void NewControls::modLabel(QLineEdit *lbl, int x, int y, int w, int h, int i, QString s){
  lbl->move(x, y);
  lbl->resize(w, h);
  if(i >= 0)
    lbl->setFont(*fonts->at(i));
  if(s != NULL)
    lbl->setStyleSheet(s);
  lbl->setValidator(validator);
  lbl->setFocusPolicy(Qt::ClickFocus);
  editors.append(lbl);
}

void NewControls::nextFrame(){
  unsigned int nframe = (frame >= (totalFrames - 1))? 0 : frame + 1;
  gotoFrame(nframe);
}

void NewControls::previousFrame(){
  unsigned int nframe = (frame <= 0)? totalFrames - 1 : frame - 1;
  gotoFrame(nframe);
}

void NewControls::firstFrame(){
  gotoFrame(0);
}

void NewControls::lastFrame(){
  gotoFrame(totalFrames - 1);
}

void NewControls::gotoFrame(){
  int frams = editors[0]->text().toInt();
  if(frams != frame){
    if((frams >= 0) && (frams < totalFrames)){
      gotoFrame(frams);
    }else if(frams >= totalFrames){
      gotoFrame(totalFrames - 1);
    }else if(frams < 0){
      gotoFrame(0);
    }
  }
}

bool NewControls::gotoFrame(unsigned int _frame){
  bool ok = false;
  if((_frame != frame) && exist){
    if(_frame >= totalFrames)
      frame = totalFrames - 1;
    else
      frame = _frame;
    editors[0]->setText(QString::number(frame));

    ok = true;

    emit updateFrame(frame);
    emit updateData();
  }
  return ok;
}

void NewControls::nextDataset(){
  positionDataset = (positionDataset >= (sizeDatasets - 1))? 0 : positionDataset + 1;
  gotoDataset(datasets[positionDataset]);
}

void NewControls::previousDataset(){
  positionDataset = (positionDataset <= 0)? sizeDatasets - 1 : positionDataset - 1;
  gotoDataset(datasets[positionDataset]);
}

void NewControls::gotoDataset(){
  unsigned int ndataset = editors[1]->text().toUInt();
  if(ndataset != dataset)
    gotoDataset(ndataset);
}

bool NewControls::gotoDataset(unsigned int _dataset){
  bool ok = false;
  if(_dataset != dataset){
    exist = false;
    for(int i = 0; i < datasets.size(); i++){
      if(datasets[i] == _dataset){
        positionDataset = i;
        dataset = datasets[i];
        exist = true;
        //opening the velodyne_points folder
        QDir cDir(E_KITTIFOLDER + QString("%1_sync/velodyne_points/data/")
            .arg(QString::number(dataset).rightJustified(4, '0')));
        if(cDir.exists()){
          //viewing all the files inside the folder
          cDir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
          //counting all the files
          totalFrames = cDir.count();
          labels[1]->setText(QString("DATASET (%1):").arg(totalFrames));

          ok = true;

          emit updateDataset(dataset);
          emit updateFrame(0);
          emit updateData();
        }
      }
    }
    if(!exist){
      qWarning() << "This dataset: " <<_dataset << " does not exist";
      if(datasets.size() > 0){
        //dataset = datasets[0];
        //positionDataset = 0;
        emit updateDataset(dataset);
        emit updateFrame(0);
        emit updateData();
        exist = true;
      }else{
        qWarning("No datasets were found");
      }
    }
    editors[1]->setText(QString::number(dataset));
    frame = 0;
    editors[0]->setText("0");
  }
  return ok;
}

void NewControls::setDelayment(){
  unsigned int ndelay = editors[2]->text().toUInt();
  if(ndelay != delayment)
    setDelayment(ndelay);
}

void NewControls::setDelayment(unsigned int _delay){
  delayment = _delay;
  editors[2]->setText(QString::number(_delay));
  if(isPlayed){
    timer->start(delayment);
  }
}

void NewControls::pause(){
  if(isPlayed){
    isPlayed = false;
    timer->stop();
    buttons[6]->setStyleSheet(cssButton.arg("right top").arg(""));
  }
}

unsigned int NewControls::getFrame(){
  return frame;
}

unsigned int NewControls::getDataset(){
  return dataset;
}

void NewControls::play(){
  isPlayed = !isPlayed;
  if(!isPlayed){
    timer->stop();
    buttons[6]->setStyleSheet(cssButton.arg("right top").arg(""));
  }else{
    timer->start(delayment);
    buttons[6]->setStyleSheet(cssButton.arg("right center").arg(""));
  }
}

void NewControls::firstShot(){
  emit updateDataset(dataset);
  emit updateFrame(frame);
  emit updateData();
}

void NewControls::setPosition(){
  if(*options)
    this->move(parent->width()/2.0f - 545, parent->height()/2.0f - 145);
  else
    this->move(parent->width() - 215, 10);
}


//-------------------------------------------------------------------------------------------------------------//
//                                              camera UI element                                              //
//-------------------------------------------------------------------------------------------------------------//

NewCamera::NewCamera(QWidget *_parent, QString _name, unsigned int _initialDataset, unsigned int _initialFrame,
                     unsigned int _cameraSet, const float _width, const float _height, const float _positionX, const float _positionY,
                     const float _horizontalAlignment, const float _verticalAlignment, QVector<QFont *> *_fonts) :
  QFrame(_parent),
  parent(_parent),
  dataset(_initialDataset),
  frame(_initialFrame),
  cameraSet(_cameraSet),
  name(_name),
  labels(0),
  fonts(_fonts),
  backCamera(1242, 375),
  positionX(_positionX),
  positionY(_positionY),
  relativePositionX(_positionX),
  relativePositionY(_positionY),
  width2(_width),
  height2(_height),
  horizontalAlignment(_horizontalAlignment),
  verticalAlignment(_verticalAlignment)
{
  setAbsoluteSize();
  setElement();
}

NewCamera::~NewCamera(){
  for(int i = 0; i < labels.size(); i++)
    delete labels[i];
  labels.clear();
}

void NewCamera::setElement(){
  this->resize(width, height);
  this->move(relativePositionX, relativePositionY);
  this->setFrameShape(QFrame::NoFrame);
  this->setLineWidth(0);

  QLabel *l_back = new QLabel(this);
  l_back->setAlignment(Qt::AlignTop);
  modLabel(l_back, 0, 0, 1242, 375, -1, NULL);
  QLabel *l_name = new QLabel(name, this);
  modLabel(l_name, 10, 10, 200, 35, 8, "background:#000; color:#FFF; padding:5px;");

  connect(parent->parentWidget(), SIGNAL(resizing()), this, SLOT(setPosition()));
  update();
  this->show();
}

void NewCamera::setAbsoluteSize(){
  if(width2 <= 0)
    width = parent->width() + width2;
  else if((width2 > 0) && (width2 <= 1))
    width = parent->width() * width2;
  else
    width = width2;

  if(height2 <= 0)
    height = parent->height() + height2;
  else if((height2 > 0) && (height2 <= 1))
    height = parent->height() * height2;
  else
    height = height2;


  if(horizontalAlignment == RIGHT)
    relativePositionX = parent->width() - width - positionX;
  else if(horizontalAlignment == CENTER)
    relativePositionX = parent->width()/2.0f - width/2.0f - positionX;
  else
    relativePositionX = positionX;
  if(verticalAlignment == BOTTOM)
    relativePositionY = parent->height() - height - positionY;
  else if(verticalAlignment == CENTER)
    relativePositionY = parent->height()/2.0f - height/2.0f - positionY;
  else
    relativePositionY = positionY;
}

void NewCamera::gotoFrame(unsigned int _frame){
  frame = _frame;
  update();
}

void NewCamera::setDataSet(unsigned int _dataset){
  dataset = _dataset;
  update();
}

void NewCamera::modLabel(QLabel *lbl, int x, int y, int w, int h, int i, QString s){
  lbl->move(x, y);
  lbl->resize(w, h);
  if(i >= 0)
    lbl->setFont(*fonts->at(i));
  if(s != NULL)
    lbl->setStyleSheet(s);
  else
    labels.append(lbl);
}

void NewCamera::changeName(const QString _name){
  name = _name;
  labels[1]->setText(_name);
}

void NewCamera::changePosition(const float _positionX, const float _positionY){
  if(_positionX == _positionX)
    positionX = _positionX;
  if(_positionY == _positionY)
    positionY = _positionY;

  setPosition();
}

void NewCamera::changeAlignment(const float _horizontalAlignment, const float _verticalAlignment){
  if(_horizontalAlignment == _horizontalAlignment)
    horizontalAlignment = _horizontalAlignment;
  if(_verticalAlignment == _verticalAlignment)
    verticalAlignment = _verticalAlignment;

  setPosition();
}

void NewCamera::update(){
  if(*onCamera){
    if(this->isHidden())
      this->show();
    if(backCamera.load(E_KITTIFOLDER + QString("%1_sync/image_%2/")
                       .arg(QString::number(dataset).rightJustified(4, '0'))
                       .arg(QString::number(cameraSet).rightJustified(2, '0')) + "data/" +
                       QString::number(frame).rightJustified(10, '0') + ".png"))
      if((width != 1242) || (height != 375))
        labels[0]->setPixmap(backCamera.scaled(width, height, Qt::KeepAspectRatio));
      else
        labels[0]->setPixmap(backCamera);
  }else
    if(!this->isHidden())
      this->hide();
}

void NewCamera::setPosition(){
  setAbsoluteSize();

  this->move(relativePositionX, relativePositionY);
  this->resize(width, height);
  labels[0]->resize(width, height);
  update();
}

void NewCamera::changeSize(const float _width, const float _height){
  if(_width == _width)
    width2 = _width;
  if(_height == _height)
    height2 = _height;

  setAbsoluteSize();
  changePosition();
}

