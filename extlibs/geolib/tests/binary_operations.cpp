#include "point.h"
#include "wkt_examples.h"

namespace {

const auto PRECISION = 16;
const auto EPSILON = 1e-8;

void test_binary_operation(
    geolib::detail::BinaryOperationType op,
    const std::string& wkt1,
    const std::string& wkt2,
    const std::string& wktExpected)
{
    XYGeometry geom1, geom2;
    XYMultiPolygon geomExpected;

    std::stringstream ss;
    ss << std::setprecision(PRECISION);

    ss << "test_binary_operation " << static_cast<int>(op) << std::endl;

    try {
        geom1 = geolib::fromWkt<XYGeometry>(wkt1);
    } catch (const geolib::Exception& e) {
        ss << "Error parsing wkt string: " << wkt1 << " error: " << e.what() << std::endl;
        throw std::runtime_error(ss.str());
    }

    try {
        geom2 = geolib::fromWkt<XYGeometry>(wkt2);
    } catch (const geolib::Exception& e) {
        ss << "Error parsing wkt string: " << wkt2 << " error: " << e.what() << std::endl;
        throw std::runtime_error(ss.str());
    }

    try {
        geomExpected = geolib::fromWkt<XYMultiPolygon>(wktExpected);
    } catch (const geolib::Exception& e) {
        ss << "Error parsing wkt string: " << wktExpected << " error: " << e.what() << std::endl;
        throw std::runtime_error(ss.str());
    }

    geolib::correct(geom1);
    geolib::correct(geom2);
    geolib::correct(geomExpected);

    ss << "geom1: " << geolib::toWkt(geom1, PRECISION) << std::endl;
    ss << "geom2: " << geolib::toWkt(geom2, PRECISION) << std::endl;

    auto result = geolib::simplified(
        geolib::detail::binaryOperation(op, geom1, geom2),
        EPSILON);

    geolib::sort(result);

    ss << "result: " << geolib::toWkt(result, PRECISION) << std::endl << std::endl;

    std::string message;
    if (!geolib::isValid(result, message)) {
        ss << "Resulting geom is not valid:" << std::endl;
        ss << message << std::endl;
        throw std::runtime_error(ss.str());
    }

    if (!geolib::equal(result, geomExpected, EPSILON)) {
        ss << "expect: " << geolib::toWkt(geomExpected, PRECISION) << std::endl;
        throw std::runtime_error(ss.str());
    }

    std::cout << ss.str() << std::endl;
}

void test_self_binary_operation(geolib::detail::BinaryOperationType op, const std::string& wkt)
{
    XYGeometry geom1;

    std::stringstream ss;
    ss << std::setprecision(PRECISION);

    ss << "test_self_binary_operation: " << static_cast<int>(op) << std::endl;

    try {
        geom1 = geolib::fromWkt<XYGeometry>(wkt);
    } catch (const geolib::Exception& e) {
        ss << "Error parsing wkt string: " << wkt << " error: " << e.what() << std::endl;
        throw std::runtime_error(ss.str());
    }

    auto geom2 = geom1;

    // Translating geometry
    geolib::detail::iterateVerticesMutable(geom2, false,
        [&](XYPoint& point, size_t, size_t) {
            point.x += 1.0;
            return true;
        });

    auto result = geolib::detail::binaryOperation(op, geom1, geom2);

    std::string message;
    if (!geolib::isValid(result, message)) {
        ss << "Resulting geom is not valid:" << std::endl;
        ss << message << std::endl;
        throw std::runtime_error(ss.str());
    }
}

} // namespace

void test_union_1()
{
    test_binary_operation(geolib::detail::BinaryOperationType::UNION,
        "MULTIPOLYGON(((0 4, 2 2, 0 0, 5 0, 5 4, 0 4)))",
        "MULTIPOLYGON(((1 5, 0 4, 2 0, 4 0, 5 4, 4 3, 4 5, 3 4, 1 5)))",
        "MULTIPOLYGON(((0 0, 5 0, 5 4, 4 4, 4 5, 3 4, 1 5, 0 4, 1.333333333333 1.333333333333, 0 0)))");
}

void test_union_2()
{
    test_binary_operation(geolib::detail::BinaryOperationType::UNION,
        "MULTIPOLYGON(((0 2, 10 2, 10 6, 0 6, 0 2), (2 4, 3 4, 3 3, 2 3, 2 4)))",
        "MULTIPOLYGON(((1 0, 9 0, 9 5, 6 5, 6 1, 4 1, 4 5, 1 5, 1 0), (8 3, 7 3, 7 4, 8 4, 8 3)))",
        "MULTIPOLYGON(((0 2, 1 2, 1 0, 9 0, 9 2, 10 2, 10 6, 0 6, 0 2), (4 1, 4 2, 6 2, 6 1, 4 1)))");
}

