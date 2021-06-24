/** point.cpp
 *
 * Copyright (c) 2019 Navigine.
 *
 */

#include <navigine/navigation-core/point.h>
#include <cmath>

namespace navigine {
namespace navigation_core {

constexpr double EQUATOR_CIRCUMFERENCE_METERS = 40075160.0;
constexpr double POLE_CIRCUMFERENCE_METERS = 40008000.0;

// Normalize longitude to (-180, 180]
double normalizeLongitude(double lon)
{
  if (lon > 180)
  {
    lon -= 360;
  } else if (lon <= -180)
  {
    lon += 360;
  }
  return lon;
}

double degToRad(double deg)
{
  return deg * M_PI / 180.0;
}

XYPoint gpsToLocal(const GeoPoint& point, const GeoPoint& bindPoint)
{
  const double deltaLat = point.latitude - bindPoint.latitude;
  const double deltaLon = normalizeLongitude(point.longitude - bindPoint.longitude);
  const double latitudeCircumference =
    EQUATOR_CIRCUMFERENCE_METERS * std::cos(degToRad(bindPoint.latitude));

  XYPoint localPoint;
  localPoint.x = deltaLon * (latitudeCircumference / 360.0);
  localPoint.y = deltaLat * (POLE_CIRCUMFERENCE_METERS / 360.0);
  return localPoint;
}

GeoPoint localToGps(const XYPoint& localPoint, const GeoPoint& bindPoint)
{
  const double latitudeCircumference = EQUATOR_CIRCUMFERENCE_METERS * std::cos(degToRad(bindPoint.latitude));
  const double lat = (localPoint.y * (360.0 / POLE_CIRCUMFERENCE_METERS)) + bindPoint.latitude;
  const double lon = normalizeLongitude(localPoint.x * 360.0 / latitudeCircumference + bindPoint.longitude);
  return GeoPoint(lat, lon);
}

} } // namespace navigine::navigation_core
