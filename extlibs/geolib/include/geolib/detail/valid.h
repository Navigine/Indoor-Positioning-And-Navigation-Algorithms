#pragma once

#include "compare.h"
#include "correct.h"
#include "iterate_edges.h"
#include "iterate_vertices.h"
#include "type_checks.h"
#include "util.h"

namespace GEOLIB_NAMESPACE::detail {

template <typename Point>
bool isValidPoint(const Point& point, std::string& message)
{
    if (!std::isfinite(util::getX(point)) || !std::isfinite(util::getY(point))) {
        message += GEOLIB_FORMAT("E100: Point has invalid coordinates: (" << util::getX(point) << ", " << util::getY(point) << ")");
        return false;
    }
    if (std::fabs(util::getX(point)) > util::maxX<Point>() || std::fabs(util::getY(point)) > util::maxY<Point>()) {
        message += GEOLIB_FORMAT("E101: Point coordinates exceed the predefined bounds: (" << util::getX(point) << ", " << util::getY(point) << ")");
        return false;
    }
    return true;
}

template <typename Point>
bool isValidSegment(const Segment<Point>& segment, std::string& message)
{
    static_assert(isPoint<Point>());

    if (!isValidPoint(segment.first, message)) {
        message += GEOLIB_FORMAT("E110: Segment has invalid starting point: (" << util::getX(segment.first) << ", " << util::getY(segment.first) << ")");
        return false;
    }

    if (!isValidPoint(segment.second, message)) {
        message += GEOLIB_FORMAT("E111: Segment has invalid ending point: (" << util::getX(segment.second) << ", " << util::getY(segment.second) << ")");
        return false;
    }

    return true;
}

template <typename Point>
bool isValidBox(const Box<Point>& box, std::string& message)
{
    static_assert(isPoint<Point>());

    if (!isValidPoint(box.minCorner, message)) {
        message += GEOLIB_FORMAT("E120: Box has invalid min corner: (" << util::getX(box.minCorner) << ", " << util::getY(box.minCorner) << ")");
        return false;
    }

    if (!isValidPoint(box.maxCorner, message)) {
        message += GEOLIB_FORMAT("E121: Box has invalid max corner: (" << util::getX(box.maxCorner) << ", " << util::getY(box.maxCorner) << ")");
        return false;
    }

    if (util::getX(box.minCorner) > util::getX(box.maxCorner) || util::getY(box.minCorner) > util::getY(box.maxCorner)) {
        message += GEOLIB_FORMAT("E122: Box min corner should be less or equal to max corner: "
            "min(" << util::getX(box.minCorner) << ", " << util::getY(box.minCorner) << "), "
            "max(" << util::getX(box.maxCorner) << ", " << util::getY(box.maxCorner) << ")");
        return false;
    }

    return true;
}

template <typename Point>
bool isValidLineString(const LineString<Point>& linestring, std::string& message)
{
    static_assert(isPoint<Point>());

    for(size_t i = 0; i < linestring.points.size(); ++i) {
        const auto& point = linestring.points[i];
        if (!isValidPoint(point, message)) {
            message += GEOLIB_FORMAT("E130: LineString has invalid point: (" << util::getX(point) << ", " << util::getY(point) << "), index=" << i);
            return false;
        }
    }
    return true;
}

template <typename Point>
bool isValidRing(const Ring<Point>& ring, bool isOuter, std::string& message)
{
    static_assert(isPoint<Point>());

    for(size_t i = 0; i < ring.points.size(); ++i) {
        const auto& point = ring.points[i];
        if (!isValidPoint(point, message)) {
            message += GEOLIB_FORMAT("E140: Ring has invalid point: (" << util::getX(point) << ", " << util::getY(point) << "), index=" << i);
            return false;
        }
    }

    if (ring.points.empty()) {
        message += GEOLIB_FORMAT("E141: Ring is empty");
        return false;
    }

    if (!detail::equal(ring.points.back(), ring.points.front())) {
        message += GEOLIB_FORMAT("E142: Ring is not closed, use correct");
        return false;
    }

    if (ring.points.size() < 4) {
        message += GEOLIB_FORMAT("E143: Ring has not enough points: " << ring.points.size() << ", expected >= 4");
        return false;
    }

    if (detail::checkClockwiseRing(ring) == isOuter) {
        if (isOuter) {
            message += GEOLIB_FORMAT("E144: Ring has invalid orientation (expected counter-clockwise), use correct");
        } else {
            message += GEOLIB_FORMAT("E145: Ring has invalid orientation (expected clockwise), use correct");
        }
        return false;
    }

    const auto N = ring.points.size();

    for(size_t i = 0; i <= N - 4; ++i) {
        const auto p1 = ring.points[i];
        const auto p2 = ring.points[i+1];
        const auto maxJ = i ? N - 2 : N - 3;
        for(size_t j = i + 2; j <= maxJ; ++j) {
            const auto q1 = ring.points[j];
            const auto q2 = ring.points[j+1];

            double u1, u2, v1, v2;
            Point i1, i2;

            if (segmentIntersection(p1, p2, q1, q2, u1, v1, i1, u2, v2, i2) > 0) {
                message += GEOLIB_FORMAT("E146: Ring has self-intersection at point: (" << util::getX(i1) << ", " << util::getY(i1) << ")");
                return false;
            }
        }
    }

    return true;
}

template <typename Point>
bool isInnerRing(const Ring<Point>& outer, const Ring<Point>& inner, std::string& message)
{
    static_assert(isPoint<Point>());

    // Check that:
    //  1) inner ring is inside the outer ring
    //  2) number of common border points is finite

    bool ok = true;

    ok = iterateVertices(outer, false,
        [&](const Point& point, size_t, size_t) {
            if (!containsPoint(inner, point, ContainsPolicy::OUTER_BORDER_FINITE)) {
                message += GEOLIB_FORMAT("E150: Outer ring point is inside the inner ring: (" << util::getX(point) << ", " << util::getY(point) << ")");
                return false;
            }
            return true;
        });

    if (!ok) {
        return false;
    }

    ok = iterateVertices(inner, false,
        [&](const Point& point, size_t, size_t) {
            if (!containsPoint(outer, point, ContainsPolicy::INNER_BORDER_FINITE)) {
                message += GEOLIB_FORMAT("E151: Inner ring point is outside the outer ring: (" << util::getX(point) << ", " << util::getY(point) << ")");
                return false;
            }
            return true;
        });

    if (!ok) {
        return false;
    }

    ok = iterateEdges(outer, false,
        [&](const Point& p1, const Point& p2) {
            Point ip;
            if (!containsSegment(inner, p1, p2, ContainsPolicy::OUTER_BORDER_FINITE, &ip)) {
                message += GEOLIB_FORMAT("E152: Rings do intersect at point: (" << util::getX(ip) << ", " << util::getY(ip) << ")");
                return false;
            }
            return true;
        });

    if (!ok) {
        return false;
    }

    ok = iterateEdges(inner, false,
        [&](const Point& p1, const Point& p2) {
            Point ip;
            if (!containsSegment(outer, p1, p2, ContainsPolicy::INNER_BORDER_FINITE, &ip)) {
                message += GEOLIB_FORMAT("E153: Rings do intersect at point: (" << util::getX(ip) << ", " << util::getY(ip) << ")");
                return false;
            }
            return true;
        });

    if (!ok) {
        return false;
    }

    return true;
}

template <typename Geometry1, typename Geometry2>
bool checkDisjointGeometries(
    const Geometry1& geom1,
    const Geometry2& geom2,
    std::string& message)
{
    static_assert(isBox<Geometry1>() || isRing<Geometry1>() || isPolygon<Geometry1>() || isMultiPolygon<Geometry1>());
    static_assert(isBox<Geometry2>() || isRing<Geometry2>() || isPolygon<Geometry2>() || isMultiPolygon<Geometry2>());
    static_assert(std::is_same<typename Geometry1::point_type, typename Geometry2::point_type>::value == true);

    // Check that two given geometries are disjoint:
    //  1) don't have common inner points
    //  2) number of common border points is finite

    using Point = typename Geometry1::point_type;

    const auto policy = ContainsPolicy::OUTER_BORDER_FINITE;

    bool ok = true;

    ok = iterateVertices(geom1, false,
        [&](const Point& point, size_t, size_t) {
            if (!containsPoint(geom2, point, policy)) {
                message += GEOLIB_FORMAT("E160: Geometries do intersect at point: (" << util::getX(point) << ", " << util::getY(point) << ")");
                return false;
            }
            return true;
        });

    if (!ok) {
        return false;
    }

    ok = iterateVertices(geom2, false,
        [&](const Point& point, size_t, size_t) {
            if (!containsPoint(geom1, point, policy)) {
                message += GEOLIB_FORMAT("E161: Geometries do intersect at point: (" << util::getX(point) << ", " << util::getY(point) << ")");
                return false;
            }
            return true;
        });

    if (!ok) {
        return false;
    }

    ok = iterateEdges(geom1, false,
        [&](const Point& p1, const Point& p2) {
            Point ip;
            if (!containsSegment(geom2, p1, p2, policy, &ip)) {
                message += GEOLIB_FORMAT("E162: Geometries do intersect at point: (" << util::getX(ip) << ", " << util::getY(ip) << ")");
                return false;
            }
            return true;
        });

    if (!ok) {
        return false;
    }

    ok = iterateEdges(geom2, false,
        [&](const Point& p1, const Point& p2) {
            Point ip;
            if (!containsSegment(geom1, p1, p2, policy, &ip)) {
                message += GEOLIB_FORMAT("E163: Geometries do intersect at point: (" << util::getX(ip) << ", " << util::getY(ip) << ")");
                return false;
            }
            return true;
        });

    if (!ok) {
        return false;
    }

    return true;
}

template <typename Point>
bool isValidPolygon(const Polygon<Point>& polygon, std::string& message)
{
    static_assert(isPoint<Point>());

    if (!isValidRing(polygon.outer, true, message)) {
        message += GEOLIB_FORMAT("E170: Polygon has invalid outer ring");
        return false;
    }

    for(size_t i = 0; i < polygon.inners.size(); ++i) {
        if (!isValidRing(polygon.inners[i], false, message)) {
            message += GEOLIB_FORMAT("E171: Polygon has invalid inner ring, index=" << i);
            return false;
        }
        if (!isInnerRing(polygon.outer, polygon.inners[i], message)) {
            message += GEOLIB_FORMAT("E172: Polygon has invalid inner ring, index=" << i);
            return false;
        }
    }

    // Check that inner rings do not intersect
    for(size_t i = 0; i < polygon.inners.size(); ++i) {
        for(size_t j = i + 1; j < polygon.inners.size(); ++j) {
            if (!checkDisjointGeometries(polygon.inners[i], polygon.inners[j], message)) {
                message += GEOLIB_FORMAT("E173: Polygon has intersecting inner rings: index=" << i << ", index=" << j);
                return false;
            }
        }
    }

    return true;
}

template <typename Point>
bool isValidMultiPolygon(const MultiPolygon<Point>& geometry, std::string& message)
{
    static_assert(isPoint<Point>());

    for(size_t i = 0; i < geometry.parts.size(); ++i) {
        if (!isValidPolygon(geometry.parts[i], message)) {
            message += GEOLIB_FORMAT("E180: MultiPolygon has invalid part, index=" << i);
            return false;
        }
    }

    for(size_t i = 0; i < geometry.parts.size(); ++i) {
        for(size_t j = i + 1; j < geometry.parts.size(); ++j) {
            if (!checkDisjointGeometries(geometry.parts[i], geometry.parts[j], message)) {
                message += GEOLIB_FORMAT("E181: Intersecting MultiPolygon parts: index=" << i << ", index=" << j);
                return false;
            }
        }
    }

    return true;
}

template <typename Point>
bool isValidGeometry(const Geometry<Point>& geometry, std::string& message)
{
    static_assert(isPoint<Point>());

    bool ok = true;
    bool result = std::visit(overloaded{
        [&](const Point& geometry) {
            return isValidPoint(geometry, message);
        },
        [&](const Segment<Point>& geometry) {
            return isValidSegment(geometry, message);
        },
        [&](const LineString<Point>& geometry) {
            return isValidLineString(geometry, message);
        },
        [&](const Box<Point>& geometry) {
            return isValidBox(geometry, message);
        },
        [&](const Ring<Point>& geometry) {
            return isValidRing(geometry, true, message);
        },
        [&](const Polygon<Point>& geometry) {
            return isValidPolygon(geometry, message);
        },
        [&](const MultiPolygon<Point>& geometry) {
            return isValidMultiPolygon(geometry, message);
        },
        [&](const auto&) {
            ok = false;
            return false;
        }
    }, geometry.geometry);

    if (!ok) {
        throw Exception("Error in geometryBoundingBox: Invalid geometry type. Check input geometries.");
    }

    return result;
}

template <typename Geometry>
bool isValid(const Geometry& geometry, std::string& message)
{
    static_assert(
        isPoint<Geometry>() ||
        isSegment<Geometry>() ||
        isBox<Geometry>() ||
        isLineString<Geometry>() ||
        isRing<Geometry>() ||
        isPolygon<Geometry>() ||
        isMultiPolygon<Geometry>() ||
        isGeometry<Geometry>());

    if constexpr (isPoint<Geometry>()) {
        return isValidPoint(geometry, message);
    } else if constexpr (isSegment<Geometry>()) {
        return isValidSegment(geometry, message);
    } else if constexpr (isLineString<Geometry>()) {
        return isValidLineSting(geometry, message);
    } else if constexpr (isBox<Geometry>()) {
        return isValidBox(geometry, message);
    } else if constexpr (isRing<Geometry>()) {
        return isValidRing(geometry, true, message);
    } else if constexpr (isPolygon<Geometry>()) {
        return isValidPolygon(geometry, message);
    } else if constexpr (isMultiPolygon<Geometry>()) {
        return isValidMultiPolygon(geometry, message);
    } else if constexpr (isGeometry<Geometry>()) {
        return isValidGeometry(geometry, message);
    }
}

} // namespace GEOLIB_NAMESPACE::detail
