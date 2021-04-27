#include "headers/optionbuttons.h"

OptionButtons::OptionButtons(const QString _name, QWidget *_parent, uint _id) :
  QPushButton(_name, _parent),
  name(_name),
  id(_id)
{
  this->resize(160, 30);
  this->setCursor(Qt::PointingHandCursor);
  this->setFocusPolicy(Qt::NoFocus);
  this->setFlat(true);
  this->show();
  connect(this, SIGNAL(released()), this, SLOT(emitSignal()));
}

unsigned int OptionButtons::getId(){
  return id;
}

void OptionButtons::emitSignal(){
  emit buttonReleased(id);
}

