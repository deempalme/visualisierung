#ifndef COORDINATECONVERSOR_H
#define COORDINATECONVERSOR_H

#include <math.h>
#include <QDebug>
#include <QVector2D>

#include "headers/definitions.h"

class CoordinateConversor
{
public:
  CoordinateConversor(double *_latitude, double *_longitude);

  QVector2D GPStoXY(double _latitude, double _longitude);
  QVector2D XYtoGPS(double _x, double _y);
  double distance(double _startLatitude, double _startLongitude, double _endLatitude, double _endLongitude);
  vector2d distances(double _startLatitude, double _startLongitude, double _endLatitude, double _endLongitude);

private:
  double *latitude, *longitude;
};

#endif // COORDINATECONVERSOR_H
