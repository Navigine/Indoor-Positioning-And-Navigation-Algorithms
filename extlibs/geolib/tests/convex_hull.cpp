#include "point.h"

namespace {

const auto PRECISION = 16;
const auto EPSILON = 1e-10;

void test_convex_hull(
    const std::vector<XYPoint>& inputPoints,
    const std::vector<XYPoint>& expectedPoints)
{
    std::stringstream ss;
    ss << std::setprecision(PRECISION);
    ss << "test_convex_hull:" << std::endl;

    for(const auto& p : inputPoints) {
        ss << "  " << geolib::toWkt(p, PRECISION) << std::endl;
    }

    auto result = geolib::convexHull(inputPoints);

    if (!std::equal(
        result.begin(), result.end(),
        expectedPoints.begin(), expectedPoints.end(),
        [&](auto p1, auto p2) {
            return geolib::equal(p1, p2, EPSILON);
        }))
    {
        ss << "result:   ";
        for(auto p : result) {
            ss << " " << geolib::toWkt(p, PRECISION);
        }
        ss << std::endl;

        ss << "expected: ";
        for(auto p : expectedPoints) {
            ss << " " << geolib::toWkt(p, PRECISION);
        }
        ss << std::endl;
        throw std::runtime_error(ss.str());
    }

    std::cout << ss.str() << std::endl;
}

} // namespace

void test_convex_hull_1()
{
    test_convex_hull({
            XYPoint{1, 0}, XYPoint{2, 0}, XYPoint{3, 0},
            XYPoint{0, 1}, XYPoint{1, 1}, XYPoint{2, 1}, XYPoint{3, 1},
            XYPoint{0, 2}, XYPoint{1, 2}, XYPoint{2, 2}, XYPoint{3, 2},
            XYPoint{0, 3}, XYPoint{1, 3}, XYPoint{2, 3}, XYPoint{3, 3},
            XYPoint{1, 0}, XYPoint{2, 0}, XYPoint{3, 0},                    // duplicating points
            XYPoint{0, 3}, XYPoint{1, 3}, XYPoint{2, 3}, XYPoint{3, 3},     // duplicating points
        }, {
            XYPoint{1, 0}, XYPoint{3, 0}, XYPoint{3, 3}, XYPoint{0, 3}, XYPoint{0, 1},
        });
}