void test_union_3()
{
    test_binary_operation(geolib::detail::BinaryOperationType::UNION,
        "MULTIPOLYGON(((0 0, 4 0, 4 1, 1 1, 1 2, 4 2, 4 3, 0 3, 0 0)))",
        "MULTIPOLYGON(((3 0, 3 2, 1 2, 3 0)))",
        "MULTIPOLYGON(((0 0, 4 0, 4 1, 3 1, 3 2, 4 2, 4 3, 0 3, 0 0), (1 1, 1 2, 2 1, 1 1)))");
}

void test_union_4()
{
    test_binary_operation(geolib::detail::BinaryOperationType::UNION,
        "MULTIPOLYGON(((-1 -1, 1.5 -1, 1.5 4, -1 4, -1 -1)))",
        "MULTIPOLYGON(((1 0, 3 0, 3 3, 0 3, 0 1, 1 1, 1 0), (1 1, 1 2, 2 2, 2 1, 1 1)))",
        "MULTIPOLYGON(((-1 -1, 1.5 -1, 1.5 0, 3 0, 3 3, 1.5 3, 1.5 4, -1 4, -1 -1), (1.5 1, 1.5 2, 2 2, 2 1, 1.5 1)))");
}

void test_union_5()
{
    test_binary_operation(geolib::detail::BinaryOperationType::UNION,
        "MULTIPOLYGON(((-2 -2, 2 -2, 0 0, -2 -2)), ((0 0, 2 2, -2 2, 0 0)))",
        "MULTIPOLYGON(((0 0, 0.5 -1.5, 2 -0.5, 0 0)), ((-0.5 1.5, -2 0.5, 0 0, -0.5 1.5)))",
        "MULTIPOLYGON(((-2 -2, 2 -2, 1.1 -1.1, 2 -0.5, 0 0, -2 -2)), ((-2 0.5, 0 0, 2 2, -2 2, -1.1 1.1, -2 0.5)))");
}

void test_union_6()
{
    test_binary_operation(geolib::detail::BinaryOperationType::UNION,
        "MULTIPOLYGON(((-0.5 0, 0 -0.5, 4 -0.5, 4.5 0, 4.5 4, 4 4.5, 3.5 4, 0 4, 0 0.5, -0.5 0)))",
        "MULTIPOLYGON(((4 3.5, 4.5 4, 4 4.5, 0 4.5, -0.5 4, 0 3.5, 4 3.5)))",
        "MULTIPOLYGON(((-0.5 0, 0 -0.5, 4 -0.5, 4.5 0, 4.5 4, 4 4.5, 0 4.5, -0.5 4, 0 3.5, 0 0.5, -0.5 0)))");
}

void test_union_7()
{
    // Same as in test_union_1, but rotated by 45 degrees and mutiplied by 0.1
    // NB: coordinates could not be represented exactly by double.
    test_binary_operation(geolib::detail::BinaryOperationType::UNION,
        "MULTIPOLYGON(((0 0, 0.5 0.5, 0.1 0.9, -0.4 0.4, 0 0.4, 0 0)))",
        "MULTIPOLYGON(((-0.4 0.4, 0.2 0.2, 0.4 0.4, 0.1 0.9, 0.1 0.7, -0.1 0.9, -0.1 0.7, -0.4 0.6, -0.4 0.4)))",
        "MULTIPOLYGON(((-0.4 0.4, 0 0.266666666667, 0 0, 0.5 0.5, 0.1 0.9, 0 0.8, -0.1 0.9, -0.1 0.7, -0.4 0.6, -0.4 0.4)))");
}

void test_intersection_1()
{
    test_binary_operation(geolib::detail::BinaryOperationType::INTERSECTION,
        "MULTIPOLYGON(((0 4, 2 2, 0 0, 5 0, 5 4, 0 4)))",
        "MULTIPOLYGON(((1 5, 0 4, 2 0, 4 0, 5 4, 4 3, 4 5, 3 4, 1 5)))",
        "MULTIPOLYGON(((0 4, 2 2, 1.333333333333 1.333333333333, 2 0, 4 0, 5 4, 4 3, 4 4, 0 4)))");
}

