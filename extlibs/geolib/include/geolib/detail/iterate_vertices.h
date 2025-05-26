#pragma once

#include "type_checks.h"
#include "util.h"

namespace GEOLIB_NAMESPACE::detail {

template <typename Point>
using IterateVertexFuncConst = std::function<bool(
    const Point& point,
    size_t index,
    size_t ringSize)>;

template <typename Point>
using IterateVertexFuncMutable = std::function<bool(
    Point& point,
    size_t index,
    size_t ringSize)>;

template <typename Point, typename Functor>
bool iterateVerticesPointsHelper(
    std::vector<Point>& points,
    const bool reverse,
    const Functor& func,
    size_t& shift)
{
    static_assert(isPoint<Point>());

    if (points.size() < 2) {
        return true;
    }

    const auto n = points.size();

    for(size_t i = 0; i < n; ++i) {
        const auto index = reverse ? n - i - 1 : i;
        if (!func(points[index], shift + i, i + 1 < n ? 0 : n)) {
            return false;
        }
    }
    shift += n;
    return true;
}

template <typename Box, typename Functor>
bool iterateVerticesBox(
    const Box& box,
    const bool reverse,
    const Functor& func)
{
    static_assert(isBox<Box>());

    using Point = typename Box::point_type;

    size_t shift = 0;

    std::vector<Point> points(5);
    util::getX(points[0]) = util::getX(box.minCorner);
    util::getY(points[0]) = util::getY(box.minCorner);
    util::getX(points[1]) = util::getX(box.maxCorner);
    util::getY(points[1]) = util::getY(box.minCorner);
    util::getX(points[2]) = util::getX(box.maxCorner);
    util::getY(points[2]) = util::getY(box.maxCorner);
    util::getX(points[3]) = util::getX(box.minCorner);
    util::getY(points[3]) = util::getY(box.maxCorner);
    util::getX(points[4]) = util::getX(box.minCorner);
    util::getY(points[4]) = util::getY(box.minCorner);

    if (!iterateVerticesPointsHelper(points, reverse, func, shift)) {
        return false;
    }

    return true;
}

template <typename Ring, typename Functor>
bool iterateVerticesRing(
    Ring& ring,
    const bool reverse,
    const Functor& func)
{
    static_assert(isRing<Ring>());

    size_t shift = 0;

    if (!iterateVerticesPointsHelper(ring.points, reverse, func, shift)) {
        return false;
    }

    return true;
}

template <typename Polygon, typename Functor>
bool iterateVerticesPolygon(
    Polygon& poly,
    const bool reverse,
    const Functor& func)
{
    static_assert(isPolygon<Polygon>());

    size_t shift = 0;

    if (!iterateVerticesPointsHelper(poly.outer.points, reverse, func, shift)) {
        return false;
    }

    for(auto& inner : poly.inners) {
        if (!iterateVerticesPointsHelper(inner.points, reverse, func, shift)) {
            return false;
        }
    }

    return true;
}

template <typename MultiPolygon, typename Functor>
bool iterateVerticesMultiPolygon(
    MultiPolygon& geometry,
    const bool reverse,
    const Functor& func)
{
    static_assert(isMultiPolygon<MultiPolygon>());

    for(auto& poly : geometry.parts) {
        if (!iterateVerticesPolygon(poly, reverse, func)) {
            return false;
        }
    }

    return true;
}

template <typename Geometry, typename Functor>
bool iterateVerticesGeometry(
    const Geometry& geom,
    const bool reverse,
    const Functor& func)
{
    static_assert(isGeometry<Geometry>());

    using Point = typename Geometry::point_type;

    bool ok = true;

    bool result = std::visit(overloaded{
        [&](const Box<Point>& geometry) {
            return iterateVerticesBox(geometry, reverse, func);
        },
        [&](const Ring<Point>& geometry) {
            return iterateVerticesRing(const_cast<Ring<Point>&>(geometry), reverse, func);
        },
        [&](const Polygon<Point>& geometry) {
            return iterateVerticesPolygon(const_cast<Polygon<Point>&>(geometry), reverse, func);
        },
        [&](const MultiPolygon<Point>& geometry) {
            return iterateVerticesMultiPolygon(const_cast<MultiPolygon<Point>&>(geometry), reverse, func);
        },
        [&](const auto&) {
            ok = false;
            return false;
        }
    }, geom.geometry);

    if (!ok) {
        throw Exception("Error in iterateVerticesGeometry: Invalid geometry type. Check input geometries.");
    }

    return result;
}

template <typename Geometry, typename Functor>
bool iterateVerticesGeometry(
    Geometry& geom,
    const bool reverse,
    const Functor& func)
{
    static_assert(isGeometry<Geometry>());

    using Point = typename Geometry::point_type;
    bool ok = true;

    bool result = std::visit(overloaded{
        [&](Ring<Point>& geometry) {
            return iterateVerticesRing(geometry, reverse, func);
        },
        [&](Polygon<Point>& geometry) {
            return iterateVerticesPolygon(geometry, reverse, func);
        },
        [&](MultiPolygon<Point>& geometry) {
            return iterateVerticesMultiPolygon(geometry, reverse, func);
        },
        [&](auto&) {
            ok = false;
            return false;
        }
    }, geom.geometry);

    if (!ok) {
        throw Exception("Error in iterateVerticesGeometry: Invalid geometry type. Check input geometries.");
    }

    return result;
}

template <typename Geometry>
bool iterateVertices(
    const Geometry& geometry,
    const bool reverse,
    const IterateVertexFuncConst<typename Geometry::point_type>& func)
{
    static_assert(
        isBox<Geometry>() ||
        isRing<Geometry>() ||
        isPolygon<Geometry>() ||
        isMultiPolygon<Geometry>() ||
        isGeometry<Geometry>());

    if constexpr (isBox<Geometry>()) {
        return iterateVerticesBox(geometry, reverse, func);
    } else if constexpr (isRing<Geometry>()) {
        return iterateVerticesRing(const_cast<Geometry&>(geometry), reverse, func);
    } else if constexpr (isPolygon<Geometry>()) {
        return iterateVerticesPolygon(const_cast<Geometry&>(geometry), reverse, func);
    } else if constexpr (isMultiPolygon<Geometry>()) {
        return iterateVerticesMultiPolygon(const_cast<Geometry&>(geometry), reverse, func);
    } else if constexpr (isGeometry<Geometry>()) {
        return iterateVerticesGeometry(geometry, reverse, func);
    }
}

template <typename Geometry>
bool iterateVerticesMutable(
    Geometry& geometry,
    const bool reverse,
    const IterateVertexFuncMutable<typename Geometry::point_type>& func)
{
    static_assert(
        isRing<Geometry>() ||
        isPolygon<Geometry>() ||
        isMultiPolygon<Geometry>() ||
        isGeometry<Geometry>());

    if constexpr (isRing<Geometry>()) {
        return iterateVerticesRing(geometry, reverse, func);
    } else if constexpr (isPolygon<Geometry>()) {
        return iterateVerticesPolygon(geometry, reverse, func);
    } else if constexpr (isMultiPolygon<Geometry>()) {
        return iterateVerticesMultiPolygon(geometry, reverse, func);
    } else if constexpr (isGeometry<Geometry>()) {
        return iterateVerticesGeometry(geometry, reverse, func);
    }
}

template <typename Geometry>
void transform(Geometry& geometry, const std::function<void(typename Geometry::point_type& point)>& func)
{
    static_assert(
        isRing<Geometry>() ||
        isPolygon<Geometry>() ||
        isMultiPolygon<Geometry>() ||
        isGeometry<Geometry>());

    using Point = typename Geometry::point_type;

    iterateVerticesMutable(geometry, false,
        [&func](Point& point, size_t, size_t) {
            func(point);
            return true;
        });

    detail::correct(geometry);
}

} // namespace GEOLIB_NAMESPACE::detail
