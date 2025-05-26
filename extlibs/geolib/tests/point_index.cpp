#include "point.h"

namespace {

const auto PRECISION = 16;

void test_point_index(
    const std::string& wkt,
    const XYPoint& point,
    const int expectedIndex)
{
    XYMultiPolygon geom;

    std::stringstream ss;
    ss << std::setprecision(PRECISION);
    ss << "test_point_index: " << geolib::toWkt(point, PRECISION) << std::endl;

    try {
        geom = geolib::fromWkt<XYMultiPolygon>(wkt);
    } catch (const geolib::Exception& e) {
        ss << "Error parsing wkt string: " << wkt << " error: " << e.what() << std::endl;
        throw std::runtime_error(ss.str());
    }

    geolib::correct(geom);
    ss << geolib::toWkt(geom, PRECISION) << std::endl;

    auto index = geolib::detail::pointIndex(geom, point);

    if (index != expectedIndex) {
        ss << "result:   " << index << std::endl;
        ss << "expected: " << expectedIndex << std::endl;
        throw std::runtime_error(ss.str());
    }

    std::cout << ss.str() << std::endl;
}

} // namespace

void test_point_index_1()
{
    // Outer point
    test_point_index(
        "MULTIPOLYGON(((1 0, 3 0, 3 3, 0 3, 0 1, 1 1, 1 0), (1 1, 1 2, 2 2, 2 1, 1 1)))",
        XYPoint{0, 0}, -1);
}

void test_point_index_2()
{
    // Outer point
    test_point_index(
        "MULTIPOLYGON(((1 0, 3 0, 3 3, 0 3, 0 1, 1 1, 1 0), (1 1, 1 2, 2 2, 2 1, 1 1)))",
        XYPoint{-1, 1}, -1);
}

void test_point_index_3()
{
    // Outer point
    test_point_index(
        "MULTIPOLYGON(((1 0, 3 0, 3 3, 0 3, 0 1, 1 1, 1 0), (1 1, 1 2, 2 2, 2 1, 1 1)))",
        XYPoint{1.5, 1.5}, -1);
}

void test_point_index_4()
{
    // Border point
    test_point_index(
        "MULTIPOLYGON(((1 0, 3 0, 3 3, 0 3, 0 1, 1 1, 1 0), (1 1, 1 2, 2 2, 2 1, 1 1)))",
        XYPoint{0.5, 1}, 0);
}

void test_point_index_5()
{
    // Border point
    test_point_index(
        "MULTIPOLYGON(((1 0, 3 0, 3 3, 0 3, 0 1, 1 1, 1 0), (1 1, 1 2, 2 2, 2 1, 1 1)))",
        XYPoint{1, 1}, 0);
}

void test_point_index_6()
{
    // Border point
    test_point_index(
        "MULTIPOLYGON(((1 0, 3 0, 3 3, 0 3, 0 1, 1 1, 1 0), (1 1, 1 2, 2 2, 2 1, 1 1)))",
        XYPoint{1.5, 2}, 0);
}

void test_point_index_7()
{
    // Inner point
    test_point_index(
        "MULTIPOLYGON(((1 0, 3 0, 3 3, 0 3, 0 1, 1 1, 1 0), (1 1, 1 2, 2 2, 2 1, 1 1)))",
        XYPoint{0.5, 2}, 1);
}

void test_point_index_8()
{
    // Inner point
    test_point_index(
        "MULTIPOLYGON(((1 0, 3 0, 3 3, 0 3, 0 1, 1 1, 1 0), (1 1, 1 2, 2 2, 2 1, 1 1)))",
        XYPoint{2, 0.5}, 1);
}

void test_point_index_9()
{
    // Inner point
    test_point_index(
        "MULTIPOLYGON(((1 0, 3 0, 3 3, 0 3, 0 1, 1 1, 1 0), (1 1, 1 2, 2 2, 2 1, 1 1)))",
        XYPoint{2.1, 2.1}, 1);
}

void test_point_index_10()
{
    // Border point
    test_point_index(
        "MULTIPOLYGON(((0 0, 2 2, 3 4, 0 0)))",
        XYPoint{0, 0},
        0);
}
