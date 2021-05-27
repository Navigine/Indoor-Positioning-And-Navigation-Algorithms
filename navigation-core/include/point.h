/** point.h
 *
 * Author: Aleksandr Galov <a.galov@navigine.com>
 * Copyright (c) 2017 Navigine. All rights reserved.
 *
 */

#ifndef NAVIGINE_POINT_H
#define NAVIGINE_POINT_H

namespace navigine {
namespace navigation_core {

struct GeoPoint
{
  GeoPoint()
    : latitude ( 0.0 )
    , longitude( 0.0 )
  { }

  GeoPoint(double _latitude, double _longitude)
    : latitude ( _latitude  )
    , longitude( _longitude )
  { }

  double latitude;
  double longitude;
};

struct GeoPoint3D
{
  GeoPoint3D()
    : latitude ( 0.0 )
    , longitude( 0.0 )
    , altitude ( 0.0 )
  { }

  GeoPoint3D(double _latitude, double _longitude, double _altitude)
    : latitude ( _latitude  )
    , longitude( _longitude )
    , altitude ( _altitude )
  { }

  double latitude;
  double longitude;
  double altitude;
};

struct XYPoint
{
  XYPoint()
    : x( 0.0 )
    , y( 0.0 )
  { }

  XYPoint(double _x, double _y)
    : x( _x )
    , y( _y )
  { }

  double x;
  double y;
};

struct XYZPoint
{
  XYZPoint()
    : x( 0.0 )
    , y( 0.0 )
    , z( 0.0 )
  { }

  XYZPoint(double _x, double _y, double _z)
    : x( _x )
    , y( _y )
    , z( _z )
  { }

  double x;
  double y;
  double z;
};

XYPoint gpsToLocal(const GeoPoint& point, const GeoPoint& bindPoint);
GeoPoint localToGps(const XYPoint& localPoint, const GeoPoint& bindPoint);

} } // namespace navigine::navigation_core

#endif // NAVIGINE_POINT
