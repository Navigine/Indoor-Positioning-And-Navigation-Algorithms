#pragma once

#include "type_checks.h"
#include "util.h"

namespace GEOLIB_NAMESPACE::detail {

template <typename Point>
bool equalPoints(const Point& point1, const Point& point2, double epsilon = 0.0)
{
    static_assert(isPoint<Point>());

    const auto p1x = util::getX(point1);
    const auto p2x = util::getX(point2);
    const auto p1y = util::getY(point1);
    const auto p2y = util::getY(point2);

    return epsilon == 0.0 ?
        (p1x == p2x && p1y == p2y) :
        (std::fabs(p1x - p2x) < epsilon && std::fabs(p1y - p2y) < epsilon);
}

template <typename Point>
int comparePoints(const Point& point1, const Point& point2)
{
    static_assert(isPoint<Point>());

    if (util::getX(point1) < util::getX(point2)) {
        return -1;
    } else if (util::getX(point1) > util::getX(point2)) {
        return 1;
    }

    if (util::getY(point1) < util::getY(point2)) {
        return -1;
    } else if (util::getY(point1) > util::getY(point2)) {
        return 1;
    }

    return 0;
}

template <typename Point>
bool equalSegments(const Segment<Point>& segment1, const Segment<Point>& segment2, double epsilon = 0.0)
{
    static_assert(isPoint<Point>());
    return equalPoints(segment1.first, segment2.first, epsilon) &&
           equalPoints(segment1.second, segment2.second, epsilon);
}

template <typename Point>
int compareSegments(const Segment<Point>& segment1, const Segment<Point>& segment2)
{
    static_assert(isPoint<Point>());

    if (int r = comparePoints(segment1.first, segment2.first)) {
        return r;
    }

    return comparePoints(segment1.second, segment2.second);
}

template <typename Point>
bool equalBoxes(const Box<Point>& box1, const Box<Point>& box2, double epsilon = 0.0)
{
    static_assert(isPoint<Point>());
    return equalPoints(box1.minCorner, box2.minCorner, epsilon) &&
           equalPoints(box1.maxCorner, box2.maxCorner, epsilon);
}

template <typename Point>
int compareBoxes(const Box<Point>& box1, const Box<Point>& box2)
{
    static_assert(isPoint<Point>());

    if (int r = comparePoints(box1.minCorner, box2.minCorner)) {
        return r;
    }

    return comparePoints(box1.maxCorner, box2.maxCorner);
}

template <typename Point>
bool equalLineStrings(const LineString<Point>& linestring1, const LineString<Point>& linestring2, double epsilon = 0.0)
{
    static_assert(isPoint<Point>());

    if (linestring1.points.size() != linestring2.points.size()) {
        return false;
    }

    for(size_t i = 0; i < linestring1.points.size(); ++i) {
        if (!equalPoints(linestring1.points[i], linestring2.points[i], epsilon)) {
            return false;
        }
    }

    return true;
}

template <typename Point>
int compareLineStrings(const LineString<Point>& linestring1, const LineString<Point>& linestring2)
{
    static_assert(isPoint<Point>());

    for(size_t i = 0; i < linestring1.points.size() && i < linestring2.points.size(); ++i) {
        if (int r = comparePoints(linestring1.points[i], linestring2.points[i])) {
            return r;
        }
    }

    if (linestring1.points.size() < linestring2.points.size()) {
        return -1;
    } else if (linestring1.points.size() > linestring2.points.size()) {
        return 1;
    }

    return 0;
}

template <typename Point>
bool equalRings(const Ring<Point>& ring1, const Ring<Point>& ring2, double epsilon = 0.0)
{
    static_assert(isPoint<Point>());

    if (ring1.points.size() != ring2.points.size()) {
        return false;
    }

    for(size_t i = 0; i < ring1.points.size(); ++i) {
        if (!equalPoints(ring1.points[i], ring2.points[i], epsilon)) {
            return false;
        }
    }

    return true;
}

template <typename Point>
int compareRings(const Ring<Point>& ring1, const Ring<Point>& ring2)
{
    static_assert(isPoint<Point>());

    for(size_t i = 0; i < ring1.points.size() && i < ring2.points.size(); ++i) {
        if (int r = comparePoints(ring1.points[i], ring2.points[i])) {
            return r;
        }
    }

    if (ring1.points.size() < ring2.points.size()) {
        return -1;
    } else if (ring1.points.size() > ring2.points.size()) {
        return 1;
    }

    return 0;
}

template <typename Point>
bool equalPolygons(const Polygon<Point>& poly1, const Polygon<Point>& poly2, double epsilon = 0.0)
{
    static_assert(isPoint<Point>());

    if (!equalRings(poly1.outer, poly2.outer, epsilon)) {
        return false;
    }

    if (poly1.inners.size() != poly2.inners.size()) {
        return false;
    }

    for(size_t i = 0; i < poly1.inners.size(); ++i) {
        if (!equalRings(poly1.inners[i], poly2.inners[i], epsilon)) {
            return false;
        }
    }

    return true;
}

template <typename Point>
int comparePolygons(const Polygon<Point>& poly1, const Polygon<Point>& poly2)
{
    static_assert(isPoint<Point>());

    if (int r = compareRings(poly1.outer, poly2.outer)) {
        return r;
    }

    for(size_t i = 0; i < poly1.inners.size() && i < poly2.inners.size(); ++i) {
        if (int r = compareRings(poly1.inners[i], poly2.inners[i])) {
            return r;
        }
    }

    if (poly1.inners.size() < poly2.inners.size()) {
        return -1;
    } else if (poly1.inners.size() > poly2.inners.size()) {
        return 1;
    }

    return 0;
}

template <typename Point>
bool equalMultiPolygons(const MultiPolygon<Point>& geom1, const MultiPolygon<Point>& geom2, double epsilon = 0.0)
{
    static_assert(isPoint<Point>());

    if (geom1.parts.size() != geom2.parts.size()) {
        return false;
    }

    for(size_t i = 0; i < geom1.parts.size(); ++i) {
        if (!equalPolygons(geom1.parts[i], geom2.parts[i], epsilon)) {
            return false;
        }
    }

    return true;
}

template <typename Point>
int compareMultiPolygons(const MultiPolygon<Point>& geom1, const MultiPolygon<Point>& geom2)
{
    static_assert(isPoint<Point>());

    for(size_t i = 0; i < geom1.parts.size() && i < geom2.parts.size(); ++i) {
        if (int r = comparePolygons(geom1.parts[i], geom2.parts[i])) {
            return r;
        }
    }

    if (geom1.parts.size() < geom2.parts.size()) {
        return -1;
    } else if (geom1.parts.size() > geom2.parts.size()) {
        return 1;
    }

    return 0;
}

template <typename Point>
bool equalGeometries(const Geometry<Point>& geom1, const Geometry<Point>& geom2, double epsilon = 0.0)
{
    static_assert(isPoint<Point>());

    bool ok = true;
    bool result = std::visit(overloaded{
        [&](const Point& arg1, const Point& arg2) {
            return equalPoints(arg1, arg2, epsilon);
        },
        [&](const Segment<Point>& arg1, const Segment<Point>& arg2) {
            return equalSegments(arg1, arg2, epsilon);
        },
        [&](const Box<Point>& arg1, const Box<Point>& arg2) {
            return equalBoxes(arg1, arg2, epsilon);
        },
        [&](const LineString<Point>& arg1, const LineString<Point>& arg2) {
            return equalLineStrings(arg1, arg2, epsilon);
        },
        [&](const Ring<Point>& arg1, const Ring<Point>& arg2) {
            return equalRings(arg1, arg2, epsilon);
        },
        [&](const Polygon<Point>& arg1, const Polygon<Point>& arg2) {
            return equalPolygons(arg1, arg2, epsilon);
        },
        [&](const MultiPolygon<Point>& arg1, const MultiPolygon<Point>& arg2) {
            return equalMultiPolygons(arg1, arg2, epsilon);
        },
        [&](const auto&, const auto&) {
            ok = false;
            return false;
        }},
        geom1.geometry, geom2.geometry);

    if (!ok) {
        throw Exception("Error in equalGeometries: trying to compare different geometries. Check input geometries.");
    }

    return result;
}

template <typename Point>
int compareGeometries(const Geometry<Point>& geom1, const Geometry<Point>& geom2)
{
    static_assert(isPoint<Point>());

    bool ok = true;
    int result = std::visit(overloaded{
        [](const Point& arg1, const Point& arg2) {
            return comparePoints(arg1, arg2);
        },
        [](const Segment<Point>& arg1, const Segment<Point>& arg2) {
            return compareSegments(arg1, arg2);
        },
        [](const Box<Point>& arg1, const Box<Point>& arg2) {
            return compareBoxes(arg1, arg2);
        },
        [](const LineString<Point>& arg1, const LineString<Point>& arg2) {
            return compareLineStrings(arg1, arg2);
        },
        [](const Ring<Point>& arg1, const Ring<Point>& arg2) {
            return compareRings(arg1, arg2);
        },
        [](const Polygon<Point>& arg1, const Polygon<Point>& arg2) {
            return comparePolygons(arg1, arg2);
        },
        [](const MultiPolygon<Point>& arg1, const MultiPolygon<Point>& arg2) {
            return compareMultiPolygons(arg1, arg2);
        },
        [&ok](const auto&, const auto&) {
            ok = false;
            return -1;
        }},
        geom1.geometry, geom2.geometry);

    if (!ok) {
        throw Exception("Error in compareGeometries: trying to compare different geometries. Check input geometries.");
    }

    return result;
}

template <typename Geometry>
bool equal(const Geometry& geometry1, const Geometry& geometry2, double epsilon = 0.0)
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
        return equalPoints(geometry1, geometry2, epsilon);
    } else if constexpr (isSegment<Geometry>()) {
        return equalSegments(geometry1, geometry2, epsilon);
    } else if constexpr (isBox<Geometry>()) {
        return equalBoxes(geometry1, geometry2, epsilon);
    } else if constexpr (isLineString<Geometry>()) {
        return equalLineStrings(geometry1, geometry2, epsilon);
    } else if constexpr (isRing<Geometry>()) {
        return equalRings(geometry1, geometry2, epsilon);
    } else if constexpr (isPolygon<Geometry>()) {
        return equalPolygons(geometry1, geometry2, epsilon);
    } else if constexpr (isMultiPolygon<Geometry>()) {
        return equalMultiPolygons(geometry1, geometry2, epsilon);
    } else if constexpr (isGeometry<Geometry>()) {
        return equalGeometries(geometry1, geometry2, epsilon);
    }
    return false;
}

