#ifndef UIELEMENTS_H
#define UIELEMENTS_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>
#include <QStyle>
#include <QFont>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QDateTime>
#include <math.h>
#include <QLineEdit>
#include <QIntValidator>
#include <QTimer>
#include <QDir>

#include "headers/parameters.h"
#include "headers/definitions.h"

class NewSimpleElement : public QFrame
{
  Q_OBJECT

public:
  explicit NewSimpleElement(QWidget *_parent = NULL, const int _type = VERTICALBAR, const QString _name = "", const int _decimals = 0,
                            const double _max = 1, const double _min = 0, double *_input = nullptr, QVector<QFont*> *_fonts = nullptr,
                            const float _positionX = 0, const float _positionY = 0, const float _horizontalAlignment = LEFT,
                            const float _verticalAlignment = BOTTOM);
  ~NewSimpleElement();

  void changeName(const QString _name);
  void changeType(const int _type = VERTICALBAR);
  void changeInput(double *_input);
  void changePosition(const float _positionX = UNCHANGED, const float _positionY = UNCHANGED);
  void changeAlignment(const float _horizontalAlignment = UNCHANGED, const float _verticalAlignment = UNCHANGED);
  void changeMaxMin(const double _max = UNCHANGED, const double _min = UNCHANGED);
  void update();

private slots:
  void setPosition();

private:
  void setElement();
  void setSteering();
  void setBattery();
  void setVerticalBar();
  void modLabel(QLabel *lbl, int x, int y, int w, int h, int i = -1, QString s = NULL);

  QWidget *parent;

  int type, positionX, positionY, relativePositionX, relativePositionY, width,
  height, horizontalAlignment, verticalAlignment, decimals;
  double max, min, *input;
  QString name;

  QVector<QLabel*> labels;
  QVector<QFont*> *fonts;

  QPainter ptr;
  QPixmap x_steering, o_steering;
  float a_steering, v_bar;
};

//-------------------------------------------------------------------------------------------------------------//
//                                          speedometer UI element                                             //
//-------------------------------------------------------------------------------------------------------------//

class NewSpeedo : public QFrame
{
  Q_OBJECT
public:
  explicit NewSpeedo(QWidget *_parent = NULL, const QString _name = "", const double _maxSpeed = 100,
                     const double _minSpeed = 0, const double _maxRPM = 6000, const double _minRPM = 0,
                     double *_inputSpeed = nullptr, double *_inputRPM = nullptr, QVector<QFont*> *_fonts = nullptr,
                     const float _positionX = 0, const float _positionY = 0, const float _horizontalAlignment = LEFT,
                     const float _verticalAlignment = BOTTOM);
  ~NewSpeedo();

  void changeName(const QString _name);
  void changeInputs(double *_inputSpeed = nullptr, double *_inputRPM = nullptr);
  void changePosition(const float _positionX = UNCHANGED, const float _positionY = UNCHANGED);
  void changeAlignment(const float _horizontalAlignment = UNCHANGED, const float _verticalAlignment = UNCHANGED);
  void changeMaxMin(const double _maxSpeed = UNCHANGED, const double _minSpeed = UNCHANGED, const double _maxRPM = UNCHANGED,
                    const double _minRPM = UNCHANGED);
  void update();

private slots:
  void setPosition();

private:
  void setElement();
  void modLabel(QLabel *lbl, int x, int y, int w, int h, int i = -1, QString s = NULL);

  QWidget *parent;

  int positionX, positionY, relativePositionX, relativePositionY, width, height, horizontalAlignment, verticalAlignment;
  double maxSpeed, minSpeed, maxRPM, minRPM, *inputSpeed, *inputRPM, relationSpeed, relationRPM;

  QString name;

  QVector<QLabel*> labels;
  QVector<QFont*> *fonts;

  QPainter ptr;
  QPixmap x_speed, x_rpm, i_speed, i_rpm;
  float a_speed, a_rpm;
};

//-------------------------------------------------------------------------------------------------------------//
//                                            gearbox UI element                                               //
//-------------------------------------------------------------------------------------------------------------//

class NewGearbox : public QFrame
{
  Q_OBJECT

public:
  explicit NewGearbox(QWidget *_parent = NULL, const int _type = AUTOMATIC, const QString _name = "",
                      vector<nameANDvalue> *_values = NULL, double *_input = NULL,
                      QVector<QFont*> *_fonts = nullptr, const float _positionX = 0, const float _positionY = 0,
                      const float _horizontalAlignment = LEFT, const float _verticalAlignment = BOTTOM);
  ~NewGearbox();

