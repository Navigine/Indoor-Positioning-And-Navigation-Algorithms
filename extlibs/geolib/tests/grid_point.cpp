#include "point.h"

namespace {

using GridPoint = geolib::detail::GridPoint;

void testIntersection(
    const std::string& name,
    const GridPoint& p1,
    const GridPoint& p2,
    const GridPoint& q1,
    const GridPoint& q2,
    const int expected_count,
    std::optional<GridPoint> expected_i1 = std::nullopt,
    std::optional<GridPoint> expected_i2 = std::nullopt)
{
    GridPoint i1, i2;

    std::stringstream ss;
    ss << name << ":" << std::endl;
    ss << "  p1 = (" << p1.x << ", " << p1.y << ")" << std::endl;
    ss << "  p2 = (" << p2.x << ", " << p2.y << ")" << std::endl;
    ss << "  q1 = (" << q1.x << ", " << q1.y << ")" << std::endl;
    ss << "  q2 = (" << q2.x << ", " << q2.y << ")" << std::endl;

    int count = geolib::detail::gridSegmentIntersection(p1, p2, q1, q2, i1, i2);

    ss << "Result:" << std::endl;
    ss << "  count = " << count << std::endl;
    if (count >= 1) {
        ss << "  i1 = (" << i1.x << ", " << i1.y << ")" << std::endl;
    }
    if (count >= 2) {
        ss << "  i2 = (" << i2.x << ", " << i2.y << ")" << std::endl;
    }

    bool ok = true;

    if (count != expected_count) {
        ss << "*** Test failed: count=" << count << ", expected=" << expected_count << std::endl;
        ok = false;
    }

    if (expected_i1 && i1 != *expected_i1) {
        ss << "*** Test failed: i1=(" << i1.x << ", " << i1.y << "), "
           << "expected=(" << expected_i1->x << ", " << expected_i1->y << ")" << std::endl;
        ok = false;
    }

    if (expected_i2 && i2 != *expected_i2) {
        ss << "*** Test failed: i2=(" << i2.x << ", " << i2.y << "), "
           << "expected: (" << expected_i2->x << ", " << expected_i2->y << ")" << std::endl;
        ok = false;
    }

    if (!ok) {
        throw std::runtime_error(ss.str());
    }

    std::cout << ss.str() << std::endl;
}

} // namespace

void test_grid_segment_intersection_1()
{
    testIntersection(__FUNCTION__,
        GridPoint{4000000000000LL, 1000000000000LL},
        GridPoint{1000000000000LL, 1000000000000LL},
        GridPoint{3000000000000LL, 0},
        GridPoint{3000000000000LL, 2000000000000LL},
        1,
        GridPoint{3000000000000LL, 1000000000000LL});
}

void test_grid_segment_intersection_2()
{
    testIntersection(__FUNCTION__,
        GridPoint{0LL, 0LL},
        GridPoint{2000000000000LL, 2000000000000LL},
        GridPoint{0LL, 4000000000000LL},
        GridPoint{2000000000000LL, 0LL},
        1,
        GridPoint{1333333333333LL, 1333333333333LL});
}

void test_grid_segment_intersection_3()
{
    testIntersection(__FUNCTION__,
        GridPoint{0, 0},
        GridPoint{2000, 2000},
        GridPoint{1000, 1000},
        GridPoint{3000, 3000},
        2,
        GridPoint{2000, 2000},
        GridPoint{1000, 1000});
}

void test_grid_segment_intersection_4()
{
    testIntersection(__FUNCTION__,
        GridPoint{0, 0},
        GridPoint{2000, 2000},
        GridPoint{3000, 3000},
        GridPoint{2000, 2000},
        1,
        GridPoint{2000, 2000});
}

void test_grid_segment_intersection_5()
{
    testIntersection(__FUNCTION__,
        GridPoint{0, 0},
        GridPoint{2000, 2000},
        GridPoint{2000, 2000},
        GridPoint{0, 0},
        2,
        GridPoint{0, 0},
        GridPoint{2000, 2000});
}

void test_grid_segment_intersection_6()
{
    testIntersection(__FUNCTION__,
        GridPoint{0, 0},
        GridPoint{3000, 3000},
        GridPoint{1000, 1000},
        GridPoint{2000, 2000},
        2,
        GridPoint{1000, 1000},
        GridPoint{2000, 2000});
}

void test_grid_segment_intersection_7()
{
    testIntersection(__FUNCTION__,
        GridPoint{0, 0},
        GridPoint{2000, 2000},
        GridPoint{1000, 0},
        GridPoint{3000, 0},
        0);
}

void test_grid_segment_intersection_8()
{
    testIntersection(__FUNCTION__,
        GridPoint{1000, 1000},
        GridPoint{2000, 0},
        GridPoint{0, 2000},
        GridPoint{1000, 1000},
        1,
        GridPoint{1000, 1000});
}