void test_intersection_2()
{
    test_binary_operation(geolib::detail::BinaryOperationType::INTERSECTION,
        "MULTIPOLYGON(((0 2, 10 2, 10 6, 0 6, 0 2), (2 4, 3 4, 3 3, 2 3, 2 4)))",
        "MULTIPOLYGON(((1 0, 9 0, 9 5, 6 5, 6 1, 4 1, 4 5, 1 5, 1 0), (8 3, 7 3, 7 4, 8 4, 8 3)))",
        "MULTIPOLYGON(((1 2, 4 2, 4 5, 1 5, 1 2), (2 3, 2 4, 3 4, 3 3, 2 3)), ((6 2, 9 2, 9 5, 6 5, 6 2), (7 3, 7 4, 8 4, 8 3, 7 3)))");
}

void test_intersection_3()
{
    test_binary_operation(geolib::detail::BinaryOperationType::INTERSECTION,
        "MULTIPOLYGON(((0 0, 4 0, 4 1, 1 1, 1 2, 4 2, 4 3, 0 3, 0 0)))",
        "MULTIPOLYGON(((3 0, 3 2, 1 2, 3 0)))",
        "MULTIPOLYGON(((2 1, 3 0, 3 1, 2 1)))");
}

void test_intersection_4()
{
    test_binary_operation(geolib::detail::BinaryOperationType::INTERSECTION,
        "MULTIPOLYGON(((-1 -1, 1.5 -1, 1.5 4, -1 4, -1 -1)))",
        "MULTIPOLYGON(((1 0, 3 0, 3 3, 0 3, 0 1, 1 1, 1 0), (1 1, 1 2, 2 2, 2 1, 1 1)))",
        "MULTIPOLYGON(((0 1, 1 1, 1 2, 1.5 2, 1.5 3, 0 3, 0 1)), ((1 0, 1.5 0, 1.5 1, 1 1, 1 0)))");
}

void test_intersection_5()
{
    test_binary_operation(geolib::detail::BinaryOperationType::INTERSECTION,
        "MULTIPOLYGON(((-2 -2, 2 -2, 0 0, -2 -2)), ((0 0, 2 2, -2 2, 0 0)))",
        "MULTIPOLYGON(((0 0, 0.5 -1.5, 2 -0.5, 0 0)), ((-0.5 1.5, -2 0.5, 0 0, -0.5 1.5)))",
        "MULTIPOLYGON(((-1.1 1.1, 0 0, -0.5 1.5, -1.1 1.1)), ((0 0, 0.5 -1.5, 1.1 -1.1, 0 0)))");
}

void test_difference_1()
{
    test_binary_operation(geolib::detail::BinaryOperationType::DIFFERENCE,
        "MULTIPOLYGON(((0 4, 2 2, 0 0, 5 0, 5 4, 0 4)))",
        "MULTIPOLYGON(((1 5, 0 4, 2 0, 4 0, 5 4, 4 3, 4 5, 3 4, 1 5)))",
        "MULTIPOLYGON(((0 0, 2 0, 1.333333333333 1.333333333333, 0 0)), ((4 0, 5 0, 5 4, 4 0)), ((4 3, 5 4, 4 4, 4 3)))");
}

void test_difference_2()
{
    test_binary_operation(geolib::detail::BinaryOperationType::DIFFERENCE,
        "MULTIPOLYGON(((0 2, 10 2, 10 6, 0 6, 0 2), (2 4, 3 4, 3 3, 2 3, 2 4)))",
        "MULTIPOLYGON(((1 0, 9 0, 9 5, 6 5, 6 1, 4 1, 4 5, 1 5, 1 0), (8 3, 7 3, 7 4, 8 4, 8 3)))",
        "MULTIPOLYGON(((0 2, 1 2, 1 5, 4 5, 4 2, 6 2, 6 5, 9 5, 9 2, 10 2, 10 6, 0 6, 0 2)), ((7 3, 8 3, 8 4, 7 4, 7 3)))");
}

void test_difference_3()
{
    test_binary_operation(geolib::detail::BinaryOperationType::DIFFERENCE,
        "MULTIPOLYGON(((0 0, 4 0, 4 1, 1 1, 1 2, 4 2, 4 3, 0 3, 0 0)))",
        "MULTIPOLYGON(((3 0, 3 2, 1 2, 3 0)))",
        "MULTIPOLYGON(((0 0, 3 0, 2 1, 1 1, 1 2, 4 2, 4 3, 0 3, 0 0)), ((3 0, 4 0, 4 1, 3 1, 3 0)))");
}