  void changeName(const QString _name);
  void changeType(const int _type = AUTOMATIC);
  void changeInput(double *_input);
  void changePosition(const float _positionX = UNCHANGED, const float _positionY = UNCHANGED);
  void changeAlignment(const float _horizontalAlignment = UNCHANGED, const float _verticalAlignment = UNCHANGED);
  void changeLabels(vector<nameANDvalue> *_values = nullptr);
  void update();

private slots:
  void setPosition();

private:
  void setElement();
  void setManual();
  void setAutomatic();
  void modLabel(QLabel *lbl, int x, int y, int w, int h, int i = -1, QString s = NULL);

  QWidget *parent;

  int type, positionX, positionY, relativePositionX, relativePositionY,
  width, height, horizontalAlignment, verticalAlignment;
  float verticalSeparation;
  double *input;
  QString name;
  vector<nameANDvalue> *labelValues;

  QVector<QLabel*> labels;
  QVector<QFont*> *fonts;
};

//-------------------------------------------------------------------------------------------------------------//
//                                           text list UI element                                              //
//-------------------------------------------------------------------------------------------------------------//

class NewTextList : public QFrame
{
  Q_OBJECT

public:
  explicit NewTextList(QWidget *_parent = NULL, const QString _name = "",
                       vector<nameANDdata> *_values = NULL, QVector<QFont*> *_fonts = NULL,
                       const float _positionX = 0, const float _positionY = 0, const float _width = 210,
                       const float _height = 150, const float _horizontalAlignment = LEFT,
                       const float _verticalAlignment = BOTTOM);
  ~NewTextList();

  void changeName(const QString _name);
  void changePosition(const float _positionX = UNCHANGED, const float _positionY = UNCHANGED);
  void changeAlignment(const float _horizontalAlignment = UNCHANGED, const float _verticalAlignment = UNCHANGED);
  void changeInputs(vector<nameANDdata> *_values = NULL);
  void changeSize(const float _width = UNCHANGED, const float _height = UNCHANGED);
  void update();

private slots:
  void setPosition();

private:
  void setElement();
  void setList();
  void modLabel(QLabel *lbl, int x, int y, int w, int h, int i = -1, QString s = NULL);
  void setAbsoluteSize();

  QWidget *parent;

  int positionX, positionY, relativePositionX, relativePositionY,
  width, height, horizontalAlignment, verticalAlignment;
  float width2, height2;

  QString name;
  vector<nameANDdata> *labelValues;

  QVector<QLabel*> labels;
  QScrollArea *s_container;
  QFrame *f_container;
  QVector<QFont*> *fonts;
};

//-------------------------------------------------------------------------------------------------------------//
//                                           icon list UI element                                              //
//-------------------------------------------------------------------------------------------------------------//

class NewIconList : public QFrame
{
  Q_OBJECT

public:
  explicit NewIconList(QWidget *_parent = NULL, const QString _name = "",
                       vector<nameANDboolen> *_values = nullptr, QVector<QFont*> *_fonts = nullptr,
                       const float _positionX = 0, const float _positionY = 0, const float _width = 135,
                       const float _height = 150, const float _horizontalAlignment = LEFT,
                       const float _verticalAlignment = BOTTOM);
  ~NewIconList();

  void changeName(const QString _name);
  void changePosition(const float _positionX = UNCHANGED, const float _positionY = UNCHANGED);
  void changeAlignment(const float _horizontalAlignment = UNCHANGED, const float _verticalAlignment = UNCHANGED);
  void changeInputs(vector<nameANDboolen> *_values = nullptr);
  void changeSize(const float _width = UNCHANGED, const float _height = UNCHANGED);
  void update();

private slots:
  void setPosition();

private:
  void setElement();
  void setList();
  void modLabel(QLabel *lbl, int x, int y, int w, int h, int i = -1, QString s = NULL);
  void setAbsoluteSize();

  QWidget *parent;

  int positionX, positionY, relativePositionX, relativePositionY,
  width, height, horizontalAlignment, verticalAlignment;
  float width2, height2;

  QString name;
  vector<nameANDboolen> *labelValues;

  QVector<QLabel*> labels;
  QScrollArea *s_container;
  QFrame *f_container;
  QVector<QFont*> *fonts;
};

//-------------------------------------------------------------------------------------------------------------//
//                                            messages UI element                                              //
//-------------------------------------------------------------------------------------------------------------//

class NewMessages : public QFrame
{
  Q_OBJECT

public:
  explicit NewMessages(QWidget *_parent = NULL, const QString _name = "", QVector<QFont*> *_fonts = nullptr,
                       const float _positionX = 0, const float _positionY = 0, const float _width = -500,
                       const float _height = 150, const float _horizontalAlignment = LEFT,
                       const float _verticalAlignment = BOTTOM);
  ~NewMessages();

