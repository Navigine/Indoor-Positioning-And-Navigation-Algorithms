#pragma once

#include "type_checks.h"
#include "util.h"

namespace GEOLIB_NAMESPACE::detail {

template <typename Point>
using IterateEdgeFunc = std::function<bool(
    const Point& point1,
    const Point& point2)>;

template <typename Point>
bool iterateEdgesPointsHelper(
    const std::vector<Point>& points,
    const bool reverse,
    const IterateEdgeFunc<Point>& func)
{
    static_assert(isPoint<Point>());

    if (points.size() < 2) {
        return true;
    }

    const auto n = points.size() - 1;

    if (reverse) {
        for(size_t i = n; i > 0; --i) {
            if (!func(points[i], points[i - 1])) {
                return false;
            }
        }
    } else {
        for(size_t i = 0; i < n; ++i) {
            if (!func(points[i], points[i + 1])) {
                return false;
            }
        }
    }

    return true;
}

template <typename Point>
bool iterateEdgesSegment(
    const Segment<Point>& segment,
    const bool reverse,
    const IterateEdgeFunc<Point>& func)
{
    static_assert(isPoint<Point>());
    return reverse ?
        func(segment.second, segment.first) :
        func(segment.first, segment.second);
}

template <typename Point>
bool iterateEdgesBox(
    const Box<Point>& box,
    const bool reverse,
    const IterateEdgeFunc<Point>& func)
{
    static_assert(isPoint<Point>());

    Point p1, p2, p3, p4;
    util::getX(p1) = util::getX(box.minCorner);
    util::getY(p1) = util::getY(box.minCorner);
    util::getX(p2) = util::getX(box.maxCorner);
    util::getY(p2) = util::getY(box.minCorner);
    util::getX(p3) = util::getX(box.maxCorner);
    util::getY(p3) = util::getY(box.maxCorner);
    util::getX(p4) = util::getX(box.minCorner);
    util::getY(p4) = util::getY(box.maxCorner);
    return iterateEdgesPointsHelper({p1, p2, p3, p4, p1}, reverse, func);
}

template <typename Point>
bool iterateEdgesLineString(
    const LineString<Point>& linestring,
    const bool reverse,
    const IterateEdgeFunc<Point>& func)
{
    static_assert(isPoint<Point>());

    return iterateEdgesPointsHelper(linestring.points, reverse, func);
}

template <typename Point>
bool iterateEdgesRing(
    const Ring<Point>& ring,
    const bool reverse,
    const IterateEdgeFunc<Point>& func)
{
    static_assert(isPoint<Point>());

    return iterateEdgesPointsHelper(ring.points, reverse, func);
}

template <typename Point>
bool iterateEdgesPolygon(
    const Polygon<Point>& poly,
    const bool reverse,
    const IterateEdgeFunc<Point>& func)
{
    static_assert(isPoint<Point>());

    if (!iterateEdgesPointsHelper(poly.outer.points, reverse, func)) {
        return false;
    }

    for(const auto& inner : poly.inners) {
        if (!iterateEdgesPointsHelper(inner.points, reverse, func)) {
            return false;
        }
    }

    return true;
}

template <typename Point>
bool iterateEdgesMultiPolygon(
    const MultiPolygon<Point>& geometry,
    const bool reverse,
    const IterateEdgeFunc<Point>& func)
{
    static_assert(isPoint<Point>());

    for(const auto& poly : geometry.parts) {
        if (!iterateEdgesPolygon(poly, reverse, func)) {
            return false;
        }
    }

    return true;
}

template <typename Point>
bool iterateEdgesGeometry(
    const Geometry<Point>& geom,
    const bool reverse,
    const IterateEdgeFunc<Point>& func)
{
    static_assert(isPoint<Point>());

    bool ok = true;

    bool result = std::visit(overloaded{
        [reverse, &func](const Segment<Point>& geometry) {
            return iterateEdgesSegment(geometry, reverse, func);
        },
        [reverse, &func](const Box<Point>& geometry) {
            return iterateEdgesBox(geometry, reverse, func);
        },
        [reverse, &func](const LineString<Point>& geometry) {
            return iterateEdgesLineString(geometry, reverse, func);
        },
        [reverse, &func](const Ring<Point>& geometry) {
            return iterateEdgesRing(geometry, reverse, func);
        },
        [reverse, &func](const Polygon<Point>& geometry) {
            return iterateEdgesPolygon(geometry, reverse, func);
        },
        [reverse, &func](const MultiPolygon<Point>& geometry) {
            return iterateEdgesMultiPolygon(geometry, reverse, func);
        },
        [&ok](const auto&) {
            ok = false;
            return false;
        }
    }, geom.geometry);

    if (!ok) {
        throw Exception("Error in iterateEdgesGeometry: Invalid geometry type. Check input geometries.");
    }

    return result;
}

template <typename Geometry>
bool iterateEdges(
    const Geometry& geometry,
    const bool reverse,
    const IterateEdgeFunc<typename Geometry::point_type>& func)
{
    static_assert(
        isSegment<Geometry>() ||
        isBox<Geometry>() ||
        isLineString<Geometry>() ||
        isRing<Geometry>() ||
        isPolygon<Geometry>() ||
        isMultiPolygon<Geometry>() ||
        isGeometry<Geometry>());

    if constexpr (isSegment<Geometry>()) {
        return iterateEdgesSegment(geometry, reverse, func);
    } else if constexpr (isBox<Geometry>()) {
        return iterateEdgesBox(geometry, reverse, func);
    } else if constexpr (isLineString<Geometry>()) {
        return iterateEdgesLineString(geometry, reverse, func);
    } else if constexpr (isRing<Geometry>()) {
        return iterateEdgesRing(geometry, reverse, func);
    } else if constexpr (isPolygon<Geometry>()) {
        return iterateEdgesPolygon(geometry, reverse, func);
    } else if constexpr (isMultiPolygon<Geometry>()) {
        return iterateEdgesMultiPolygon(geometry, reverse, func);
    } else if constexpr (isGeometry<Geometry>()) {
        return iterateEdgesGeometry(geometry, reverse, func);
    }
}

} // namespace GEOLIB_NAMESPACE::detail
