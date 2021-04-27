#ifndef TEXTDRAFTSMAN_H
#define TEXTDRAFTSMAN_H

#include <QFrame>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QMatrix4x4>
#include <QObject>

#include "headers/definitions.h"

class TextDraftsman : public QObject
{
  Q_OBJECT
public:
  TextDraftsman(QFrame *_frame, const vector<XYZLabel> *const _labels, const QMatrix4x4 *const _screenMatrix,
                QFont *_font, bool *_onLabels, bool *_visible);
  ~TextDraftsman();
  void setTransformationMatrix(const QMatrix4x4 *const _transformationMatrix);
  void translateMatrix(const float _x, const float _y, const float _z);
  void rotateMatrix(const float _pitch, const float _yaw, const float _roll);

public slots:
  void updateLabels();

private:
  QFrame *frame;
  QLabel *background;
  const QMatrix4x4 *const screenMatrix;
  const vector<XYZLabel> *const labelList;
  const QMatrix4x4 *modelVehicle;
  QMatrix4x4 modelMatrix;

  QPainter ptr;
  QFont *font;
  bool *onLabels, *visible;
};

#endif // TEXTDRAFTSMAN_H
