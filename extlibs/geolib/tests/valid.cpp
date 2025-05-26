#include "point.h"
#include "wkt_examples.h"

namespace {

const auto PRECISION = 16;

template <typename Geometry>
void test_valid(
    const std::string& wkt,
    bool expectedValid)
{
    Geometry geom;

    std::stringstream ss;
    ss << std::setprecision(PRECISION);
    ss << "test_valid: " << wkt << std::endl;

    try {
        geom = geolib::fromWkt<Geometry>(wkt);
    } catch (const geolib::Exception& e) {
        ss << "Error parsing wkt string: " << wkt << " error: " << e.what() << std::endl;
        throw std::runtime_error(ss.str());
    }

    geolib::correct(geom);

    std::string message;
    bool valid = geolib::isValid(geom, message);

    if (!valid) {
        ss << message;
    }

    if (valid != expectedValid) {
        ss << "result:   " << valid << std::endl;
        ss << "expected: " << expectedValid << std::endl;
        throw std::runtime_error(ss.str());
    }

    std::cout << ss.str() << std::endl;
}

} // namespace

void test_valid_1()
{
    // Valid ring
    test_valid<XYRing>(
        "LINEARRING(0 4, 2 2, 0 0, 5 0, 5 4, 0 4)",
        true);
}

void test_valid_2()
{
    // Invalid: ring self-intersection at point (1,1)
    test_valid<XYRing>(
        "LINEARRING(0 0, 2 2, 0 2, 2 0, 0 0)",
        false);
}

void test_valid_3()
{
    // Invalid: ring self-touching at point (1,1)
    test_valid<XYPolygon>(
        "POLYGON((0 0, 2 0, 1 1, 2 2, 0 2, 1 1, 0 0))",
        false);
}

void test_valid_4()
{
    // Valid multipolygon: two separate touching rings
    test_valid<XYMultiPolygon>(
        "MULTIPOLYGON(((0 0, 2 0, 1 1, 0 0)), ((2 2, 1 1, 0 2, 2 2)))",
        true);
}

void test_valid_5()
{
    // Self-intersection
    test_valid<XYPolygon>(
        "POLYGON((1 0, 3 0, 3 3, 0 3, 0 1, 1 1, 1 2, 2 2, 2 1, 1 1, 1 0))",
        false);
}

void test_valid_6()
{
    // Valid polygon, though inner ring touches outer ring
    test_valid<XYPolygon>(
        "POLYGON((1 0, 3 0, 3 3, 0 3, 0 1, 1 1, 1 0), (1 1, 1 2, 2 2, 2 1, 1 1))",
        true);
}

void test_valid_7()
{
    // Invalid: inner ring has two common points with outer ring
    test_valid<XYPolygon>(
        "POLYGON((0 0, 2 2, 4 0, 2 4, 0 0), (1 1, 2 3, 3 1, 1 1))",
        false);
}

void test_valid_8()
{
    // TODO: in fact this polygon should not considered valid because
    // it is disconnected but this check is currently skipped
    test_valid<XYPolygon>(
        "POLYGON((0 0, 4 0, 4 2, 4 4, 0 4, 0 2, 0 0), (0 2, 1 3, 2 2, 1 1, 0 2), (2 2, 3 3, 4 2, 3 1, 2 2))",
        true);
}

void test_valid_9()
{
    // Valid multipolygon though parts touches in 3 points
    test_valid<XYMultiPolygon>(
        "MULTIPOLYGON(((0 0, 4 0, 4 2, 3 1, 2 2, 1 1, 0 2, 0 0)), ((0 2, 1 3, 2 2, 3 3, 4 2, 4 4, 0 4, 0 2)))",
        true);
}

void test_valid_10()
{
    // Invalid multipolygon: parts do intersect
    test_valid<XYMultiPolygon>(
        "MULTIPOLYGON(((0 0, 4 0, 4 2, 3 3, 2 2, 1 3, 0 2, 0 0)), ((0 2, 1 1, 2 2, 3 1, 4 2, 4 4, 0 4, 0 2)))",
        false);
}

void test_valid_11()
{
    // Invalid multipolygon: parts do intersect
    test_valid<XYMultiPolygon>(
        "MULTIPOLYGON(((1 0, 3 0, 3 1, 2 2, 3 3, 3 4, 1 4, 1 3, 0 2, 1 1, 1 0)), ((1 1, 3 1, 3 3, 1 3, 1 1)))",
        false);
}

void test_valid_12()
{
    // Invalid multipolygon: parts have common part of border
    test_valid<XYMultiPolygon>(
        "MULTIPOLYGON(((0 2, 2 0, 4 2, 2 4, 0 2)), ((1 1, -1 3, -3 1, -1 -1, 1 1)))",
        false);
}

void test_valid_13()
{
    // Invalid multipolygon: parts do intersect
    test_valid<XYGeometry>(
        "MULTIPOLYGON("
            "((0 0, 4 0, 4 4, 0 4, 0 0), (1 1, 3 1, 3 3, 1 3, 1 1)), "
            "((2 0, 4 2, 2 4, 0 2, 2 0), (1 2, 2 3, 3 2, 2 1, 1 2)))",
        false);
}

void test_valid_14()
{
    // Valid multipolygon: parts do not intersect though have a common border point: (2, 4)
    test_valid<XYGeometry>(
        "MULTIPOLYGON("
            "((0 0, 4 0, 4 4, 0 4, 0 0), (1 1, 3 1, 3 3, 1 3, 1 1)), "
            "((2 4, 4 6, 2 8, 0 6, 2 4), (1 6, 2 7, 3 6, 2 5, 1 6)))",
        true);
}
