#pragma once

#include "correct.h"
#include "point_helper.h"
#include "type_checks.h"
#include "wkt_write.h"
#include "util.h"

//#define GEOLIB_CONVEX_HULL_DEBUG_ENABLED

#ifdef GEOLIB_CONVEX_HULL_DEBUG_ENABLED
# include <iostream>
#endif

namespace GEOLIB_NAMESPACE::detail {

template <typename Point>
std::vector<Point> convexHull(const std::vector<Point>& points)
{
    // Graham scan algorithm:
    //  https://en.wikipedia.org/wiki/Graham_scan

    static_assert(isPoint<Point>());

    if (points.size() < 2) {
        return points;
    }

    std::vector<size_t> index(points.size());
    for(size_t i = 0; i < points.size(); ++i) {
        index[i] = i;
    }

    std::stable_sort(index.begin(), index.end(),
        [&](auto i1, auto i2) {
            return (util::getY(points[i1]) < util::getY(points[i2])) ||
                   (util::getY(points[i1]) == util::getY(points[i2]) &&
                    util::getX(points[i1]) < util::getX(points[i2]));
        });

    // Removing points with duplicating coordinates
    auto last = std::unique(index.begin(), index.end(),
        [&](auto i1, auto i2) {
            return util::getX(points[i1]) == util::getX(points[i2]) &&
                   util::getY(points[i1]) == util::getY(points[i2]);
        });

    assert(last != index.begin());

    const auto& p0 = points[index[0]];

    // result > 0, if three points make a counter-clockwise turn,
    // result < 0, if three points make a clockwise turn,
    // result = 0, if points are collinear.
    static const auto checkCCW =
        [](const Point& a, const Point& b, const Point& c) {
            const auto ux = util::getX(b) - util::getX(a);
            const auto uy = util::getY(b) - util::getY(a);
            const auto vx = util::getX(c) - util::getX(b);
            const auto vy = util::getY(c) - util::getY(b);
            return ux * vy - vx * uy;
        };

    std::stable_sort(std::next(index.begin()), last,
        [&](auto i1, auto i2) {
            const auto ccw = checkCCW(p0, points[i1], points[i2]);
            return (ccw > 0.0) || (ccw == 0.0 && distance(points[i1], p0) > distance(points[i2], p0));
        });

    // Removing points with duplicating polar angles
    last = std::unique(std::next(index.begin()), last,
        [&](auto i1, auto i2) {
            return checkCCW(p0, points[i1], points[i2]) == 0.0;
        });

#ifdef GEOLIB_CONVEX_HULL_DEBUG_ENABLED
    std::cout << "Sorted points: ";
    for(auto iter = index.begin(); iter != last; ++iter) {
        std::cout << detail::toWkt(points[*iter], 12, false) << " ";
    }
    std::cout << "\n";
#endif

    std::vector<Point> result;

    size_t n = std::distance(index.begin(), last);

    if (n >= 1) {
        result.push_back(points[index[0]]);
#ifdef GEOLIB_CONVEX_HULL_DEBUG_ENABLED
        std::cout << "Push " << detail::toWkt(result.back(), 12, false) << std::endl;
#endif
    }

    if (n >= 2) {
        result.push_back(points[index[1]]);
#ifdef GEOLIB_CONVEX_HULL_DEBUG_ENABLED
        std::cout << "Push " << detail::toWkt(result.back(), 12, false) << std::endl;
#endif
    }

    for(size_t i = 2; i < n; ++i) {
        while (true) {
            const auto& a = result[result.size() - 2];
            const auto& b = result[result.size() - 1];
            const auto& c = points[index[i]];
            const auto ccw = checkCCW(a, b, c);
#ifdef GEOLIB_CONVEX_HULL_DEBUG_ENABLED
            std::cout << "Check triangle: "
                      << detail::toWkt(a, 12, false) << ", "
                      << detail::toWkt(b, 12, false) << ", "
                      << detail::toWkt(c, 12, false) << ": "
                      << "ccw=" << ccw << std::endl;
#endif
            if (ccw > 0) {
                break;
            }
#ifdef GEOLIB_CONVEX_HULL_DEBUG_ENABLED
            std::cout << "Pop " << detail::toWkt(result.back(), 12, false) << std::endl;
#endif
            result.pop_back();
        }
        result.push_back(points[index[i]]);
#ifdef GEOLIB_CONVEX_HULL_DEBUG_ENABLED
        std::cout << "Push " << detail::toWkt(result.back(), 12, false) << std::endl;
#endif
    }

    return result;
}

} // namespace GEOLIB_NAMESPACE::detail
