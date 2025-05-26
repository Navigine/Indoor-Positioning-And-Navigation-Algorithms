#pragma once

#include <string>
#include <vector>

#include "models.h"

#include "detail/area.h"
#include "detail/binary_operation.h"
#include "detail/bounding_box.h"
#include "detail/buffer.h"
#include "detail/centroid.h"
#include "detail/compare.h"
#include "detail/contains.h"
#include "detail/convex_hull.h"
#include "detail/correct.h"
#include "detail/graph.h"
#include "detail/intersection.h"
#include "detail/iterate_edges.h"
#include "detail/iterate_vertices.h"
#include "detail/num_points.h"
#include "detail/point_helper.h"
#include "detail/precision.h"
#include "detail/simplify.h"
#include "detail/valid.h"
#include "detail/wkt_read.h"
#include "detail/wkt_write.h"
#include "detail/type_checks.h"

namespace GEOLIB_NAMESPACE {

template <typename Point>
double precisionX()
{
    return detail::precisionX<Point>();
}

template <typename Point>
double precisionY()
{
    return detail::precisionY<Point>();
}

template <typename Geometry>
unsigned int numPoints(const Geometry& geometry)
{
    return detail::numPoints(geometry);
}

template <typename Geometry>
bool isEmpty(const Geometry& geometry)
{
    return detail::numPoints(geometry) == 0;
}

template <typename Geometry>
double area(const Geometry& geometry)
{
    return detail::area(geometry);
}

template <detail::IsPoint Point>
Point centroid(const Point& point)
{
    return detail::centroid(point);
}

template <typename Geometry>
typename Geometry::point_type centroid(const Geometry& geometry)
{
    return detail::centroid(geometry);
}

template <typename Geometry>
std::string toWkt(const Geometry& geometry, int precision = -1, bool fixed = false)
{
    return detail::toWkt(geometry, precision, fixed);
}

template <typename Geometry>
Geometry fromWkt(const std::string& wkt)
{
    return detail::fromWkt<Geometry>(wkt);
}

template <typename Geometry>
void correct(Geometry& geometry)
{
    detail::correct(geometry);
}

template <typename Geometry>
void sort(Geometry& geometry)
{
    detail::sort(geometry);
}

template <typename Geometry>
void transform(Geometry& geometry, const std::function<void(typename Geometry::point_type& point)>& func)
{
    detail::transform(geometry, func);
}

template <typename Geometry>
int compare(const Geometry& geometry1, const Geometry& geometry2)
{
    return detail::compare(geometry1, geometry2);
}

template <typename Geometry>
bool equal(const Geometry& geometry1, const Geometry& geometry2, double epsilon = 0.0)
{
    return detail::equal(geometry1, geometry2, epsilon);
}

template <typename Geometry>
bool less(const Geometry& geometry1, const Geometry& geometry2)
{
    return detail::less(geometry1, geometry2);
}

template <typename Geometry>
bool greater(const Geometry& geometry1, const Geometry& geometry2)
{
    return detail::greater(geometry1, geometry2);
}

template <typename Geometry1, typename Geometry2>
bool contains(const Geometry1& geometry1, const Geometry2& geometry2, ContainsPolicy policy = ContainsPolicy::INNER_BORDER)
{
    return detail::contains(geometry1, geometry2, policy);
}

template <typename Geometry>
Box<typename Geometry::point_type> boundingBox(const Geometry& geometry)
{
    return detail::boundingBox<Box<typename Geometry::point_type>>(geometry);
}

template <typename Geometry>
void expand(Box<typename Geometry::point_type>& box, const Geometry& geometry)
{
    return detail::expand<Box<typename Geometry::point_type>, Geometry>(box, geometry);
}

template <typename Geometry>
bool isValid(const Geometry& geometry, std::string& message)
{
    return detail::isValid(geometry, message);
}

template <typename Point>
std::vector<Point> convexHull(const std::vector<Point>& points)
{
    return detail::convexHull<Point>(points);
}

template <typename Geometry1, typename Geometry2>
MultiPolygon<typename Geometry1::point_type> intersection(const Geometry1& geometry1, const Geometry2& geometry2)
{
    return detail::binaryOperation(detail::BinaryOperationType::INTERSECTION, geometry1, geometry2);
}

template <typename Geometry1, typename Geometry2>
MultiPolygon<typename Geometry1::point_type> union_(const Geometry1& geometry1, const Geometry2& geometry2)
{
    return detail::binaryOperation(detail::BinaryOperationType::UNION, geometry1, geometry2);
}

template <typename Geometry1, typename Geometry2>
MultiPolygon<typename Geometry1::point_type> difference(const Geometry1& geometry1, const Geometry2& geometry2)
{
    return detail::binaryOperation(detail::BinaryOperationType::DIFFERENCE, geometry1, geometry2);
}

template <detail::IsPoint Point>
MultiPolygon<Point> buffer(const Point& point, double radius, unsigned int numPointsSemiCircle = 18)
{
    return detail::buffer<MultiPolygon<Point>>(point, radius, numPointsSemiCircle);
}

template <typename Geometry>
MultiPolygon<typename Geometry::point_type> buffer(const Geometry& geometry, double radius, unsigned int numPointsSemiCircle = 18)
{
    return detail::buffer<MultiPolygon<typename Geometry::point_type>>(geometry, radius, numPointsSemiCircle);
}

template <typename Geometry>
Geometry simplified(const Geometry& geometry, double nullAreaThreshold)
{
    return detail::simplified(geometry, nullAreaThreshold);
}

} // namespace GEOLIB_NAMESPACE
