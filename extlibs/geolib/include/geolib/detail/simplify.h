#pragma once

#include "binary_operation.h"
#include "type_checks.h"
#include "util.h"
#include "valid.h"

namespace GEOLIB_NAMESPACE::detail {

template <typename Point>
double triangleArea(const Point& a, const Point& b, const Point& c)
{
    const auto ax = util::getX(a);
    const auto ay = util::getY(a);
    const auto bx = util::getX(b);
    const auto by = util::getY(b);
    const auto cx = util::getX(c);
    const auto cy = util::getY(c);
    return std::fabs((ax - cx) * (by - cy) - (bx - cx) * (ay - cy)) / 2;
}

template <typename Point>
Ring<Point> simplifiedRing(const Ring<Point>& ring, double nullAreaThreshold, bool isOuter)
{
    if (ring.points.size() < 4) {
        return Ring<Point>{};
    }

    std::list<Point> points;

    for(size_t i = 0; i + 1 < ring.points.size(); ++i) {
        points.push_back(ring.points[i]);
    }

    for(auto iter = points.begin(); iter != points.end(); ) {
        auto prev = std::prev(iter);
        if (prev == points.end()) {
            prev = std::prev(prev);
        }
        auto next = std::next(iter);
        if (next == points.end()) {
            next = points.begin();
        }

        double area = triangleArea(*prev, *iter, *next);

        if (area > nullAreaThreshold) {
            ++iter;
            continue;
        }

        points.erase(iter);

        if (points.size() < 3) {
            return Ring<Point>{};
        }

        iter = points.begin();
    }

    assert(points.size() >= 3);

    Ring<Point> result;
    for(const auto& point : points) {
        result.points.push_back(point);
    }
    result.points.push_back(points.front());

    correctRing(result, isOuter);

    std::string message;
    if (!isValidRing(result, isOuter, message)) {
        return ring;
    }

    return result;
}

template <typename Point>
Polygon<Point> simplifiedPolygon(const Polygon<Point>& poly, double nullAreaThreshold)
{
    Polygon<Point> result;

    auto newOuter = simplifiedRing<Point>(poly.outer, nullAreaThreshold, true);
    if (newOuter.points.empty()) {
        return Polygon<Point>{};
    }

    result.outer = std::move(newOuter);

    for(const auto& inner : poly.inners) {
        auto newInner = simplifiedRing<Point>(inner, nullAreaThreshold, false);
        if (!newInner.points.empty()) {
            result.inners.push_back(std::move(newInner));
        }
    }

    std::string message;
    if (!isValidPolygon(result, message)) {
        return poly;
    }

    return result;
}

template <typename Point>
MultiPolygon<Point> simplifiedMultiPolygon(const MultiPolygon<Point>& geometry, double nullAreaThreshold)
{
    MultiPolygon<Point> result;

    for(const auto& part : geometry.parts) {
        auto newPart = simplifiedPolygon<Point>(part, nullAreaThreshold);
        if (!newPart.outer.points.empty()) {
            result.parts.push_back(std::move(newPart));
        }
    }

    std::string message;
    if (!isValidMultiPolygon(result, message)) {
        return geometry;
    }

    return result;
}

template <typename Point>
Geometry<Point> simplifiedGeometry(const Geometry<Point>& geometry, double nullAreaThreshold)
{
    bool ok = true;

    auto result = std::visit(overloaded{
        [&](const Ring<Point>& geometry) {
            return Geometry<Point>{simplifiedRing<Point>(geometry, nullAreaThreshold, true)};
        },
        [&](const Polygon<Point>& geometry) {
            return Geometry<Point>{simplifiedPolygon<Point>(geometry, nullAreaThreshold)};
        },
        [&](const MultiPolygon<Point>& geometry) {
            return Geometry<Point>{simplifiedMultiPolygon<Point>(geometry, nullAreaThreshold)};
        },
        [&](const auto&) {
            ok = false;
            return Geometry<Point>{};
        }
    }, geometry.geometry);

    if (!ok) {
        throw Exception("Error in geometryBoundingBox: Invalid geometry type. Check input geometries.");
    }

    return result;
}

template <typename Geometry>
Geometry simplified(const Geometry& geometry, double nullAreaThreshold)
{
    using Point = typename Geometry::point_type;

    static_assert(
        isRing<Geometry>() ||
        isPolygon<Geometry>() ||
        isMultiPolygon<Geometry>() ||
        isGeometry<Geometry>());

    if constexpr (isRing<Geometry>()) {
        return simplifiedRing<Point>(geometry, nullAreaThreshold, true);
    } else if constexpr (isPolygon<Geometry>()) {
        return simplifiedPolygon<Point>(geometry, nullAreaThreshold);
    } else if constexpr (isMultiPolygon<Geometry>()) {
        return simplifiedMultiPolygon<Point>(geometry, nullAreaThreshold);
    } else if constexpr (isGeometry<Geometry>()) {
        return simplifiedGeometry<Point>(geometry, nullAreaThreshold);
    }
}

} // namespace GEOLIB_NAMESPACE::detail
