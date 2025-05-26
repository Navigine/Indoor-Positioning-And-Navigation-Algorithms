#pragma once

#include "type_checks.h"
#include "util.h"

namespace GEOLIB_NAMESPACE::detail {

template <typename Point>
void printPoint(std::ostream& os, const Point& point)
{
    static_assert(isPoint<Point>());
    os << util::getX(point) << " " << util::getY(point);
}

template <typename Segment>
void printSegment(std::ostream& os, const Segment& segment)
{
    static_assert(isSegment<Segment>());

    using Point = typename Segment::point_type;

    os << "(";
    printPoint<Point>(os, segment.first);
    os << ", ";
    printPoint<Point>(os, segment.second);
    os << ")";
}

template <typename Box>
void printBox(std::ostream& os, const Box& box)
{
    static_assert(isBox<Box>());

    using Point = typename Box::point_type;

    os << "(";
    printPoint<Point>(os, box.minCorner);
    os << ", ";
    printPoint<Point>(os, box.maxCorner);
    os << ")";
}

template <typename LineString>
void printLineString(std::ostream& os, const LineString& linestring)
{
    static_assert(isLineString<LineString>());

    using Point = typename LineString::point_type;

    os << "(";
    for(size_t i = 0; i < linestring.points.size(); ++i) {
        if (i > 0) {
            os << ", ";
        }
        printPoint<Point>(os, linestring.points[i]);
    }
    os << ")";
}

template <typename Ring>
void printRing(std::ostream& os, const Ring& ring)
{
    static_assert(isRing<Ring>());

    using Point = typename Ring::point_type;

    os << "(";
    for(size_t i = 0; i < ring.points.size(); ++i) {
        if (i > 0) {
            os << ", ";
        }
        printPoint<Point>(os, ring.points[i]);
    }
    os << ")";
}

template <typename Polygon>
void printPolygon(std::ostream& os, const Polygon& poly)
{
    static_assert(isPolygon<Polygon>());

    using Ring = typename Polygon::ring_type;

    os << "(";
    printRing(os, poly.outer);

    for(const auto& inner : poly.inners) {
        os << ", ";
        printRing<Ring>(os, inner);
    }

    os << ")";
}

template <typename MultiPolygon>
void printMultiPolygon(std::ostream& os, const MultiPolygon& mpoly)
{
    static_assert(isMultiPolygon<MultiPolygon>());

    using Polygon = typename MultiPolygon::polygon_type;

    os << "(";

    for(size_t i = 0; i < mpoly.parts.size(); ++i) {
        if (i > 0) {
            os << ", ";
        }
        printPolygon<Polygon>(os, mpoly.parts[i]);
    }

    os << ")";
}

template <typename Geometry>
std::string toWkt(const Geometry& geometry, int precision = -1, bool fixed = false)
{
    std::stringstream ss;

    if (precision >= 0) {
        ss << std::setprecision(precision);
    }

    if (fixed) {
        ss << std::fixed;
    }

    static_assert(
        isPoint<Geometry>() ||
        isBox<Geometry>() ||
        isSegment<Geometry>() ||
        isLineString<Geometry>() ||
        isRing<Geometry>() ||
        isPolygon<Geometry>() ||
        isMultiPolygon<Geometry>() ||
        isGeometry<Geometry>());

    if constexpr (isPoint<Geometry>()) {
        ss << "POINT(";
        detail::printPoint(ss, geometry);
        ss << ")";
    } else if constexpr (isBox<Geometry>()) {
        ss << Geometry::geometry_type;
        detail::printBox(ss, geometry);
    } else if constexpr (isSegment<Geometry>()) {
        ss << Geometry::geometry_type;
        detail::printSegment(ss, geometry);
    } else if constexpr (isLineString<Geometry>()) {
        ss << Geometry::geometry_type;
        detail::printLineString(ss, geometry);
    } else if constexpr (isRing<Geometry>()) {
        ss << Geometry::geometry_type;
        detail::printRing(ss, geometry);
    } else if constexpr (isPolygon<Geometry>()) {
        ss << Geometry::geometry_type;
        detail::printPolygon(ss, geometry);
    } else if constexpr (isMultiPolygon<Geometry>()) {
        ss << Geometry::geometry_type;
        detail::printMultiPolygon(ss, geometry);
    } else if constexpr (isGeometry<Geometry>()) {
        return std::visit(
            [precision, fixed](const auto& arg) {
                return detail::toWkt(arg, precision, fixed);
            }, geometry.geometry);
    }

    return ss.str();
}

} // namespace GEOLIB_NAMESPACE::detail
