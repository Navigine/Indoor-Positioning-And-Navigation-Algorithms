#pragma once

#include "type_checks.h"
#include "util.h"

namespace GEOLIB_NAMESPACE::detail {

template <typename Point>
Box<Point> infiniteBox()
{
    static_assert(isPoint<Point>());

    Box<Point> box;
    util::getX(box.minCorner) = util::getY(box.minCorner) = std::numeric_limits<double>::infinity();
    util::getX(box.maxCorner) = util::getY(box.maxCorner) = -std::numeric_limits<double>::infinity();
    return box;
}

template <typename Point>
void pointExpand(Box<Point>& box, const Point& point)
{
    static_assert(isPoint<Point>());

    util::getX(box.minCorner) = std::min(util::getX(box.minCorner), util::getX(point));
    util::getY(box.minCorner) = std::min(util::getY(box.minCorner), util::getY(point));
    util::getX(box.maxCorner) = std::max(util::getX(box.maxCorner), util::getX(point));
    util::getY(box.maxCorner) = std::max(util::getY(box.maxCorner), util::getY(point));
}

template <typename Point>
void segmentExpand(Box<Point>& box, const Segment<Point>& geometry)
{
    static_assert(isPoint<Point>());

    pointExpand(box, geometry.first);
    pointExpand(box, geometry.second);
}

template <typename Point>
Box<Point> segmentBoundingBox(const Segment<Point>& geometry)
{
    static_assert(isPoint<Point>());

    Box<Point> box;
    util::getX(box.minCorner) = std::min(util::getX(geometry.first), util::getX(geometry.second));
    util::getY(box.minCorner) = std::min(util::getY(geometry.first), util::getY(geometry.second));
    util::getX(box.maxCorner) = std::max(util::getX(geometry.first), util::getX(geometry.second));
    util::getY(box.maxCorner) = std::max(util::getY(geometry.first), util::getY(geometry.second));
    return box;
}

template <typename Point>
void boxExpand(Box<Point>& box, const Box<Point>& geometry)
{
    static_assert(isPoint<Point>());

    pointExpand(box, geometry.minCorner);
    pointExpand(box, geometry.maxCorner);
}

template <typename Point>
Box<Point> boxBoundingBox(const Box<Point>& geometry)
{
    static_assert(isPoint<Point>());

    Box<Point> box;
    box.minCorner = geometry.minCorner;
    box.maxCorner = geometry.maxCorner;
    return box;
}

template <typename Point>
void lineStringExpand(Box<Point>& box, const LineString<Point>& geometry)
{
    static_assert(isPoint<Point>());

    for(const auto& p : geometry.points) {
        pointExpand(box, p);
    }
}

template <typename Point>
Box<Point> lineStringBoundingBox(const LineString<Point>& geometry)
{
    static_assert(isPoint<Point>());

    auto box = infiniteBox<Point>();
    lineStringExpand(box, geometry);

    return box;
}

template <typename Point>
void ringExpand(Box<Point>& box, const Ring<Point>& geometry)
{
    static_assert(isPoint<Point>());
    for(const auto& p : geometry.points) {
        pointExpand(box, p);
    }
}

template <typename Point>
Box<Point> ringBoundingBox(const Ring<Point>& geometry)
{
    static_assert(isPoint<Point>());

    auto box = infiniteBox<Point>();
    ringExpand(box, geometry);

    return box;
}

template <typename Point>
void polygonExpand(Box<Point>& box, const Polygon<Point>& geometry)
{
    static_assert(isPoint<Point>());
    ringExpand(box, geometry.outer);
}

template <typename Point>
Box<Point> polygonBoundingBox(const Polygon<Point>& geometry)
{
    static_assert(isPoint<Point>());

    auto box = infiniteBox<Point>();
    polygonExpand(box, geometry);

    return box;
}

template <typename Point>
void multiPolygonExpand(Box<Point>& box, const MultiPolygon<Point>& geometry)
{
    static_assert(isPoint<Point>());

    for(const auto& part : geometry.parts) {
        polygonExpand(box, part);
    }
}

template <typename Point>
Box<Point> multiPolygonBoundingBox(const MultiPolygon<Point>& geometry)
{
    static_assert(isPoint<Point>());

    auto box = infiniteBox<Point>();
    multiPolygonExpand(box, geometry);

    return box;
}

template <typename Point>
void geometryExpand(Box<Point>& box, const Geometry<Point>& geometry)
{
    static_assert(isPoint<Point>());

    bool ok = true;

    std::visit(overloaded{
        [&](const Segment<Point>& geometry) {
            segmentExpand(box, geometry);
        },
        [&](const Box<Point>& geometry) {
            boxExpand(box, geometry);
        },
        [&](const LineString<Point>& geometry) {
            lineStringExpand(box, geometry);
        },
        [&](const Ring<Point>& geometry) {
            ringExpand(box, geometry);
        },
        [&](const Polygon<Point>& geometry) {
            polygonExpand(box, geometry);
        },
        [&](const MultiPolygon<Point>& geometry) {
            multiPolygonExpand(box, geometry);
        },
        [&](const auto&) {
            ok = false;
        }
    }, geometry.geometry);

    if (!ok) {
        throw Exception("Error in geometryBoundingBox: Invalid geometry type. Check input geometries.");
    }
}

template <typename Point>
Box<Point> geometryBoundingBox(const Geometry<Point>& geometry)
{
    static_assert(isPoint<Point>());

    bool ok = true;

    auto box = std::visit(overloaded{
        [](const Segment<Point>& geometry) {
            return segmentBoundingBox(geometry);
        },
        [](const Box<Point>& geometry) {
            return boxBoundingBox(geometry);
        },
        [](const LineString<Point>& geometry) {
            return lineStringBoundingBox(geometry);
        },
        [](const Ring<Point>& geometry) {
            return ringBoundingBox(geometry);
        },
        [](const Polygon<Point>& geometry) {
            return polygonBoundingBox(geometry);
        },
        [](const MultiPolygon<Point>& geometry) {
            return multiPolygonBoundingBox(geometry);
        },
        [&ok](const auto&) {
            ok = false;
            return Box<Point>{};
        }
    }, geometry.geometry);

    if (!ok) {
        throw Exception("Error in geometryBoundingBox: Invalid geometry type. Check input geometries.");
    }

    return box;
}

template <typename Box, typename Geometry>
Box boundingBox(const Geometry& geometry)
{
    static_assert(
        isSegment<Geometry>() ||
        isBox<Geometry>() ||
        isLineString<Geometry>() ||
        isRing<Geometry>() ||
        isPolygon<Geometry>() ||
        isMultiPolygon<Geometry>() ||
        isGeometry<Geometry>());

    static_assert(std::is_same<typename Geometry::point_type, typename Box::point_type>::value == true);

    using Point = typename Geometry::point_type;

    if constexpr (isSegment<Geometry>()) {
        return segmentBoundingBox<Point>(geometry);
    } else if constexpr (isBox<Geometry>()) {
        return boxBoundingBox<Point>(geometry);
    } else if constexpr (isLineString<Geometry>()) {
        return lineStringBoundingBox<Point>(geometry);
    } else if constexpr (isRing<Geometry>()) {
        return ringBoundingBox<Point>(geometry);
    } else if constexpr (isPolygon<Geometry>()) {
        return polygonBoundingBox<Point>(geometry);
    } else if constexpr (isMultiPolygon<Geometry>()) {
        return multiPolygonBoundingBox<Point>(geometry);
    } else if constexpr (isGeometry<Geometry>()) {
        return geometryBoundingBox<Point>(geometry);
    }
}

template <typename Box, typename Geometry>
void expand(Box& box, const Geometry& geometry)
{
    static_assert(
        isSegment<Geometry>() ||
        isBox<Geometry>() ||
        isLineString<Geometry>() ||
        isRing<Geometry>() ||
        isPolygon<Geometry>() ||
        isMultiPolygon<Geometry>() ||
        isGeometry<Geometry>());

    static_assert(std::is_same<typename Geometry::point_type, typename Box::point_type>::value == true);

    using Point = typename Geometry::point_type;

    if constexpr (isSegment<Geometry>()) {
        segmentExpand<Point>(box, geometry);
    } else if constexpr (isBox<Geometry>()) {
        boxExpand<Point>(box, geometry);
    } else if constexpr (isLineString<Geometry>()) {
        lineStringExpand<Point>(box, geometry);
    } else if constexpr (isRing<Geometry>()) {
        ringExpand<Point>(box, geometry);
    } else if constexpr (isPolygon<Geometry>()) {
        polygonExpand<Point>(box, geometry);
    } else if constexpr (isMultiPolygon<Geometry>()) {
        multiPolygonExpand<Point>(box, geometry);
    } else if constexpr (isGeometry<Geometry>()) {
        geometryExpand<Point>(box, geometry);
    }
}

} // namespace GEOLIB_NAMESPACE::detail
