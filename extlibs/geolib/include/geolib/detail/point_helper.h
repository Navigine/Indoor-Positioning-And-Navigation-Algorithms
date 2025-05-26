#pragma once

#include "util.h"
#include "type_checks.h"

namespace GEOLIB_NAMESPACE::detail {

template <IsPoint Point>
Point operator+(const Point& p1, const Point& p2)
{
    Point result;
    util::getX(result) = util::getX(p1) + util::getX(p2);
    util::getY(result) = util::getY(p1) + util::getY(p2);
    return result;
}

template <IsPoint Point>
Point operator-(const Point& p1, const Point& p2)
{
    Point result;
    util::getX(result) = util::getX(p1) - util::getX(p2);
    util::getY(result) = util::getY(p1) - util::getY(p2);
    return result;
}

template <IsPoint Point>
Point operator*(const Point& p, double multiplier)
{
    Point result;
    util::getX(result) = util::getX(p) * multiplier;
    util::getY(result) = util::getY(p) * multiplier;
    return result;
}

template <IsPoint Point>
Point operator*(double multiplier, const Point& p)
{
    Point result;
    util::getX(result) = util::getX(p) * multiplier;
    util::getY(result) = util::getY(p) * multiplier;
    return result;
}

template <IsPoint Point>
Point operator/(const Point& p, double divisor)
{
    Point result;
    util::getX(result) = util::getX(p) / divisor;
    util::getY(result) = util::getY(p) / divisor;
    return result;
}

template <IsPoint Point>
double distance(const Point& p1, const Point& p2)
{
    double dx = util::getX(p1) - util::getX(p2);
    double dy = util::getY(p1) - util::getY(p2);
    return std::sqrt(dx * dx + dy * dy);
}

template <IsPoint Point>
struct PointHasher
{
    std::size_t operator()(const Point& point) const {
        const std::size_t h1 = std::hash<double>()(util::getX(point));
        const std::size_t h2 = std::hash<double>()(util::getY(point));
        return h1 ^ (h2 << 1);
    }
};

template <IsPoint Point>
struct PointComparator
{
    bool operator()(const Point& point1, const Point& point2) const {
        return util::getX(point1) == util::getX(point2) &&
               util::getY(point1) == util::getY(point2);
    }
};

} // namespace GEOLIB_NAMESPACE::detail
