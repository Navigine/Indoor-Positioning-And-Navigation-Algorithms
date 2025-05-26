#pragma once

#include "type_checks.h"
#include "util.h"

namespace GEOLIB_NAMESPACE::detail {

template <typename Geometry>
unsigned int numPoints(const Geometry& geometry)
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
        return 1;
    } else if constexpr (isSegment<Geometry>()) {
        return 2;
    } else if constexpr (isBox<Geometry>()) {
        return 5;
    } else if constexpr (isLineString<Geometry>() || isRing<Geometry>()) {
        return geometry.points.size();
    } else if constexpr (isPolygon<Geometry>()) {
        unsigned int size = detail::numPoints(geometry.outer);
        for(const auto& inner : geometry.inners) {
            size += detail::numPoints(inner);
        }
        return size;
    } else if constexpr (isMultiPolygon<Geometry>()) {
        unsigned int size = 0;
        for(const auto& part : geometry.parts) {
            size += detail::numPoints(part);
        }
        return size;
    } else if constexpr (isGeometry<Geometry>()) {
        return std::visit(
            [](const auto& arg) {
                return detail::numPoints(arg);
            }, geometry.geometry);
    }
}

} // namespace GEOLIB_NAMESPACE::detail
