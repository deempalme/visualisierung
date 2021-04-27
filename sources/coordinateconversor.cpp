#include "headers/coordinateconversor.h"

CoordinateConversor::CoordinateConversor(double *_latitude, double *_longitude) :
  latitude(_latitude),
  longitude(_longitude)
{
//  qWarning("initial coordinates %.9f %.9f", latitude, longitude);
}

QVector2D CoordinateConversor::GPStoXY(double _latitude, double _longitude){
  _latitude *= toRADIANS;
  _longitude *= toRADIANS;

  double latitudeRAD(*latitude * toRADIANS);
  double longitudeRAD(*longitude * toRADIANS);

  double A(cos(latitudeRAD));
  double B(sin(latitudeRAD));
  double C(cos(_latitude));
  double D(sin(_latitude));
  double E(cos(_longitude - longitudeRAD));

  double c(acos(B * D + A * C * E));
  double k(c / sin(c));

  float x(k * C * sin(_longitude - longitudeRAD) * 6378137);
  float y(k * (A * D - B * C * E) * 6378137);

  if(x != x) x = 0;
  if(y != y) y = 0;

  return QVector2D(x, y);
}

QVector2D CoordinateConversor::XYtoGPS(double _x, double _y){
  _x /= 6378137.0;
  _y /= 6378137.0;

  double c(sqrt(pow(_x, 2) + pow(_y, 2)));
  double latitudeRAD(*latitude * toRADIANS);
  double A(cos(latitudeRAD));
  double B(sin(latitudeRAD));
  double C(sin(c));
  double D(cos(c));

  float _latitude(asin(D * B + (_y * C * A) / c) * toDEGREES);
  float _longitude;

  if(*latitude == 90)
    _longitude = *longitude + atan(-_x/_y) * toDEGREES;
  else if(*latitude == -90)
    _longitude = *longitude + atan(_x/_y) * toDEGREES;
  else
    _longitude = *longitude + atan((_x * C)/(c * A * D - _y * B * C)) * toDEGREES;

  return QVector2D(_latitude, _longitude);
}

double CoordinateConversor::distance(double _startLatitude, double _startLongitude, double _endLatitude, double _endLongitude){
  _startLatitude *= toRADIANS;
  _startLongitude *= toRADIANS;
  _endLatitude *= toRADIANS;
  _endLongitude *= toRADIANS;

  double A(cos(_startLatitude));
  double B(sin(_startLatitude));
  double C(cos(_endLatitude));
  double D(sin(_endLatitude));
  double E(cos(_endLongitude - _startLongitude));

  double c(acos(B * D + A * C * E));
  double k(c / sin(c));

  float x(k * C * sin(_endLongitude - _startLongitude) * 6378137);
  float y(k * (A * D - B * C * E) * 6378137);

  if(x != x) x = 0;
  if(y != y) y = 0;

  return double(sqrt(pow(x, 2) + pow(y, 2)));
}

vector2d CoordinateConversor::distances(double _startLatitude, double _startLongitude, double _endLatitude, double _endLongitude){
  vector2d xy;

  _startLatitude *= toRADIANS;
  _startLongitude *= toRADIANS;
  _endLatitude *= toRADIANS;
  _endLongitude *= toRADIANS;

  double A(cos(_startLatitude));
  double B(sin(_startLatitude));
  double C(cos(_endLatitude));
  double D(sin(_endLatitude));
  double E(cos(_endLongitude - _startLongitude));

  double c(acos(B * D + A * C * E));
  double k(c / sin(c));

  float x(k * C * sin(_endLongitude - _startLongitude) * 6378137);
  float y(k * (A * D - B * C * E) * 6378137);

  if(x != x) x = 0;
  if(y != y) y = 0;

  xy.x = x;
  xy.y = y;

  return xy;
}
