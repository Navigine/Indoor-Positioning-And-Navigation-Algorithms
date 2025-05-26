#include "point.h"

namespace {

const auto PRECISION = 16;

void test_contains(
    const std::string& wkt1,
    const std::string& wkt2,
    bool expectedContains)
{
    XYMultiPolygon geom1;
    XYLineString geom2;

    std::stringstream ss;
    ss << std::setprecision(PRECISION);
    ss << "test_contains" << std::endl;

    try {
        geom1 = geolib::fromWkt<XYMultiPolygon>(wkt1);
    } catch (const geolib::Exception& e) {
        ss << "Error parsing wkt string: " << wkt1 << " error: " << e.what() << std::endl;
        throw std::runtime_error(ss.str());
    }

    geolib::correct(geom1);
    ss << geolib::toWkt(geom1, PRECISION) << std::endl;

    try {
        geom2 = geolib::fromWkt<XYLineString>(wkt2);
    } catch (const geolib::Exception& e) {
        ss << "Error parsing wkt string: " << wkt2 << " error: " << e.what() << std::endl;
        throw std::runtime_error(ss.str());
    }

    ss << geolib::toWkt(geom2, PRECISION) << std::endl;

    auto contains = geolib::contains(geom1, geom2);

    if (contains != expectedContains) {
        ss << "result:   " << contains << std::endl;
        ss << "expected: " << expectedContains << std::endl;
        throw std::runtime_error(ss.str());
    }

    std::cout << ss.str() << std::endl;
}

} // namespace

void test_contains_1()
{
    test_contains(
        "MULTIPOLYGON(((1 0, 3 0, 3 3, 0 3, 0 1, 1 1, 1 0), (1 1, 1 2, 2 2, 2 1, 1 1)))",
        "LINESTRING(0.5 1.5, 0.5 2.5)",
        true);
}

void test_contains_2()
{
    test_contains(
        "MULTIPOLYGON(((1 0, 3 0, 3 3, 0 3, 0 1, 1 1, 1 0), (1 1, 1 2, 2 2, 2 1, 1 1)))",
        "LINESTRING(0.5 2, 2.5 2)",
        true);
}

void test_contains_3()
{
    test_contains(
        "MULTIPOLYGON(((1 0, 3 0, 3 3, 0 3, 0 1, 1 1, 1 0), (1 1, 1 2, 2 2, 2 1, 1 1)))",
        "LINESTRING(0 1, 2.001 3)",
        false);
}

void test_contains_4()
{
    test_contains(
        "MULTIPOLYGON(((0 0, 3 0, 3 2, 2 2, 2 1, 1 1, 1 2, 0 2, 0 0)))",
        "LINESTRING(0 1, 3 1)",
        true);
}

void test_contains_5()
{
    test_contains(
        "MULTIPOLYGON(((0 0, 3 0, 3 2, 2 2, 2 1, 1 1, 1 2, 0 2, 0 0)))",
        "LINESTRING(0 2, 3 2)",
        false);
}

void test_contains_6()
{
    test_contains(
        "MULTIPOLYGON(((0 0, 3 0, 3 2, 2 2, 2 1, 1 1, 1 2, 0 2, 0 0)))",
        "LINESTRING(0 0, 1 0, 2 0, 3 0, 3 2, 2 2, 2 1, 1 1, 1 2, 0 2)",
        true);
}

void test_contains_7()
{
    test_contains(
        "MULTIPOLYGON(((0 0, 2 2, 3 4, 0 0)))",
        "LINESTRING(0 0, 2 2)",
        true);
}
