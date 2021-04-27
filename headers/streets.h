#ifndef STREETS_H
#define STREETS_H

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <math.h>
#include <vector>
#include <QObject>

#include "headers/definitions.h"
#include "headers/loadmap.h"

using namespace std;

struct streetData{
  QVector3D position; //vertex position
  QVector2D texcoor;  //texture coordinate
};

struct signsData{
  QVector3D position;
  QVector2D translation;
  QVector2D texcoor;
  float texture;
};

class Streets : public QObject
{
  Q_OBJECT
public:
  Streets(QOpenGLShaderProgram *_program, int *_scenaryRotationY);
  ~Streets();

  bool setMapPosition(double *_latitude, double *_longitude);
  bool setMapRange(unsigned int _width = 150, unsigned int _length = 150);
  void setMapTransformationMatrix(QMatrix4x4 *_transformationMatrix = nullptr);
  void translateMap(const float _x = 0, const float _y = 0, const float _z = 0);
  void rotateMap(const float _pitch = 0, const float _yaw = 0, const float _roll = 0);
  QString seeFolderPath();
  bool updateMap();
  bool forceUpdateMap();
  void restartData();
  void transformCoordinates();
  void drawStreets(bool _wtd);
  void loadMap();

  LoadMaps *mapping;
  bool *isInstalling;

private:
  void initTextures(QString image, bool mrd, int inx);

  QOpenGLShaderProgram *m_program;

  unsigned int i_position, i_translation, i_texcoor, i_texture, u_model, u_map, u_mapModel, u_rotation,
  indicesSignalsSize, indicesStreetsSize;
  float lineW;

  QVector<unsigned int> indicesSignals, indicesStreets;
  QVector<streetData> streetPoints, lanesPoints;
  QVector<signsData> signsPoints;
  QVector3D pB[4], pT[4];
  QVector2D tP[4];
  int oP[6];

  QOpenGLBuffer streetBuffer, lanesBuffer, signsBuffer, indexBuffer, indexSignsBuffer;
  QOpenGLTexture *texture[6];

  double *latitude, *longitude, ltMax, lnMax, ltMin, lnMin, dLn, dLt;
  unsigned int mapRangeW, mapRangeL;

  QMatrix4x4 *matrix, emptyMatrix, signalMatrix, mapMatrix;
  int *yrot, yrot2;

  GLuint texture1, texture2, texture3, texture4;

signals:
  void reloadMapMatrix();
};

#endif // STREETS_H