  void changeName(const QString _name);
  void changePosition(const float _positionX = UNCHANGED, const float _positionY = UNCHANGED);
  void changeAlignment(const float _horizontalAlignment = UNCHANGED, const float _verticalAlignment = UNCHANGED);
  void changeSize(const float _width = UNCHANGED, const float _height = UNCHANGED);
  void addMessage(const QString _message = "", const int _type = WARNING_M);
  void clearMessages();

private slots:
  void setPosition();

private:
  void setElement();
  void setList();
  void modLabel(QLabel *lbl, int x, int y, int w, int h, int p, int i = -1, QString s = NULL);
  void setAbsoluteSize();

  QWidget *parent;

  int positionX, positionY, relativePositionX, relativePositionY,
  width, height, horizontalAlignment, verticalAlignment;
  float width2, height2;

  QString name;

  QVector<QLabel*> labels, labels2, labels3;
  QScrollArea *s_container;
  QFrame *f_container;
  QVector<QFont*> *fonts;
};

//-------------------------------------------------------------------------------------------------------------//
//                                            signaling UI element                                             //
//-------------------------------------------------------------------------------------------------------------//

class NewSignaling : public QFrame
{
  Q_OBJECT

public:
  explicit NewSignaling(QWidget *_parent = NULL, QVector<QFont*> *_fonts = nullptr, const float _positionX = 10,
                        const float _positionY = 110, const float _width = 50, const float _height = -145,
                        const float _horizontalAlignment = RIGHT, const float _verticalAlignment = TOP);
  ~NewSignaling();

  void changePosition(const float _positionX = UNCHANGED, const float _positionY = UNCHANGED);
  void changeAlignment(const float _horizontalAlignment = UNCHANGED, const float _verticalAlignment = UNCHANGED);
  void changeSize(const float _width = UNCHANGED, const float _height = UNCHANGED);
  void addSignal(const int _type = SEMAPHORE, const int _numericalValue = GREEN);
  void clearSignals();

private slots:
  void setPosition();

private:
  void setElement();
  void setAbsoluteSize();
  void modLabel(QLabel *lbl, int x, int y, int w, int h, int i, QString s = NULL);

  QWidget *parent;

  int positionX, positionY, relativePositionX, relativePositionY, width, height,
  horizontalAlignment, verticalAlignment, relativeHeight;
  float width2, height2;

  QVector<QLabel*> labels;
  QVector<QFont*> *fonts;
};

//-------------------------------------------------------------------------------------------------------------//
//                                              clock UI element                                               //
//-------------------------------------------------------------------------------------------------------------//

class NewClock : public QFrame
{
  Q_OBJECT

public:
  explicit NewClock(QWidget *_parent = NULL, int *_whichCompany = nullptr, const QString *const _timestamp = nullptr,
                    QVector<QFont*> *_fonts = nullptr, const float _positionX = 10, const float _positionY = 10,
                    const float _width = 250, const float _height = 70, const float _horizontalAlignment = LEFT,
                    const float _verticalAlignment = TOP);
  ~NewClock();

  void changePosition(const float _positionX = UNCHANGED, const float _positionY = UNCHANGED);
  void changeAlignment(const float _horizontalAlignment = UNCHANGED, const float _verticalAlignment = UNCHANGED);
  void changeSize(const float _width = UNCHANGED, const float _height = UNCHANGED);
  void update();

  QPushButton *logo;

private slots:
  void setPosition();

private:
  void setElement();
  void modLabel(QLabel *lbl, int x, int y, int w, int h, int i, QString s = NULL);
  void setAbsoluteSize();

  QWidget *parent;

  int positionX, positionY, relativePositionX, relativePositionY,
  width, height, horizontalAlignment, verticalAlignment;
  float width2, height2;

  int *whichCompany;

  QVector<QLabel*> labels;
  QVector<QFont*> *fonts;
  const QString *const timestamp;

  QDateTime dateTime;

public slots:
  void change();
};

//-------------------------------------------------------------------------------------------------------------//
//                                             compass UI element                                              //
//-------------------------------------------------------------------------------------------------------------//

class NewCompass : public QFrame
{
  Q_OBJECT

public:
  explicit NewCompass(QWidget *_parent = NULL, double *_angleVehicle = nullptr, double *_angleFrame = nullptr,
                      int *_angleScenary = nullptr, QVector<QFont*> *_fonts = nullptr, const float _positionX = 0,
                      const float _positionY = 10, const float _width = 0.7, const float _height = 40,
                      const float _horizontalAlignment = CENTER, const float _verticalAlignment = TOP);
  ~NewCompass();

