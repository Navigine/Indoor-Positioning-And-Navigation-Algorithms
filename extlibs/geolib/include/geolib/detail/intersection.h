#pragma once

#include "point_helper.h"
#include "type_checks.h"
#include "util.h"

namespace GEOLIB_NAMESPACE::detail {

template <typename Point>
int segmentIntersection(
    const Point& p1, const Point& p2,
    const Point& q1, const Point& q2,
    double& u1, double& v1, Point& i1,
    double& u2, double& v2, Point& i2,
    double epsilon = 0.0)
{
    static_assert(isPoint<Point>());

    static const auto snapCoef =
        [](double value, double eps) {
            if (eps > 0.0) {
                if (std::fabs(value) < eps) {
                    return 0.0;
                }
                if (std::fabs(value - 1) < eps) {
                    return 1.0;
                }
            }
            return value;
        };

    const auto p1x = util::getX(p1);
    const auto p1y = util::getY(p1);
    const auto p2x = util::getX(p2);
    const auto p2y = util::getY(p2);
    const auto q1x = util::getX(q1);
    const auto q1y = util::getY(q1);
    const auto q2x = util::getX(q2);
    const auto q2y = util::getY(q2);

    const auto pMinX = std::min(p1x, p2x);
    const auto pMaxX = std::max(p1x, p2x);
    const auto pMinY = std::min(p1y, p2y);
    const auto pMaxY = std::max(p1y, p2y);
    const auto qMinX = std::min(q1x, q2x);
    const auto qMaxX = std::max(q1x, q2x);
    const auto qMinY = std::min(q1y, q2y);
    const auto qMaxY = std::max(q1y, q2y);

    // Bounding boxes are not intersecting => segments not intersecting either
    if (pMaxX < qMinX || qMaxX < pMinX ||
        pMaxY < qMinY || qMaxY < pMinY) {
        return 0;
    }

    int mode;
    double a11, a12, a21, a22, b1, b2;

    if (p1x == q2x && p1y == q2y) {
        // p1 == q2
        a11 = p2x - p1x;
        a21 = p2y - p1y;
        a12 = q2x - q1x;
        a22 = q2y - q1y;
        b1 = b2 = 0.0;
        mode = 1;
    } else if (p2x == q1x && p2y == q1y) {
        // p2 == q1
        a11 = p1x - p2x;
        a21 = p1y - p2y;
        a12 = q1x - q2x;
        a22 = q1y - q2y;
        b1 = b2 = 0.0;
        mode = 2;
    } else if (p2x == q2x && p2y == q2y) {
        // p2 == q2
        a11 = p1x - p2x;
        a21 = p1y - p2y;
        a12 = q2x - q1x;
        a22 = q2y - q1y;
        b1 = b2 = 0.0;
        mode = 3;
    } else {
        a11 = p2x - p1x;
        a21 = p2y - p1y;
        a12 = q1x - q2x;
        a22 = q1y - q2y;
        b1 = q1x - p1x;
        b2 = q1y - p1y;
        mode = 0;
    }

    const auto det = a11 * a22 - a12 * a21;
    const auto det1 = b1 * a22 - a12 * b2;
    const auto det2 = a11 * b2 - b1 * a21;

    double u, v;

    if (det != 0) {
        u = det1 / det;
        v = det2 / det;

        u = snapCoef(u, epsilon);
        v = snapCoef(v, epsilon);

        if (!std::isfinite(u) || !std::isfinite(v) || u < 0.0 || u > 1.0 || v < 0.0 || v > 1.0) {
            return 0;
        }

        u1 = (mode == 0 || mode == 1) ? u : 1 - u;
        v1 = (mode == 0 || mode == 2) ? v : 1 - v;

        if (u1 == 0.0) {
            i1 = p1;
        } else if (u1 == 1.0) {
            i1 = p2;
        } else if (v1 == 0.0) {
            i1 = q1;
        } else if (v1 == 1.0) {
            i1 = q2;
        } else {
            i1 = p1 * (1.0 - u1) + p2 * u1;
        }

        return 1;
    }

    if (det1 != 0.0 || det2 != 0.0) {
        return 0;
    }

    int count = 0;

    // Case: u = 0
    v = std::fabs(a12) > std::fabs(a22) ?
        b1 / a12 :
        b2 / a22;

    v = snapCoef(v, epsilon);

    if (std::isfinite(v) && 0.0 <= v && v <= 1.0) {
        u1 = (mode == 0 || mode == 1) ? 0.0 : 1.0;
        v1 = (mode == 0 || mode == 2) ? v : 1 - v;
        i1 = (mode == 0 || mode == 1) ? p1 : p2;
        count = 1;
    }

    // Case: u = 1
    v = std::fabs(a12) > std::fabs(a22) ?
        (b1 - a11) / a12 :
        (b2 - a21) / a22;

    v = snapCoef(v, epsilon);

    if (std::isfinite(v) && 0.0 <= v && v <= 1.0) {
        if (count == 0) {
            u1 = (mode == 0 || mode == 1) ? 1.0 : 0.0;
            v1 = (mode == 0 || mode == 2) ? v : 1 - v;
            i1 = (mode == 0 || mode == 1) ? p2 : p1;
            count = 1;
        } else {
            u2 = (mode == 0 || mode == 1) ? 1.0 : 0.0;
            v2 = (mode == 0 || mode == 2) ? v : 1 - v;
            i2 = (mode == 0 || mode == 1) ? p2 : p1;
            return 2;
        }
    }

    // Case: v = 0
    u = std::fabs(a11) > std::fabs(a21) ?
        b1 / a11 :
        b2 / a21;

    u = snapCoef(u, epsilon);

    if (std::isfinite(u) && 0.0 < u && u < 1.0) {
        if (count == 0) {
            u1 = (mode == 0 || mode == 1) ? u : 1 - u;
            v1 = (mode == 0 || mode == 2) ? 0.0 : 1.0;
            i1 = (mode == 0 || mode == 2) ? q1 : q2;
            count = 1;
        } else {
            u2 = (mode == 0 || mode == 1) ? u : 1 - u;
            v2 = (mode == 0 || mode == 2) ? 0.0 : 1.0;
            i2 = (mode == 0 || mode == 2) ? q1 : q2;
            return 2;
        }
    }

    // Case: v = 1
    u = std::fabs(a11) > std::fabs(a21) ?
        (b1 - a12) / a11 :
        (b2 - a22) / a21;

    u = snapCoef(u, epsilon);

    if (std::isfinite(u) && 0.0 < u && u < 1.0) {
        if (count == 0) {
            u1 = (mode == 0 || mode == 1) ? u : 1 - u;
            v1 = (mode == 0 || mode == 2) ? 1.0 : 0.0;
            i1 = (mode == 0 || mode == 2) ? q2 : q1;
            count = 1;
        } else {
            u2 = (mode == 0 || mode == 1) ? u : 1 - u;
            v2 = (mode == 0 || mode == 2) ? 1.0 : 0.0;
            i2 = (mode == 0 || mode == 2) ? q2 : q1;
            return 2;
        }
    }

    return count;
}

} // namespace GEOLIB_NAMESPACE::detail
