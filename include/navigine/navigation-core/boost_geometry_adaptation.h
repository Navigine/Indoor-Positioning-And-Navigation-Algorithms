#pragma once

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/register/point.hpp>

#include <navigine/navigation-core/point.h>
#include "xy_particle.h"

BOOST_GEOMETRY_REGISTER_POINT_2D(navigine::navigation_core::XYPoint, double, boost::geometry::cs::cartesian, x, y);
BOOST_GEOMETRY_REGISTER_POINT_2D(navigine::navigation_core::XYParticle, double, boost::geometry::cs::cartesian, x, y);

namespace navigine {
namespace navigation_core {

using Point = boost::geometry::model::d2::point_xy<double>;
using Box = boost::geometry::model::box<Point>;
using Ring = boost::geometry::model::ring<Point>;
using Polygon = boost::geometry::model::polygon<Point>;
using Multipolygon = boost::geometry::model::multi_polygon<Polygon>;
using Linestring = boost::geometry::model::linestring<Point>;
using Multilinestring = boost::geometry::model::multi_linestring<Linestring>;
using BoxIdx = std::pair<Box, std::size_t>;
using RTree = boost::geometry::index::rtree<BoxIdx, boost::geometry::index::rstar<16, 4>>;

inline std::pair<double, double> getWidthHeightPair(const Box& box)
{
  double width = box.max_corner().x() - box.min_corner().x();
  double height = box.max_corner().y() - box.min_corner().y();
  return std::make_pair(width, height);
}

} } //namespace navigine::navigation_core
