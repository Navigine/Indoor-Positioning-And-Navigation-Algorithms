#pragma once

#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace GEOLIB_NAMESPACE {

namespace util {

// Reimplement these functions for your own point type
template <typename Point>
double getX(const Point& point) = delete;

template <typename Point>
double getY(const Point& point) = delete;

template <typename Point>
double& getX(Point& point) = delete;

template <typename Point>
double& getY(Point& point) = delete;

// The followig values set the limits to the point coordinates which may be used in binary operations
// such as union, intersection, difference, buffer. This also affects the resulting precision of these
// operations, i.e.:
//  * range [-1, 1] stands for the precision 1.1e-16
//  * range [-10, 10] stands for the precision 1.1e-15
//  * range [-100, 100] stands for the precision 1.1e-14
//  * range [-1e3, 1e3] stands for the precision 1.1e-13
//  * range [-1e4, 1e4] stands for the precision 1.1e-12
//  * range [-1e5, 1e5] stands for the precision 1.1e-11
//  * range [-1e6, 1e6] stands for the precision 1.1e-10
//  * range [-1e7, 1e7] stands for the precision 1.1e-9
//  * range [-1e8, 1e8] stands for the precision 1.1e-8
//  ...
template <typename Point>
double maxX() {
    // This allows to measure Earth's surface in meters using the point coordinates.
    // The resulting precision: 5e-9 (at least 8 correct digits after the decimal point).
    return 5e7;
}

template <typename Point>
double maxY() {
    // This allows to measure Earth's surface in meters using the point coordinates.
    // The resulting precision: 5e-9 (at least 8 correct digits after the decimal point).
    return 5e7;
}

} // namespace util

template <typename Point>
struct Segment
{
    using point_type = Point;
    static constexpr auto geometry_type = "SEGMENT";
    point_type first;
    point_type second;
};

template <typename Point>
struct Box
{
    using point_type = Point;
    static constexpr auto geometry_type = "BOX";
    point_type minCorner;
    point_type maxCorner;
};

template <typename Point>
struct LineString
{
    using point_type = Point;
    static constexpr auto geometry_type = "LINESTRING";
    std::vector<point_type> points;
};

template <typename Point>
struct Ring
{
    using point_type = Point;
    static constexpr auto geometry_type = "LINEARRING";
    std::vector<point_type> points;
};

template <typename Point>
struct Polygon
{
    using point_type = Point;
    using ring_type = Ring<Point>;
    static constexpr auto geometry_type = "POLYGON";
    ring_type outer;
    std::vector<ring_type> inners;
};

template <typename Point>
struct MultiPolygon
{
    using point_type = Point;
    using ring_type = Ring<Point>;
    using polygon_type = Polygon<Point>;
    static constexpr auto geometry_type = "MULTIPOLYGON";
    std::vector<polygon_type> parts;
};

template <typename Point>
struct Geometry
{
    using point_type = Point;
    static constexpr auto geometry_type = "GEOMETRY";
    std::variant<
        Point,
        Segment<Point>,
        Box<Point>,
        LineString<Point>,
        Ring<Point>,
        Polygon<Point>,
        MultiPolygon<Point>> geometry;
};

enum class ContainsPolicy {
    INNER,
    INNER_BORDER,
    INNER_BORDER_FINITE,
    OUTER,
    OUTER_BORDER,
    OUTER_BORDER_FINITE,
};

class Exception : public std::runtime_error
{
public:
    Exception(const std::string& what = "")
        : std::runtime_error(what)
    {}
};

} // namespace GEOLIB_NAMESPACE