  void changePosition(const float _positionX = UNCHANGED, const float _positionY = UNCHANGED);
  void changeAlignment(const float _horizontalAlignment = UNCHANGED, const float _verticalAlignment = UNCHANGED);
  void changeSize(const float _width = UNCHANGED, const float _height = UNCHANGED);
  void setVehicleYaw(double *_yawAngleVehicle);
  void setFrameYaw(double *_yawAngleFrame);

public slots:
  void update();

private slots:
  void setPosition();

private:
  void setElement();
  void modLabel(QLabel *lbl, int x, int y, int w, int h, int i, QString s = NULL);
  void setAbsoluteSize();

  QWidget *parent;

  int positionX, positionY, relativePositionX, relativePositionY,
  width, height, horizontalAlignment, verticalAlignment;

  float width2, height2, lcs3, lcs30, lcs1;
  double *angleVehicle, *angleFrame;
  int *angleScenary;

  QVector<QLabel*> labels;
  QVector<QFont*> *fonts;
  QPixmap pxm;
  QPainter ptr;
};

//-------------------------------------------------------------------------------------------------------------//
//                                            controls UI element                                              //
//-------------------------------------------------------------------------------------------------------------//

class NewControls : public QFrame
{
  Q_OBJECT

public:
  explicit NewControls(QWidget *_parent = NULL, unsigned int _initialDataset = 0, unsigned int _initialFrame = 0,
                       unsigned int _initialDelayment = 70, bool *_options = nullptr, QVector<QFont*> *_fonts = nullptr);
  ~NewControls();

  bool gotoDataset(unsigned int _dataset);
  bool gotoFrame(unsigned int _frame);
  void setDelayment(unsigned int _delay);
  void pause();
  unsigned int getFrame();
  unsigned int getDataset();

private slots:
  void setPosition();

private:
  void setElement();
  void modLabel(QLabel *lbl, int x, int y, int w, int h, int i, QString s = NULL);
  void modLabel(QPushButton *lbl, int x, int y, int w, int h, int i, QString s = NULL);
  void modLabel(QLineEdit *lbl, int x, int y, int w, int h, int i, QString s = NULL);

  QWidget *parent;

  unsigned int dataset, frame, totalFrames, delayment;
  bool *options, isPlayed, exist;

  QString cssButton, cssFrame;

  QVector<QLabel*> labels;
  QVector<QPushButton*> buttons;
  QVector<QLineEdit*> editors;
  QVector<QFont*> *fonts;

  QIntValidator *validator;
  QTimer *timer;
  QVector<unsigned int> datasets;
  unsigned int positionDataset, sizeDatasets;

public slots:
  void nextFrame();
  void previousFrame();
  void firstFrame();
  void lastFrame();
  void gotoFrame();

  void nextDataset();
  void previousDataset();
  void gotoDataset();

  void setDelayment();
  void play();

  void firstShot();

signals:
  void updateDataset(unsigned int _dataset);
  void updateFrame(unsigned int _frame);
  void updateData();
};

//-------------------------------------------------------------------------------------------------------------//
//                                              camera UI element                                              //
//-------------------------------------------------------------------------------------------------------------//

class NewCamera : public QFrame
{
  Q_OBJECT

public:
  explicit NewCamera(QWidget *_parent = NULL, QString _name = "Camera 02", unsigned int _initialDataset = 0,
                     unsigned int _initialFrame = 0, unsigned int _cameraSet = 2, const float _width = 1242,
                     const float _height = 375, const float _positionX = 0, const float _positionY = 0,
                     const float _horizontalAlignment = CENTER, const float _verticalAlignment = BOTTOM,
                     QVector<QFont*> *_fonts = nullptr);
  ~NewCamera();

  void changeName(const QString _name);
  void changeSize(const float _width = 1242, const float _height = 375);
  void changePosition(const float _positionX = UNCHANGED, const float _positionY = UNCHANGED);
  void changeAlignment(const float _horizontalAlignment = UNCHANGED, const float _verticalAlignment = UNCHANGED);
  void update();

  static bool *onCamera;

private slots:
  void setPosition();

private:
  void setElement();
  void modLabel(QLabel *lbl, int x, int y, int w, int h, int i, QString s = NULL);
  void setAbsoluteSize();

  QWidget *parent;

  unsigned int dataset, frame, cameraSet;

  QString name;

  QVector<QLabel*> labels;
  QVector<QFont*> *fonts;
  QPixmap backCamera;

  int positionX, positionY, relativePositionX, relativePositionY,
  width, height, horizontalAlignment, verticalAlignment;
  float width2, height2;

public slots:
  void gotoFrame(unsigned int _frame);
  void setDataSet(unsigned int _dataset);
};

#endif // UIELEMENTS_H
