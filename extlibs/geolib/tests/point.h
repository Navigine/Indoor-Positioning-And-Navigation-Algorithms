#pragma once

#define GEOLIB_NAMESPACE geolib

#include <iostream>

#include <include/geolib/models.h>
#include <include/geolib/operations.h>

struct XYPoint
{
    double x = 0.0;
    double y = 0.0;
};

namespace geolib::util {

template <> inline double getX(const XYPoint& point) { return point.x; }
template <> inline double getY(const XYPoint& point) { return point.y; }
template <> inline double& getX(XYPoint& point) { return point.x; }
template <> inline double& getY(XYPoint& point) { return point.y; }
template <> inline double maxX<XYPoint>() { return 1000000.0; }
template <> inline double maxY<XYPoint>() { return 1000000.0; }

} // namespace geolib::util

using XYBox = geolib::Box<XYPoint>;
using XYRing = geolib::Ring<XYPoint>;
using XYPolygon = geolib::Polygon<XYPoint>;
using XYMultiPolygon = geolib::MultiPolygon<XYPoint>;
using XYLineString = geolib::LineString<XYPoint>;
using XYSegment = geolib::Segment<XYPoint>;
using XYGeometry = geolib::Geometry<XYPoint>;
