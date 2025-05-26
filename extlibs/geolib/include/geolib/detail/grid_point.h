#pragma once

#include "int128.h"
#include "point_helper.h"
#include "type_checks.h"
#include "util.h"

namespace GEOLIB_NAMESPACE::detail {

struct GridPoint
{
    int64_t x = 0;
    int64_t y = 0;
};

inline bool operator==(const GridPoint& lhs, const GridPoint& rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!=(const GridPoint& lhs, const GridPoint& rhs)
{
    return lhs.x != rhs.x || lhs.y != rhs.y;
}

inline bool operator<(const GridPoint& lhs, const GridPoint& rhs)
{
    return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y);
}

inline bool operator>(const GridPoint& lhs, const GridPoint& rhs)
{
    return lhs.x > rhs.x || (lhs.x == rhs.x && lhs.y > rhs.y);
}

inline int gridSegmentIntersection(
    const GridPoint& p1, const GridPoint& p2,
    const GridPoint& q1, const GridPoint& q2,
    GridPoint& i1,
    GridPoint& i2)
{
    // Check bounds for coordinates (to avoid integer overflows)
    [[maybe_unused]] static const int64_t MAX = 1LL << 61;
    [[maybe_unused]] static const int64_t MIN = -MAX;
    assert(MIN < p1.x && p1.x < MAX);
    assert(MIN < p1.y && p1.y < MAX);
    assert(MIN < p2.x && p2.x < MAX);
    assert(MIN < p2.y && p2.y < MAX);
    assert(MIN < q1.x && q1.x < MAX);
    assert(MIN < q1.y && q1.y < MAX);
    assert(MIN < q2.x && q2.x < MAX);
    assert(MIN < q2.y && q2.y < MAX);

    // Check that p1 != p2 and q1 != q2
    assert(p1.x != p2.x || p1.y != p2.y);
    assert(q1.x != q2.x || q1.y != q2.y);

    const int64_t pMinX = std::min(p1.x, p2.x);
    const int64_t pMaxX = std::max(p1.x, p2.x);
    const int64_t pMinY = std::min(p1.y, p2.y);
    const int64_t pMaxY = std::max(p1.y, p2.y);
    const int64_t qMinX = std::min(q1.x, q2.x);
    const int64_t qMaxX = std::max(q1.x, q2.x);
    const int64_t qMinY = std::min(q1.y, q2.y);
    const int64_t qMaxY = std::max(q1.y, q2.y);

    // Bounding boxes are not intersecting => segments are not intersecting either
    if (pMaxX < qMinX || qMaxX < pMinX ||
        pMaxY < qMinY || qMaxY < pMinY) {
        return 0;
    }

    const int64_t a11 = p2.x - p1.x;
    const int64_t a21 = p2.y - p1.y;
    const int64_t a12 = q1.x - q2.x;
    const int64_t a22 = q1.y - q2.y;
    const int64_t b1 = q1.x - p1.x;
    const int64_t b2 = q1.y - p1.y;

    // Solving system of equations:
    //  a11 * u + a12 * v = b1
    //  a21 * u + a22 * v = b2

    const auto det = det128(a11, a12, a21, a22);
    const auto det1 = det128(b1, a12, b2, a22);
    const auto det2 = det128(a11, b1, a21, b2);

    if (det != 0) {
        // Segments are not collinear => at most 1 common point
        if (p1 == q1 || p1 == q2) {
            i1 = p1;
            return 1;
        }
        if (p2 == q1 || p2 == q2) {
            i1 = p2;
            return 1;
        }

        // u = det1 / det
        // v = det2 / det
        // Check: 0 <= u <= 1
        // Check: 0 <= v <= 1
        if (det > 0) {
            if ((det1 < 0 || det1 > det) || (det2 < 0 || det2 > det)) {
                return 0; // No solutions
            }
        } else {
            if ((det1 > 0 || det1 < det) || (det2 > 0 || det2 < det)) {
                return 0; // No solutions
            }
        }

        if (det1 == 0) {
            i1 = p1;
        }
        else if (det1 == det) {
            i1 = p2;
        }
        else if (det2 == 0) {
            i1 = q1;
        }
        else if (det2 == det) {
            i1 = q2;
        } else {
            i1.x = p1.x + rdiv128(det1, a11, det);
            i1.y = p1.y + rdiv128(det1, a21, det);
        }
        return 1;
    }

    if (det1 != 0 || det2 != 0) {
        return 0;
    }

    int count = 0;

    // Case: u = 0
    // v = b1 / a12     (if a12 != 0)
    // v = b2 / a22     (if a22 != 0)
    // Check: 0 <= v <= 1
    if ((a12 > 0 && 0 <= b1 && b1 <= a12) ||
        (a12 < 0 && 0 >= b1 && b1 >= a12) ||
        (a22 > 0 && 0 <= b2 && b2 <= a22) ||
        (a22 < 0 && 0 >= b2 && b2 >= a22)) {
        i1 = p1;
        count = 1;
    }

    // Case: u = 1
    // v = (b1 - a11) / a12     (if a12 != 0)
    // v = (b2 - a21) / a22     (if a22 != 0)
    // Check: 0 <= v <= 1
    if ((a12 > 0 && a11 <= b1 && b1 <= a11 + a12) ||
        (a12 < 0 && a11 >= b1 && b1 >= a11 + a12) ||
        (a22 > 0 && a21 <= b2 && b2 <= a21 + a22) ||
        (a22 < 0 && a21 >= b2 && b2 >= a21 + a22)) {
        if (count == 0) {
            i1 = p2;
            count = 1;
        } else {
            i2 = p2;
            return 2;
        }
    }

    // Case: v = 0
    // u = b1 / a11     (if a11 != 0)
    // u = b2 / a21     (if a21 != 0)
    // Check: 0 < u < 1
    if ((a11 > 0 && 0 < b1 && b1 < a11) ||
        (a11 < 0 && 0 > b1 && b1 > a11) ||
        (a21 > 0 && 0 < b2 && b2 < a21) ||
        (a21 < 0 && 0 > b2 && b2 > a21)) {
        if (count == 0) {
            i1 = q1;
            count = 1;
        } else {
            i2 = q1;
            return 2;
        }
    }

    // Case: v = 1
    // u = (b1 - a12) / a11     (if a11 != 0)
    // u = (b2 - a22) / a21     (if a21 != 0)
    // Check: 0 < u < 1
    if ((a11 > 0 && a12 < b1 && b1 < a12 + a11) ||
        (a11 < 0 && a12 > b1 && b1 > a12 + a11) ||
        (a21 > 0 && a22 < b2 && b2 < a22 + a21) ||
        (a21 < 0 && a22 > b2 && b2 > a22 + a21)) {
        if (count == 0) {
            i1 = q2;
            count = 1;
        } else {
            i2 = q2;
            return 2;
        }
    }

    return count;
}

inline int updateGridPointIndex(const GridPoint& point, const GridPoint& A, const GridPoint& B, int index)
{
    // Check bounds for coordinates (to avoid integer overflows)
    [[maybe_unused]] static const int64_t MAX = 1LL << 62;
    [[maybe_unused]] static const int64_t MIN = -MAX;
    assert(MIN < point.x && point.x < MAX);
    assert(MIN < point.y && point.y < MAX);
    assert(MIN < A.x && A.x < MAX);
    assert(MIN < A.y && A.y < MAX);
    assert(MIN < B.x && B.x < MAX);
    assert(MIN < B.y && B.y < MAX);

    if (!index) {
        return 0;
    }

    int64_t px = point.x;
    int64_t py = point.y;
    int64_t ax = A.x;
    int64_t ay = A.y;
    int64_t bx = B.x;
    int64_t by = B.y;

    if (px == ax && py == ay) {
        return 0;
    }

    if (px == bx && py == by) {
        return 0;
    }

    if (ay > by) {
        std::swap(ax, bx);
        std::swap(ay, by);
    }

    if (ay == by && ay == py) {
        if ((ax <= px && px <= bx) || (bx <= px && px <= ax)) {
            return 0;
        }
        return index;
    }

    if (ay >= py || by < py) {
        return index;
    }

    assert(ay < py && py <= by);

    const auto det = det128(ax - px, bx - ax, ay - py, by - ay);

    if (det == 0) {
        return 0;
    }

    return det < 0 ? index : -index;
}

} // namespace GEOLIB_NAMESPACE::detail
