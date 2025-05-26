#pragma once

#include "type_checks.h"
#include "util.h"

namespace GEOLIB_NAMESPACE::detail {

template <typename Point>
double boxArea(const Box<Point>& box)
{
    static_assert(isPoint<Point>());

    return (util::getX(box.maxCorner) - util::getX(box.minCorner)) *
           (util::getY(box.maxCorner) - util::getY(box.minCorner));
}

template <typename Point>
double ringArea(const Ring<Point>& ring)
{
    static_assert(isPoint<Point>());

    double area = 0.0;
    for(size_t i = 0; i + 1 < ring.points.size(); ++i) {
        const auto& p1 = ring.points[i];
        const auto& p2 = ring.points[i+1];
        area += (util::getX(p1) - util::getX(p2)) * (util::getY(p1) + util::getY(p2)) / 2;
    }

    return area;
}

template <typename Point>
double polygonArea(const Polygon<Point>& poly)
{
    static_assert(isPoint<Point>());

    double area = ringArea(poly.outer);
    for(const auto& inner : poly.inners) {
        area += ringArea(inner); // Inner rings have negative area
    }
    return area;
}

template <typename Point>
double multiPolygonArea(const MultiPolygon<Point>& geometry)
{
    static_assert(isPoint<Point>());

    double area = 0.0;
    for(const auto& part : geometry.parts) {
        area += polygonArea(part);
    }
    return area;
}

template <typename Point>
double geometryArea(const Geometry<Point>& geometry)
{
    static_assert(isPoint<Point>());

    return std::visit(overloaded{
        [](const Box<Point>& box) {
            return boxArea(box);
        },
        [](const Ring<Point>& ring) {
            return ringArea(ring);
        },
        [](const Polygon<Point>& poly) {
            return polygonArea(poly);
        },
        [](const MultiPolygon<Point>& mpoly) {
            return multiPolygonArea(mpoly);
        },
        [](const auto&) {
            return 0.0;
        }
    }, geometry.geometry);
}

template <typename Geometry>
double area(const Geometry& geometry)
{
    static_assert(
        isPoint<Geometry>() ||
        isSegment<Geometry>() ||
        isLineString<Geometry>() ||
        isBox<Geometry>() ||
        isRing<Geometry>() ||
        isPolygon<Geometry>() ||
        isMultiPolygon<Geometry>() ||
        isGeometry<Geometry>());

    if constexpr (isPoint<Geometry>() || isSegment<Geometry>() || isLineString<Geometry>()) {
        return 0.0;
    } else if constexpr (isBox<Geometry>()) {
        return boxArea(geometry);
    } else if constexpr (isRing<Geometry>()) {
        return ringArea(geometry);
    } else if constexpr (isPolygon<Geometry>()) {
        return polygonArea(geometry);
    } else if constexpr (isMultiPolygon<Geometry>()) {
        return multiPolygonArea(geometry);
    } else if constexpr (isGeometry<Geometry>()) {
        return geometryArea(geometry);
    }
}

} // namespace GEOLIB_NAMESPACE::detail
