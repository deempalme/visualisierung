#include "headers/textdraftsman.h"

TextDraftsman::TextDraftsman(QFrame *_frame, const vector<XYZLabel> *const _labels,
                             const QMatrix4x4 *const _screenMatrix, QFont *_font, bool *_onLabels, bool *_visible) :
  frame(_frame),
  screenMatrix(_screenMatrix),
  labelList(_labels),
  modelVehicle(nullptr),
  font(_font),
  onLabels(_onLabels),
  visible(_visible)
{
  background = new QLabel(frame);
  background->resize(frame->width(), frame->height());
  background->show();
}

TextDraftsman::~TextDraftsman(){
  delete background;
}

void TextDraftsman::setTransformationMatrix(const QMatrix4x4 *const _transformationMatrix){
  modelVehicle = _transformationMatrix;
}

void TextDraftsman::translateMatrix(const float _x, const float _y, const float _z){
  modelMatrix.translate(-_y, _z, -_x);
}

void TextDraftsman::rotateMatrix(const float _pitch, const float _yaw, const float _roll){
  modelMatrix.rotate(QQuaternion::fromEulerAngles(_pitch * toDEGREES, _yaw * toDEGREES, _roll * toDEGREES));
}

void TextDraftsman::updateLabels(){
  if(*onLabels && *visible){
    QVector3D position;
    QPointF point;
    QRectF rect;

    QPixmap bckFrame(frame->width(), frame->height());
    if((background->width() != frame->width()) || (background->height() != frame->height()))
      background->resize(frame->width(), frame->height());

    bckFrame.fill(Qt::transparent);
    ptr.begin(&bckFrame);
    ptr.setRenderHint(QPainter::Antialiasing);
    ptr.setFont(*font);

    for(int i = 0; i < labelList->size(); i++){
      position = QVector3D(-labelList->at(i).positionY, labelList->at(i).positionZ, -labelList->at(i).positionX);
      if(modelVehicle != nullptr)
        position = (*screenMatrix) * (*modelVehicle) * modelMatrix * position;
      else
        position = (*screenMatrix) * modelMatrix * position;
      if(position.z() < 1){
        //point.setX((position[0]/position[2] + 1) * frame->width()/2);
        //point.setY(frame->height() - (position[1]/position[2] + 1) * frame->height()/2);
        point.setX((position[0] + 1) * frame->width()/2);
        point.setY((1 - position[1]) * frame->height()/2);
        if(point.x() < frame->width() && point.x() > 0 && point.y() < frame->height() && point.y() > 0){
          rect.setCoords(point.x() - 100, point.y() - 35, point.x() + 100, point.y() - 6);

          ptr.setPen(QPen(QColor(labelList->at(i).r, labelList->at(i).g, labelList->at(i).b), 2, Qt::SolidLine));
          ptr.drawText(rect, Qt::AlignCenter | Qt::AlignBottom, labelList->at(i).label);

          QPainterPath path;
          path.moveTo(point.x() - 5, point.y() - 5);
          path.lineTo(point);
          path.lineTo(point.x() + 5, point.y() - 5);
          ptr.drawPath(path);
        }
      }
    }
    ptr.end();
    background->setPixmap(bckFrame);
  }else
    background->setPixmap(QPixmap());
}