template <typename Geometry>
int compare(const Geometry& geometry1, const Geometry& geometry2)
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
        return comparePoints(geometry1, geometry2);
    } else if constexpr (isSegment<Geometry>()) {
        return compareSegments(geometry1, geometry2);
    } else if constexpr (isBox<Geometry>()) {
        return compareBoxes(geometry1, geometry2);
    } else if constexpr (isLineString<Geometry>()) {
        return compareLineStrings(geometry1, geometry2);
    } else if constexpr (isRing<Geometry>()) {
        return compareRings(geometry1, geometry2);
    } else if constexpr (isPolygon<Geometry>()) {
        return comparePolygons(geometry1, geometry2);
    } else if constexpr (isMultiPolygon<Geometry>()) {
        return compareMultiPolygons(geometry1, geometry2);
    } else if constexpr (isGeometry<Geometry>()) {
        return compareGeometries(geometry1, geometry2);
    }
    throw Exception("Error in compare: Unknown geometry type.");
}

template <typename Geometry>
bool less(const Geometry& geometry1, const Geometry& geometry2)
{
    return detail::compare(geometry1, geometry2) < 0;
}

template <typename Geometry>
bool greater(const Geometry& geometry1, const Geometry& geometry2)
{
    return detail::compare(geometry1, geometry2) > 0;
}

} // namespace GEOLIB_NAMESPACE::detail
