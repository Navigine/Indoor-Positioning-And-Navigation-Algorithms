/** level_collector.cpp
 *
 * Author: Vasiliy Kosyanchuk <v.kosyanchuk@navigine.com>
 * Copyright (c) 2019 Navigine. All rights reserved.
 *
 */

#include <level_collector.h>

namespace navigine {
namespace navigation_core {

namespace {

XYZTransmitters getTransmittersInLocalCoordinates(
    const GeoPoint& binding,
    const std::vector<Transmitter<GeoPoint3D>>& transmitters,
    const double& altitude)
{
  XYZTransmitters localTransmitters;
  for (const Transmitter<GeoPoint3D>& t: transmitters)
  {
    XYPoint point2D = gpsToLocal(GeoPoint(t.point.latitude, t.point.longitude), binding);
    localTransmitters.emplace_back(
        TransmitterId(t.id.value),
        XYZPoint(point2D.x, point2D.y, altitude + t.point.altitude),
        t.pathlossModel,
        t.type);
  }
  return localTransmitters;
}

Polygon getPolygonInLocalCoordinates(
    const GeoPoint& binding,
    const Polygon& geoPolygon)
{
  Polygon result;
  for (const auto& p : geoPolygon.outer())
  {
    XYPoint pxy = gpsToLocal(GeoPoint(p.x(), p.y()), binding);
    boost::geometry::append(result.outer(), Point(pxy.x, pxy.y));
  }
  for (const auto& innerRing : geoPolygon.inners())
  {
    result.inners().push_back({});
    for (const auto& p : innerRing)
    {
      XYPoint pxy = gpsToLocal(GeoPoint(p.x(), p.y()), binding);
      boost::geometry::append(result.inners().back(), Point(pxy.x, pxy.y));
    }
  }
  boost::geometry::correct(result);
  return result;
}

Box getBBoxInLocalCoordinates(const GeoPoint& binding, const Box& geoBox)
{
  XYPoint pmin = gpsToLocal(GeoPoint(geoBox.min_corner().x(), geoBox.min_corner().y()), binding);
  XYPoint pmax = gpsToLocal(GeoPoint(geoBox.max_corner().x(), geoBox.max_corner().y()), binding);
  return Box(Point(pmin.x, pmin.y), Point(pmax.x, pmax.y));
}

LevelGeometry getGeometryInLocalCoordinates(
    const GeoPoint& binding,
    const LevelGeometry& geolevelGeometry)
{
  Multipolygon localArea;
  for (const auto& poly : geolevelGeometry.allowedArea())
  {
    localArea.push_back(getPolygonInLocalCoordinates(binding, poly));
  }
  boost::geometry::correct(localArea);

  Box localBox = getBBoxInLocalCoordinates(binding, geolevelGeometry.boundingBox());
  boost::geometry::correct(localBox);

  return LevelGeometry{localArea, localBox};
}

} //namespace


std::shared_ptr<LevelCollector> createLevelCollector()
{
  return std::make_shared<LevelCollector>();
}

boost::optional<LevelId> LevelCollector::findLevelByTransmitterId(const TransmitterId &transmitterId) const
{
  for (const Level& level: mLevels)
  {
    if (level.containsTransmitter(transmitterId))
    {
      return level.id();
    }
  }

  return boost::none;
}

const std::vector<Level>& LevelCollector::levels() const
{
  return mLevels;
}

bool LevelCollector::hasLevel(const LevelId& levelId) const
{
  return mLevelsIndices.find(levelId) != mLevelsIndices.end();
}

const Level& LevelCollector::level(const LevelId& levelId) const
{
  return mLevels.at(mLevelsIndices.at(levelId));
}

GeoPoint getBinding(const GeoLevel& geoLevel)
{
  auto geoBox = geoLevel.geometry.boundingBox();
  boost::geometry::correct(geoBox);
  double minLatitude = geoBox.min_corner().x();
  double minLongitude = geoBox.min_corner().y();
  return GeoPoint(minLatitude, minLongitude);
}

void LevelCollector::addGeoLevel(const GeoLevel& geoLevel)
{
  LevelId levelId = LevelId(geoLevel.id);
  GeoPoint binding = getBinding(geoLevel);
  Level level(levelId, binding);

  level.addTransmitters(getTransmittersInLocalCoordinates(binding, geoLevel.transmitters, geoLevel.altitude));
  level.setGeometry(getGeometryInLocalCoordinates(binding, geoLevel.geometry));

  mLevels.push_back(std::move(level));
  mLevelsIndices.insert({levelId, mLevels.size() - 1});
}

} } // namespace navigine::navigation_core
