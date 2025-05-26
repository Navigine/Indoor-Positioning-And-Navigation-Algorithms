#pragma once

#include "area.h"
#include "compare.h"
#include "type_checks.h"
#include "util.h"

namespace GEOLIB_NAMESPACE::detail {

template <typename Point>
bool checkClockwiseRing(const Ring<Point>& ring)
{
    static_assert(isPoint<Point>());
    return ringArea(ring) < 0;
}

template <typename Point>
void correctRing(Ring<Point>& ring, bool isOuter)
{
    static_assert(isPoint<Point>());

    // Removing consecutive duplicating points in ring
    auto last = std::unique(ring.points.begin(), ring.points.end(),
        [](const auto& p1, const auto& p2) {
            return detail::equal(p1, p2);
        });

    ring.points.erase(last, ring.points.end());

    if (ring.points.size() < 3) {
        return;
    }

    if (!detail::equal(ring.points.back(), ring.points.front())) {
        ring.points.push_back(ring.points.front());
    }

    if (checkClockwiseRing(ring) == isOuter) {
        // Invalid ring orientation
        std::reverse(ring.points.begin(), ring.points.end());
    }
}

template <typename Point>
void sortRing(Ring<Point>& ring)
{
    static_assert(isPoint<Point>());

    // Rearranging points inside ring (so that min point comes first)
    auto first = ring.points.begin();
    auto last = std::next(first, ring.points.size() - 1);
    std::rotate(first, std::min_element(first, last, less<Point>), last);
    ring.points.back() = ring.points.front();
}

template <typename Point>
void correctPolygon(Polygon<Point>& poly)
{
    static_assert(isPoint<Point>());

    correctRing(poly.outer, true);
    for(auto& inner : poly.inners) {
        correctRing(inner, false);
    }
}

template <typename Point>
void sortPolygon(Polygon<Point>& poly)
{
    static_assert(isPoint<Point>());

    sortRing(poly.outer);
    for(auto& inner : poly.inners) {
        sortRing(inner);
    }

    std::stable_sort(poly.inners.begin(), poly.inners.end(), less<Ring<Point>>);
}

template <typename Point>
void correctMultiPolygon(MultiPolygon<Point>& geom)
{
    static_assert(isPoint<Point>());

    for(auto& part : geom.parts) {
        correctPolygon(part);
    }
}

template <typename Point>
void sortMultiPolygon(MultiPolygon<Point>& geom)
{
    static_assert(isPoint<Point>());

    for(auto& part : geom.parts) {
        sortPolygon(part);
    }

    std::stable_sort(geom.parts.begin(), geom.parts.end(), less<Polygon<Point>>);
}

template <typename Point>
void correctGeometry(Geometry<Point>& geom)
{
    static_assert(isPoint<Point>());

    std::visit(overloaded{
        [](Ring<Point>& ring) {
            correctRing(ring, true);
        },
        [](Polygon<Point>& polygon) {
             correctPolygon(polygon);
        },
        [](MultiPolygon<Point>& multipolygon) {
            correctMultiPolygon(multipolygon);
        },
        [](auto& ) {}
    }, geom.geometry);
}

template <typename Point>
void sortGeometry(Geometry<Point>& geom)
{
    static_assert(isPoint<Point>());

    std::visit(overloaded{
        [](Ring<Point>& ring) {
            sortRing(ring);
        },
        [](Polygon<Point>& polygon) {
             sortPolygon(polygon);
        },
        [](MultiPolygon<Point>& multipolygon) {
            sortMultiPolygon(multipolygon);
        },
        [](auto& ) {}
    }, geom.geometry);
}

template <typename Geometry>
void correct(Geometry& geometry)
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

    if constexpr (isRing<Geometry>()) {
        correctRing(geometry, true);
    } else if constexpr (isPolygon<Geometry>()) {
        correctPolygon(geometry);
    } else if constexpr (isMultiPolygon<Geometry>()) {
        correctMultiPolygon(geometry);
    } else if constexpr (isGeometry<Geometry>()) {
        correctGeometry(geometry);
    }
}

template <typename Geometry>
void sort(Geometry& geometry)
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

    if constexpr (isRing<Geometry>()) {
        sortRing(geometry);
    } else if constexpr (isPolygon<Geometry>()) {
        sortPolygon(geometry);
    } else if constexpr (isMultiPolygon<Geometry>()) {
        sortMultiPolygon(geometry);
    } else if constexpr (isGeometry<Geometry>()) {
        sortGeometry(geometry);
    }
}

} // namespace GEOLIB_NAMESPACE::detail