void test_difference_4()
{
    test_binary_operation(geolib::detail::BinaryOperationType::DIFFERENCE,
        "MULTIPOLYGON(((-1 -1, 1.5 -1, 1.5 4, -1 4, -1 -1)))",
        "MULTIPOLYGON(((1 0, 3 0, 3 3, 0 3, 0 1, 1 1, 1 0), (1 1, 1 2, 2 2, 2 1, 1 1)))",
        "MULTIPOLYGON(((-1 -1, 1.5 -1, 1.5 0, 1 0, 1 1, 0 1, 0 3, 1.5 3, 1.5 4, -1 4, -1 -1)), ((1 1, 1.5 1, 1.5 2, 1 2, 1 1)))");
}

void test_difference_5()
{
    test_binary_operation(geolib::detail::BinaryOperationType::DIFFERENCE,
        "MULTIPOLYGON(((-2 -2, 2 -2, 0 0, -2 -2)), ((0 0, 2 2, -2 2, 0 0)))",
        "MULTIPOLYGON(((0 0, 0.5 -1.5, 2 -0.5, 0 0)), ((-0.5 1.5, -2 0.5, 0 0, -0.5 1.5)))",
        "MULTIPOLYGON(((-2 -2, 2 -2, 1.1 -1.1, 0.5 -1.5, 0 0, -2 -2)), ((-2 2, -1.1 1.1, -0.5 1.5, 0 0, 2 2, -2 2)))");
}

void test_difference_6()
{
    test_binary_operation(geolib::detail::BinaryOperationType::DIFFERENCE,
        "MULTIPOLYGON(((0 0, 4 0, 4 4, 0 4, 0 0)))",
        "MULTIPOLYGON(((0 2, 1 1, 2 2, 1 3, 0 2)), ((2 2, 3 1, 4 2, 3 3, 2 2)))",
        "MULTIPOLYGON(((0 0, 4 0, 4 4, 0 4, 0 0), (0 2, 1 3, 2 2, 1 1, 0 2), (2 2, 3 3, 4 2, 3 1, 2 2)))");
}

void test_difference_7a()
{
    // Same as in test_union_1, but rotated by 45 degrees and mutiplied by 0.1
    // NB: coordinates could not be represented exactly by double.
    test_binary_operation(geolib::detail::BinaryOperationType::DIFFERENCE,
        "MULTIPOLYGON(((0 0, 0.5 0.5, 0.1 0.9, -0.4 0.4, 0 0.4, 0 0)))",
        "MULTIPOLYGON(((-0.4 0.4, 0.2 0.2, 0.4 0.4, 0.1 0.9, 0.1 0.7, -0.1 0.9, -0.1 0.7, -0.4 0.6, -0.4 0.4)))",
        "MULTIPOLYGON(((0 0, 0.2 0.2, 0 0.266666666667, 0 0)), ((0 0.8, 0.1 0.7, 0.1 0.9, 0 0.8)), ((0.1 0.9, 0.4 0.4, 0.5 0.5, 0.1 0.9)))");
}

void test_difference_7b()
{
    // Same as in test_union_1, but rotated by 45 degrees and mutiplied by 0.1
    // NB: coordinates could not be represented exactly by double.
    test_binary_operation(geolib::detail::BinaryOperationType::DIFFERENCE,
        "MULTIPOLYGON(((-0.4 0.4, 0.2 0.2, 0.4 0.4, 0.1 0.9, 0.1 0.7, -0.1 0.9, -0.1 0.7, -0.4 0.6, -0.4 0.4)))",
        "MULTIPOLYGON(((0 0, 0.5 0.5, 0.1 0.9, -0.4 0.4, 0 0.4, 0 0)))",
        "MULTIPOLYGON(((-0.4 0.4, -0.1 0.7, -0.4 0.6, -0.4 0.4)), ((-0.4 0.4, 0 0.266666666667, 0 0.4, -0.4 0.4)), ((-0.1 0.7, 0 0.8, -0.1 0.9, -0.1 0.7)))");
}

void test_self_union_dubai_mall_1()
{
    test_self_binary_operation(geolib::detail::BinaryOperationType::UNION, WKT_DUBAI_MALL_1);
}

void test_self_intersection_dubai_mall_1()
{
    test_self_binary_operation(geolib::detail::BinaryOperationType::INTERSECTION, WKT_DUBAI_MALL_1);
}

void test_self_difference_dubai_mall_1()
{
    test_self_binary_operation(geolib::detail::BinaryOperationType::DIFFERENCE, WKT_DUBAI_MALL_1);
}
