/** level_collector.cpp
 *
 * Copyright (c) 2019 Navigine.
 *
 */

#include <navigine/navigation-core/level_collector.h>

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

XYReferencePoints getRefPointsInLocalCoordinates(
    const GeoPoint& binding,
    const std::vector<ReferencePoint<GeoPoint>>& referencePoints)
{
  XYReferencePoints localReferencePoints;
  for (const ReferencePoint<GeoPoint>& rp: referencePoints)
  {
    localReferencePoints.emplace_back(
        ReferencePointId(rp.id.value),
        gpsToLocal(rp.point, binding),
        rp.fingerprints);
  }
  return localReferencePoints;
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

Graph<XYPoint> getGraphInLocalCoordinates(
  const GeoPoint& binding,
  const Graph<GeoPoint>& geoGraph)
{
  Graph<XYPoint> xyGraph;
  std::map<int, int> geoToXyVertexIds;
  for (Graph<GeoPoint>::VertexIterator it = geoGraph.vertexBegin(); it != geoGraph.vertexEnd(); it++)
  {
    Graph<GeoPoint>::Vertex geoVertex = *it;
    XYPoint p = gpsToLocal(geoVertex.point, binding);
    Graph<XYPoint>::Vertex xyVertex = xyGraph.addVertex(p);
    geoToXyVertexIds[geoVertex.id] = xyVertex.id;
  }

  for (Graph<GeoPoint>::EdgeIterator it = geoGraph.edgesBegin(); it != geoGraph.edgesEnd(); it++)
  {
    Graph<GeoPoint>::Vertex geoV1 = geoGraph.getVertex(it->first);
    Graph<GeoPoint>::Vertex geoV2 = geoGraph.getVertex(it->second);
    int xyV1 = geoToXyVertexIds[geoV1.id];
    int xyV2 = geoToXyVertexIds[geoV2.id];
    xyGraph.addEdge(xyV1, xyV2);
  }
  return xyGraph;
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

  level.setReferencePoints(getRefPointsInLocalCoordinates(binding, geoLevel.referencePoints));
  level.setGraph(getGraphInLocalCoordinates(binding, geoLevel.graph));

  mLevels.push_back(std::move(level));
  mLevelsIndices.insert({levelId, mLevels.size() - 1});
}

} } // namespace navigine::navigation_core
