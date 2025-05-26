#pragma once

#include "binary_operation.h"
#include "convex_hull.h"
#include "iterate_edges.h"
#include "type_checks.h"
#include "util.h"

namespace GEOLIB_NAMESPACE::detail {

const auto TABLE_SIN_COS_0_180 = std::vector<std::tuple<double,double>>{
    {0.000000, 1.000000},
    {0.017452, 0.999848},
    {0.034899, 0.999391},
    {0.052336, 0.998630},
    {0.069756, 0.997564},
    {0.087156, 0.996195},
    {0.104528, 0.994522},
    {0.121869, 0.992546},
    {0.139173, 0.990268},
    {0.156434, 0.987688},
    {0.173648, 0.984808},
    {0.190809, 0.981627},
    {0.207912, 0.978148},
    {0.224951, 0.974370},
    {0.241922, 0.970296},
    {0.258819, 0.965926},
    {0.275637, 0.961262},
    {0.292372, 0.956305},
    {0.309017, 0.951057},
    {0.325568, 0.945519},
    {0.342020, 0.939693},
    {0.358368, 0.933580},
    {0.374607, 0.927184},
    {0.390731, 0.920505},
    {0.406737, 0.913545},
    {0.422618, 0.906308},
    {0.438371, 0.898794},
    {0.453990, 0.891007},
    {0.469472, 0.882948},
    {0.484810, 0.874620},
    {0.500000, 0.866025},
    {0.515038, 0.857167},
    {0.529919, 0.848048},
    {0.544639, 0.838671},
    {0.559193, 0.829038},
    {0.573576, 0.819152},
    {0.587785, 0.809017},
    {0.601815, 0.798636},
    {0.615661, 0.788011},
    {0.629320, 0.777146},
    {0.642788, 0.766044},
    {0.656059, 0.754710},
    {0.669131, 0.743145},
    {0.681998, 0.731354},
    {0.694658, 0.719340},
    {0.707107, 0.707107},
    {0.719340, 0.694658},
    {0.731354, 0.681998},
    {0.743145, 0.669131},
    {0.754710, 0.656059},
    {0.766044, 0.642788},
    {0.777146, 0.629320},
    {0.788011, 0.615661},
    {0.798636, 0.601815},
    {0.809017, 0.587785},
    {0.819152, 0.573576},
    {0.829038, 0.559193},
    {0.838671, 0.544639},
    {0.848048, 0.529919},
    {0.857167, 0.515038},
    {0.866025, 0.500000},
    {0.874620, 0.484810},
    {0.882948, 0.469472},
    {0.891007, 0.453990},
    {0.898794, 0.438371},
    {0.906308, 0.422618},
    {0.913545, 0.406737},
    {0.920505, 0.390731},
    {0.927184, 0.374607},
    {0.933580, 0.358368},
    {0.939693, 0.342020},
    {0.945519, 0.325568},
    {0.951057, 0.309017},
    {0.956305, 0.292372},
    {0.961262, 0.275637},
    {0.965926, 0.258819},
    {0.970296, 0.241922},
    {0.974370, 0.224951},
    {0.978148, 0.207912},
    {0.981627, 0.190809},
    {0.984808, 0.173648},
    {0.987688, 0.156434},
    {0.990268, 0.139173},
    {0.992546, 0.121869},
    {0.994522, 0.104528},
    {0.996195, 0.087156},
    {0.997564, 0.069756},
    {0.998630, 0.052336},
    {0.999391, 0.034899},
    {0.999848, 0.017452},
    {1.000000, 0.000000},
    {0.999848, -0.017452},
    {0.999391, -0.034899},
    {0.998630, -0.052336},
    {0.997564, -0.069756},
    {0.996195, -0.087156},
    {0.994522, -0.104528},
    {0.992546, -0.121869},
    {0.990268, -0.139173},
    {0.987688, -0.156434},
    {0.984808, -0.173648},
    {0.981627, -0.190809},
    {0.978148, -0.207912},
    {0.974370, -0.224951},
    {0.970296, -0.241922},
    {0.965926, -0.258819},
    {0.961262, -0.275637},
    {0.956305, -0.292372},
    {0.951057, -0.309017},
    {0.945519, -0.325568},
    {0.939693, -0.342020},
    {0.933580, -0.358368},
    {0.927184, -0.374607},
    {0.920505, -0.390731},
    {0.913545, -0.406737},
    {0.906308, -0.422618},
    {0.898794, -0.438371},
    {0.891007, -0.453990},
    {0.882948, -0.469472},
    {0.874620, -0.484810},
    {0.866025, -0.500000},
    {0.857167, -0.515038},
    {0.848048, -0.529919},
    {0.838671, -0.544639},
    {0.829038, -0.559193},
    {0.819152, -0.573576},
    {0.809017, -0.587785},
    {0.798636, -0.601815},
    {0.788011, -0.615661},
    {0.777146, -0.629320},
    {0.766044, -0.642788},
    {0.754710, -0.656059},
    {0.743145, -0.669131},
    {0.731354, -0.681998},
    {0.719340, -0.694658},
    {0.707107, -0.707107},
    {0.694658, -0.719340},
    {0.681998, -0.731354},
    {0.669131, -0.743145},
    {0.656059, -0.754710},
    {0.642788, -0.766044},
    {0.629320, -0.777146},
    {0.615661, -0.788011},
    {0.601815, -0.798636},
    {0.587785, -0.809017},
    {0.573576, -0.819152},
    {0.559193, -0.829038},
    {0.544639, -0.838671},
    {0.529919, -0.848048},
    {0.515038, -0.857167},
    {0.500000, -0.866025},
    {0.484810, -0.874620},
    {0.469472, -0.882948},
    {0.453990, -0.891007},
    {0.438371, -0.898794},
    {0.422618, -0.906308},
    {0.406737, -0.913545},
    {0.390731, -0.920505},
    {0.374607, -0.927184},
    {0.358368, -0.933580},
    {0.342020, -0.939693},
    {0.325568, -0.945519},
    {0.309017, -0.951057},
    {0.292372, -0.956305},
    {0.275637, -0.961262},
    {0.258819, -0.965926},
    {0.241922, -0.970296},
    {0.224951, -0.974370},
    {0.207912, -0.978148},
    {0.190809, -0.981627},
    {0.173648, -0.984808},
    {0.156434, -0.987688},
    {0.139173, -0.990268},
    {0.121869, -0.992546},
    {0.104528, -0.994522},
    {0.087156, -0.996195},
    {0.069756, -0.997564},
    {0.052336, -0.998630},
    {0.034899, -0.999391},
    {0.017452, -0.999848},
    {0.000000, -1.000000},
};

template <typename Point>
void pointsInCircle(
    const Point& center,
    double radius,
    unsigned int numPointsSemiCircle,
    std::vector<Point>& points)
{
    static_assert(isPoint<Point>());

    numPointsSemiCircle = std::min(numPointsSemiCircle, 180u);
    unsigned int delta = 180u / numPointsSemiCircle;
    unsigned int mod = 180u % numPointsSemiCircle;

    for(unsigned int i = 0; i < numPointsSemiCircle; ++i) {
        unsigned int angle = i * delta + std::min(i, mod);
        const double sinAngle = std::get<0>(TABLE_SIN_COS_0_180[angle]);
        const double cosAngle = std::get<1>(TABLE_SIN_COS_0_180[angle]);
        Point point1, point2;

        util::getX(point1) = util::getX(center) + radius * cosAngle;
        util::getY(point1) = util::getY(center) + radius * sinAngle;
        points.push_back(std::move(point1));

        util::getX(point2) = util::getX(center) - radius * cosAngle;
        util::getY(point2) = util::getY(center) - radius * sinAngle;
        points.push_back(std::move(point2));
    }
}

template <typename Point>
MultiPolygon<Point> pointBuffer(
    const Point& point,
    double radius,
    unsigned int numPointsSemiCircle)
{
    static_assert(isPoint<Point>());

    std::vector<Point> points;
    pointsInCircle(point, radius, numPointsSemiCircle, points);

    MultiPolygon<Point> result;
    result.parts.resize(1);
    result.parts[0].outer.points = detail::convexHull(points);
    detail::correct(result);

    return result;
}

template <typename Point>
MultiPolygon<Point> segmentBuffer(
    const Segment<Point>& segment,
    double radius,
    unsigned int numPointsSemiCircle)
{
    static_assert(isPoint<Point>());

    std::vector<Point> points;
    pointsInCircle(segment.first, radius, numPointsSemiCircle, points);
    pointsInCircle(segment.second, radius, numPointsSemiCircle, points);

    MultiPolygon<Point> result;
    result.parts.resize(1);
    result.parts[0].outer.points = detail::convexHull(points);
    detail::correct(result);

    return result;
}

template <typename Point>
MultiPolygon<Point> lineStringBuffer(
    const LineString<Point>& linestring,
    double radius,
    unsigned int numPointsSemiCircle)
{
    static_assert(isPoint<Point>());

    std::vector<MultiPolygon<Point>> parts;

    iterateEdges(linestring, false,
        [&](const Point& p1, const Point& p2) {
            parts.push_back(segmentBuffer(Segment<Point>{p1, p2}, radius, numPointsSemiCircle));
            return true;
        });

    MultiPolygon<Point> result;

    for(const auto& part : parts) {
        try {
            result = binaryOperation(BinaryOperationType::UNION, result, part);
        } catch (Exception& e) {
            throw Exception(GEOLIB_FORMAT("Error in buffer. " << e.what()));
        }
    }

    return result;
}

template <typename Geometry>
MultiPolygon<typename Geometry::point_type> fatBuffer(
    const Geometry& geometry,
    double radius,
    unsigned int numPointsSemiCircle)
{
    static_assert(isBox<Geometry>() || isRing<Geometry>() || isPolygon<Geometry>() || isMultiPolygon<Geometry>());

    using Point = typename Geometry::point_type;

    std::vector<MultiPolygon<Point>> parts;

    iterateEdges(geometry, false,
        [&](const Point& p1, const Point& p2) {
            parts.push_back(segmentBuffer(Segment<Point>{p1, p2}, radius, numPointsSemiCircle));
            return true;
        });

    MultiPolygon<Point> result = binaryOperation(BinaryOperationType::UNION, geometry, MultiPolygon<Point>{});

    for(const auto& part : parts) {
        try {
            result = binaryOperation(BinaryOperationType::UNION, result, part);
        } catch (Exception& e) {
            throw Exception(GEOLIB_FORMAT("Error in buffer. " << e.what()));
        }
    }

    return result;
}

template <typename Point>
MultiPolygon<Point> geometryBuffer(
    const Geometry<Point>& geometry,
    double radius,
    unsigned int numPointsSemiCircle)
{
    static_assert(isPoint<Point>());

    MultiPolygon<Point> result;
    std::string error;

    std::visit(overloaded{
        [&](const Point& geometry) {
            result = pointBuffer(geometry, radius, numPointsSemiCircle);
        },
        [&](const Segment<Point>& geometry) {
            result = segmentBuffer(geometry, radius, numPointsSemiCircle);
        },
        [&](const LineString<Point>& geometry) {
            try {
                result = lineStringBuffer(geometry, radius, numPointsSemiCircle);
            } catch (Exception& e) {
                error = GEOLIB_FORMAT("Error in buffer. " << e.what());
            }
        },
        [&](const auto& geometry) {
            try {
                result = fatBuffer(geometry, radius, numPointsSemiCircle);
            } catch (Exception& e) {
                error = GEOLIB_FORMAT("Error in buffer. " << e.what());
            }
        }
    }, geometry.geometry);

    if (!error.empty()) {
        throw Exception(error);
    }

    return result;
}

template <typename MultiPolygon, typename Geometry>
MultiPolygon buffer(
    const Geometry& geometry,
    double radius,
    unsigned int numPointsSemiCircle)
{
    static_assert(isMultiPolygon<MultiPolygon>());

    static_assert(
        isPoint<Geometry>() ||
        isSegment<Geometry>() ||
        isLineString<Geometry>() ||
        isBox<Geometry>() ||
        isRing<Geometry>() ||
        isPolygon<Geometry>() ||
        isMultiPolygon<Geometry>() ||
        isGeometry<Geometry>());

    if (radius <= 0.0) {
        throw Exception("Buffer radius should be positive");
    }

    if (numPointsSemiCircle < 2 || numPointsSemiCircle > 180) {
        throw Exception("Buffer number of points in semi-circle should be within bounds [2, 180]");
    }

    if constexpr (isPoint<Geometry>()) {
        return pointBuffer(geometry, radius, numPointsSemiCircle);
    } else if constexpr (isSegment<Geometry>()) {
        return segmentBuffer(geometry, radius, numPointsSemiCircle);
    } else if constexpr (isLineString<Geometry>()) {
        return lineStringBuffer(geometry, radius, numPointsSemiCircle);
    } else if constexpr (isBox<Geometry>() || isRing<Geometry>() || isPolygon<Geometry>() || isMultiPolygon<Geometry>()) {
        return fatBuffer(geometry, radius, numPointsSemiCircle);
    } else if constexpr (isGeometry<Geometry>()) {
        return geometryBuffer(geometry, radius, numPointsSemiCircle);
    }
}

} // namespace GEOLIB_NAMESPACE::detail
