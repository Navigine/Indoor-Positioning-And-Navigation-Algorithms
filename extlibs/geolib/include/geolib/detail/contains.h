#pragma once

#include "intersection.h"
#include "iterate_edges.h"
#include "type_checks.h"
#include "util.h"

namespace GEOLIB_NAMESPACE::detail {

template <typename Point>
int updatePointIndex(
    const Point& point,
    const Point& A,
    const Point& B,
    int index)
{
    static_assert(isPoint<Point>());

    if (!index) {
        return 0;
    }

    auto px = util::getX(point);
    auto py = util::getY(point);
    auto ax = util::getX(A);
    auto ay = util::getY(A);
    auto bx = util::getX(B);
    auto by = util::getY(B);

    if (px == ax && py == ay) {
        return 0;
    }

    if (px == bx && py == by) {
        return 0;
    }

    if (ay > by) {
        std::swap(ax, bx);
        std::swap(ay, by);
    }

    if (ay == by && ay == py) {
        if ((ax <= px && px <= bx) || (bx <= px && px <= ax)) {
            return 0;
        }
        return index;
    }

    if (ay >= py || by < py) {
        return index;
    }

    assert(ay < py && py <= by);

    auto lhs = (ax - px) * (by - ay);
    auto rhs = (ay - py) * (bx - ax);

    if (lhs == rhs) {
        return 0;
    }

    return lhs < rhs ? index : -index;
}

template <typename Point>
int pointIndexBox(const Box<Point>& box, const Point& point)
{
    static_assert(isPoint<Point>());

    if (!std::isfinite(util::getX(point)) ||
        !std::isfinite(util::getY(point))) {
        return -1;
    }

    if (util::getX(point) < util::getX(box.minCorner) ||
        util::getX(point) > util::getX(box.maxCorner) ||
        util::getY(point) < util::getY(box.minCorner) ||
        util::getY(point) > util::getY(box.maxCorner)) {
        return -1;
    }

    if (util::getX(point) > util::getX(box.minCorner) ||
        util::getX(point) < util::getX(box.maxCorner) ||
        util::getY(point) > util::getY(box.minCorner) ||
        util::getY(point) < util::getY(box.maxCorner)) {
        return 1;
    }

    return 0;
}

template <typename Point>
int pointIndexRing(const Ring<Point>& ring, const Point& point)
{
    static_assert(isPoint<Point>());

    if (!std::isfinite(util::getX(point)) ||
        !std::isfinite(util::getY(point))) {
        return -1;
    }

    int index = -1;

    for(size_t i = 0; i + 1 < ring.points.size() && index != 0; ++i) {
        index = updatePointIndex(point, ring.points[i], ring.points[i + 1], index);
    }

    return index;
}

template <typename Point>
int pointIndexPolygon(const Polygon<Point>& poly, const Point& point)
{
    static_assert(isPoint<Point>());

    if (!std::isfinite(util::getX(point)) ||
        !std::isfinite(util::getY(point))) {
        return -1;
    }

    int index = pointIndexRing(poly.outer, point);

    if (index <= 0) {
        return index;
    }

    for(const auto& inner : poly.inners) {
        int index = pointIndexRing(inner, point);
        if (index >= 0) {
            return -index;
        }
    }

    return 1;
}

template <typename Point>
int pointIndexMultiPolygon(const MultiPolygon<Point>& geometry, const Point& point)
{
    static_assert(isPoint<Point>());

    if (!std::isfinite(util::getX(point)) ||
        !std::isfinite(util::getY(point))) {
        return -1;
    }

    for(const auto& part : geometry.parts) {
        int index = pointIndexPolygon(part, point);
        if (index >= 0) {
            return index;
        }
    }

    return -1;
}

template <typename Point>
int pointIndexGeometry(const Geometry<Point>& geometry, const Point& point)
{
    static_assert(isPoint<Point>());

    bool ok = true;

    int result = std::visit(overloaded{
        [&point](const Box<Point>& box) {
            return pointIndexBox(box, point);
        },
        [&point](const Ring<Point>& ring) {
            return pointIndexRing(ring, point);
        },
        [&point](const Polygon<Point>& poly) {
            return pointIndexPolygon(poly, point);
        },
        [&point](const MultiPolygon<Point>& mpoly) {
            return pointIndexMultiPolygon(mpoly, point);
        },
        [&ok](const auto&) {
            ok = false;
            return -1;
        }
    }, geometry.geometry);

    if (!ok) {
        throw Exception("Error in pointIndexGeometry: Invalid geometry type. Check input geometries.");
    }

    return result;
}

template <typename Geometry, typename Point>
int pointIndex(const Geometry& geometry, const Point& point)
{
    static_assert(
        isBox<Geometry>() ||
        isRing<Geometry>() ||
        isPolygon<Geometry>() ||
        isMultiPolygon<Geometry>() ||
        isGeometry<Geometry>());

    static_assert(isPoint<Point>());

    static_assert(std::is_same<typename Geometry::point_type, Point>::value == true);

    if constexpr (isBox<Geometry>()) {
        return pointIndexBox(geometry, point);
    } else if constexpr (isRing<Geometry>()) {
        return pointIndexRing(geometry, point);
    } else if constexpr (isPolygon<Geometry>()) {
        return pointIndexPolygon(geometry, point);
    } else if constexpr (isMultiPolygon<Geometry>()) {
        return pointIndexMultiPolygon(geometry, point);
    } else if constexpr (isGeometry<Geometry>()) {
        return pointIndexGeometry(geometry, point);
    }
}

inline bool checkIndexByPolicy(int index, ContainsPolicy policy)
{
    switch (policy) {
        case ContainsPolicy::INNER:
            return index > 0;

        case ContainsPolicy::INNER_BORDER:
        case ContainsPolicy::INNER_BORDER_FINITE:
            return index >= 0;

        case ContainsPolicy::OUTER:
            return index < 0;

        case ContainsPolicy::OUTER_BORDER:
        case ContainsPolicy::OUTER_BORDER_FINITE:
            return index <= 0;
    }

    return false;
}

template <typename Geometry, typename Point>
bool containsPoint(
    const Geometry& geometry,
    const Point& point,
    ContainsPolicy policy)
{
    static_assert(
        isBox<Geometry>() ||
        isRing<Geometry>() ||
        isPolygon<Geometry>() ||
        isMultiPolygon<Geometry>() ||
        isGeometry<Geometry>());

    static_assert(isPoint<Point>());

    static_assert(std::is_same<typename Geometry::point_type, Point>::value == true);

    return checkIndexByPolicy(pointIndex(geometry, point), policy);
}

template <typename Geometry, typename Point>
bool containsSegment(
    const Geometry& geometry,
    const Point& first,
    const Point& second,
    ContainsPolicy policy,
    Point* outPoint = nullptr)
{
    static_assert(
        isBox<Geometry>() ||
        isRing<Geometry>() ||
        isPolygon<Geometry>() ||
        isMultiPolygon<Geometry>() ||
        isGeometry<Geometry>());

    static_assert(isPoint<Point>());

    static_assert(std::is_same<typename Geometry::point_type, Point>::value == true);

    const int index1 = pointIndex(geometry, first);
    const int index2 = pointIndex(geometry, second);

    if (!checkIndexByPolicy(index1, policy)) {
        if (outPoint) {
            *outPoint = first;
        }
        return false;
    }

    if (!checkIndexByPolicy(index2, policy)) {
        if (outPoint) {
            *outPoint = second;
        }
        return false;
    }

    const bool allowInfiniteBorder =
        policy == ContainsPolicy::INNER_BORDER ||
        policy == ContainsPolicy::OUTER_BORDER;

    // TODO: avoid calculation of cross points
    std::map<double, Point> crossPoints;

    bool success = iterateEdges(geometry, false,
        [&](const Point& p1, const Point& p2) {
            double u1 = NAN, u2 = NAN, v1 = NAN, v2 = NAN;
            Point i1, i2;

            int count = segmentIntersection(first, second, p1, p2, u1, v1, i1, u2, v2, i2);
            if (count == 1 && 0.0 < u1 && u1 < 1.0 && 0.0 < v1 && v1 < 1.0) {
                if (outPoint) {
                    *outPoint = i1;
                }
                return false; // Breaking loop
            }
            if (count >= 1 && 0.0 < u1 && u1 < 1.0) {
                crossPoints.emplace(u1, i1);
            }
            if (count >= 2 && 0.0 < u2 && u2 < 1.0) {
                crossPoints.emplace(u2, i2);
            }
            if (count >= 2 && !allowInfiniteBorder) {
                if (outPoint) {
                    *outPoint = i2;
                }
                return false; // Breaking loop
            }
            return true;
        });

    if (!success) {
        // Loop was interrupted
        return false;
    }

    auto src = first;
    crossPoints.emplace(1.0, second);
    for(const auto& [_, point] : crossPoints) {
        const auto checkPoint = (src + point) / 2;
        const auto index = pointIndex(geometry, checkPoint);
        if (!checkIndexByPolicy(index, policy)) {
            if (outPoint) {
                *outPoint = checkPoint;
            }
            return false;
        }
        src = point;
    }

    return true;
}

template <typename Geometry, typename LineString>
bool containsLineString(
    const Geometry& geometry,
    const LineString& linestring,
    ContainsPolicy policy)
{
    static_assert(
        isBox<Geometry>() ||
        isRing<Geometry>() ||
        isPolygon<Geometry>() ||
        isMultiPolygon<Geometry>() ||
        isGeometry<Geometry>());

    static_assert(isLineString<LineString>());

    static_assert(std::is_same<typename Geometry::point_type, typename LineString::point_type>::value == true);

    for(size_t i = 0; i + 1 < linestring.points.size(); ++i) {
        if (!containsSegment(geometry, linestring.points[i], linestring.points[i+1], policy)) {
            return false;
        }
    }
    return true;
}

template <typename Geometry1, typename Geometry2>
bool containsGeometry(const Geometry1& geometry1, const Geometry2& geometry2, ContainsPolicy policy)
{
    static_assert(
        isBox<Geometry1>() ||
        isRing<Geometry1>() ||
        isPolygon<Geometry1>() ||
        isMultiPolygon<Geometry1>() ||
        isGeometry<Geometry1>());

    static_assert(isGeometry<Geometry2>());

    static_assert(std::is_same<typename Geometry1::point_type, typename Geometry2::point_type>::value == true);

    using Point = typename Geometry1::point_type;

    bool ok = true;
    bool result = std::visit(overloaded{
        [&](const Point& point) {
            return detail::containsPoint(geometry1, point, policy);
        },
        [&](const Segment<Point>& segment) {
            return detail::containsSegment(geometry1, segment.first, segment.second, policy);
        },
        [&](const LineString<Point>& linestring) {
            return detail::containsLineString(geometry1, linestring, policy);
        },
        [&](const auto&) {
            ok = false;
            return false;
        }
    }, geometry2.geometry);

    if (!ok) {
        throw Exception("Error in containsGeometry: Invalid geometries.");
    }

    return result;
}

template <typename Geometry1, typename Geometry2>
bool contains(const Geometry1& geometry1, const Geometry2& geometry2, ContainsPolicy policy = ContainsPolicy::INNER_BORDER)
{
    static_assert(
        isBox<Geometry1>() ||
        isRing<Geometry1>() ||
        isPolygon<Geometry1>() ||
        isMultiPolygon<Geometry1>() ||
        isGeometry<Geometry1>());

    static_assert(
        isPoint<Geometry2>() ||
        isSegment<Geometry2>() ||
        isLineString<Geometry2>() ||
        isGeometry<Geometry2>());

    if constexpr (isPoint<Geometry2>()) {
        return containsPoint(geometry1, geometry2, policy);
    } else if constexpr (isSegment<Geometry2>()) {
        return containsSegment(geometry1, geometry2.first, geometry2.second, policy);
    } else if constexpr (isLineString<Geometry2>()) {
        return containsLineString(geometry1, geometry2, policy);
    } else if constexpr (isGeometry<Geometry2>()) {
        return containsGeometry(geometry1, geometry2, policy);
    }
}

} // namespace GEOLIB_NAMESPACE::detail
