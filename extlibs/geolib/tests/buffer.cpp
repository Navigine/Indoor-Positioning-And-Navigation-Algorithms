#include "point.h"
#include "wkt_examples.h"

namespace {

const auto PRECISION = 4;
const auto EPSILON = 0.001;

void test_buffer(
    const std::string& name,
    const std::string& wkt,
    const std::string& expectedWkt,
    const double radius,
    const unsigned int numPoints)
{
    XYGeometry geom;

    std::stringstream ss;
    ss << std::setprecision(PRECISION);
    ss << name << ": radius=" << radius << ", numPoints=" << numPoints << std::endl;

    try {
        geom = geolib::fromWkt<XYGeometry>(wkt);
    } catch (const geolib::Exception& e) {
        ss << "Error parsing wkt string: " << wkt << " error: " << e.what() << std::endl;
        throw std::runtime_error(ss.str());
    }

    geolib::correct(geom);

    ss << geolib::toWkt(geom, PRECISION) << std::endl;

    auto result = geolib::simplified(
        geolib::buffer(geom, radius, numPoints),
        0.0);

    geolib::sort(result);

    ss << geolib::toWkt(result, PRECISION) << std::endl;

    std::string message;
    if (!geolib::isValid(result, message)) {
        ss << "Resulting geom is not valid:" << std::endl;
        ss << message << std::endl;
        throw std::runtime_error(ss.str());
    }

    if (!expectedWkt.empty()) {
        XYMultiPolygon expectedGeom;

        try {
            expectedGeom = geolib::fromWkt<XYMultiPolygon>(expectedWkt);
        } catch (const geolib::Exception& e) {
            ss << "Error parsing wkt string: " << expectedWkt << " error: " << e.what() << std::endl;
            throw std::runtime_error(ss.str());
        }

        geolib::correct(expectedGeom);

        if (!geolib::equal(result, expectedGeom, EPSILON)) {
            ss << "Resulting geom : " << geolib::toWkt(result, PRECISION) << std::endl;
            ss << "Expected geom  : " << geolib::toWkt(expectedGeom, PRECISION) << std::endl;
            throw std::runtime_error(ss.str());
        }
    }

    std::cout << ss.str() << std::endl;
}

void test_buffer_invalid(
    const std::string& name,
    const std::string& wkt)
{
    XYGeometry geom;

    std::stringstream ss;
    ss << std::setprecision(PRECISION);

    try {
        geom = geolib::fromWkt<XYGeometry>(wkt);
    } catch (const geolib::Exception& e) {
        ss << name << ": error parsing wkt string: " << wkt << " error: " << e.what() << std::endl;
        throw std::runtime_error(ss.str());
    }

    geolib::correct(geom);

    ss << geolib::toWkt(geom, PRECISION) << std::endl;

    try {
        auto result = geolib::buffer(geom, 1.0, 18);
    } catch (const geolib::Exception& e) {
        std::cout << ss.str() << std::endl;
        return;
    }

    ss << name << ": buffer should fail" << std::endl;
    throw std::runtime_error(ss.str());
}

} // namespace

void test_buffer_1()
{
    test_buffer(__FUNCTION__,
        "MULTIPOLYGON(((0 0, 4 0, 4 4, 0 4, 0 0)))",
        "MULTIPOLYGON(((-0.5 0, 0 -0.5, 4 -0.5, 4.5 0, 4.5 4, 4 4.5, 0 4.5, -0.5 4, -0.5 0)))",
        0.5, 2);
}

void test_buffer_2()
{
    test_buffer(__FUNCTION__,
        "MULTIPOLYGON(((0 0, 4 0, 4 4, 0 4, 0 0)))",
        "MULTIPOLYGON(((-0.5 0, -0.353553390593 -0.353553390593, -0 -0.5, 4 -0.5, 4.35355339059 -0.353553390593, 4.5 0, 4.5 4, 4.35355339059 4.35355339059, 4 4.5, 0 4.5, -0.353553390593 4.35355339059, -0.5 4, -0.5 0)))",
        0.5, 4);
}

void test_buffer_3()
{
    test_buffer(__FUNCTION__,
        "MULTIPOLYGON(((0 0, 4 0, 4 4, 0 4, 0 0)))",
        "MULTIPOLYGON(((-0.5 -0, -0.492403876506 -0.086824088833, -0.469846310393 -0.171010071663, -0.433012701892 -0.25, -0.383022221559 -0.321393804843, -0.321393804843 -0.383022221559, -0.25 -0.433012701892, -0.171010071663 -0.469846310393, -0.086824088833 -0.492403876506, 0 -0.5, 4 -0.5, 4.08682408883 -0.492403876506, 4.17101007166 -0.469846310393, 4.25 -0.433012701892, 4.32139380484 -0.383022221559, 4.38302222156 -0.321393804843, 4.43301270189 -0.25, 4.46984631039 -0.171010071663, 4.49240387651 -0.086824088833, 4.5 0, 4.5 4, 4.49240387651 4.08682408883, 4.46984631039 4.17101007166, 4.43301270189 4.25, 4.38302222156 4.32139380484, 4.32139380484 4.38302222156, 4.25 4.43301270189, 4.17101007166 4.46984631039, 4.08682408883 4.49240387651, 4 4.5, 0 4.5, -0.086824088833 4.49240387651, -0.171010071663 4.46984631039, -0.25 4.43301270189, -0.321393804843 4.38302222156, -0.383022221559 4.32139380484, -0.433012701892 4.25, -0.469846310393 4.17101007166, -0.492403876506 4.08682408883, -0.5 4, -0.5 -0)))",
        0.5, 18);
}

void test_buffer_4()
{
    test_buffer(__FUNCTION__,
        "MULTIPOLYGON(((1 5, 0 4, 2 0, 4 0, 5 4, 4 3, 4 5, 3 4, 1 5)))",
        "MULTIPOLYGON(((-0.5 4, 1.5 0, 2 -0.5, 4 -0.5, 4.5 0, 5.5 4, 5 4.5, 4.5 4, 4.5 5, 4 5.5, 3 4.5, 1 5.5, -0.5 4)))",
        0.5, 2);
}

void test_buffer_5()
{
    test_buffer(__FUNCTION__,
        "MULTIPOLYGON(((1 5, 0 4, 2 0, 4 0, 5 4, 4 3, 4 5, 3 4, 1 5)))",
        "MULTIPOLYGON(((-0.5 4, -0.353553390593 3.64644660941, 1.64644660941 -0.353553390593, 2 -0.5, 4 -0.5, 4.35355339059 -0.353553390593, 4.5 0, 5.5 4, 5.35355339059 4.35355339059, 5 4.5, 4.64644660941 4.35355339059, 4.5 4.20710678119, 4.5 5, 4.35355339059 5.35355339059, 4 5.5, 3.64644660941 5.35355339059, 2.8821488698 4.58925565099, 1.35355339059 5.35355339059, 1 5.5, 0.646446609407 5.35355339059, -0.353553390593 4.35355339059, -0.5 4)))",
        0.5, 4);
}

void test_buffer_6()
{
    test_buffer(__FUNCTION__,
        "MULTIPOLYGON(((1 5, 0 4, 2 0, 4 0, 5 4, 4 3, 4 5, 3 4, 1 5)))",
        "MULTIPOLYGON(((-0.5 4, -0.4924 3.913, -0.4698 3.829, -0.433 3.75, 1.567 -0.25, 1.617 -0.3214, 1.679 -0.383, 1.75 -0.433, 1.829 -0.4698, 1.913 -0.4924, 2 -0.5, 4 -0.5, 4.087 -0.4924, 4.171 -0.4698, 4.25 -0.433, 4.321 -0.383, 4.383 -0.3214, 4.433 -0.25, 4.47 -0.171, 4.492 -0.08682, 5.492 3.913, 5.5 4, 5.492 4.087, 5.47 4.171, 5.433 4.25, 5.383 4.321, 5.321 4.383, 5.25 4.433, 5.171 4.47, 5.087 4.492, 5 4.5, 4.913 4.492, 4.829 4.47, 4.75 4.433, 4.679 4.383, 4.5 4.204, 4.5 5, 4.492 5.087, 4.47 5.171, 4.433 5.25, 4.383 5.321, 4.321 5.383, 4.25 5.433, 4.171 5.47, 4.087 5.492, 4 5.5, 3.913 5.492, 3.829 5.47, 3.75 5.433, 3.679 5.383, 2.902 4.607, 1.25 5.433, 1.171 5.47, 1.087 5.492, 1 5.5, 0.9132 5.492, 0.829 5.47, 0.75 5.433, 0.6786 5.383, -0.383 4.321, -0.433 4.25, -0.4698 4.171, -0.4924 4.087, -0.5 4)))",
        0.5, 18);
}

void test_buffer_7()
{
    test_buffer(__FUNCTION__,
        "POINT(0 0)",
        "MULTIPOLYGON(((-1 0, -0.866 -0.5, -0.5 -0.866, 0 -1, 0.5 -0.866, 0.866 -0.5, 1 0, 0.866 0.5, 0.5 0.866, 0 1, -0.5 0.866, -0.866 0.5, -1 0)))",
        1, 6);
}

void test_buffer_8()
{
    test_buffer(__FUNCTION__,
        "SEGMENT(0 0, 4 4)",
        "MULTIPOLYGON(((-1 0, -0.866 -0.5, -0.5 -0.866, 0 -1, 0.5 -0.866, 4.866 3.5, 5 4, 4.866 4.5, 4.5 4.866, 4 5, 3.5 4.866, -0.866 0.5, -1 0)))",
        1, 6);
}

void test_buffer_9()
{
    test_buffer(__FUNCTION__,
        "LINESTRING(0 0, 4 0, 4 4, 0 4)",
        "MULTIPOLYGON(((-1 0, -0.866 -0.5, -0.5 -0.866, 0 -1, 4 -1, 4.5 -0.866, 4.866 -0.5, 5 0, 5 4, 4.866 4.5, 4.5 4.866, 4 5, 0 5, -0.5 4.866, -0.866 4.5, -1 4, -0.866 3.5, -0.5 3.134, 0 3, 3 3, 3 1, 0 1, -0.5 0.866, -0.866 0.5, -1 0)))",
        1, 6);
}

void test_buffer_10()
{
    test_buffer(__FUNCTION__,
        "LINESTRING(0 0, 8 8, 0 8, 8 0)",
        "MULTIPOLYGON(((-1 0, -0.866 -0.5, -0.5 -0.866, 0 -1, 0.5 -0.866, 4 2.634, 7.5 -0.866, 8 -1, 8.5 -0.866, 8.866 -0.5, 9 0, 8.866 0.5, 5.366 4, 8.866 7.5, 9 8, 8.866 8.5, 8.5 8.866, 8 9, 0 9, -0.5 8.866, -0.866 8.5, -1 8, -0.866 7.5, 2.634 4, -0.866 0.5, -1 0), (2.366 7, 5.634 7, 4 5.366, 2.366 7)))",
        1, 6);
}

void test_buffer_invalid_1()
{
    test_buffer_invalid(__FUNCTION__,
        "POLYGON((0 0, 8 8, 0 8, 8 0, 0 0))");
}

void test_buffer_metropolis_1()
{
    test_buffer(__FUNCTION__, WKT_METROPOLIS_1, "", 1.0, 18);
}

void test_buffer_metropolis_2()
{
    test_buffer(__FUNCTION__, WKT_METROPOLIS_2, "", 1.0, 18);
}

void test_buffer_metropolis_3()
{
    test_buffer(__FUNCTION__, WKT_METROPOLIS_3, "", 1.0, 18);
}

void test_buffer_afimoll_1()
{
    test_buffer(__FUNCTION__, WKT_AFIMOLL_1, "", 1.0, 18);
}

void test_buffer_afimoll_2()
{
    test_buffer(__FUNCTION__, WKT_AFIMOLL_2, "", 1.0, 18);
}

void test_buffer_afimoll_3()
{
    test_buffer(__FUNCTION__, WKT_AFIMOLL_3, "", 1.0, 18);
}

void test_buffer_afimoll_4()
{
    test_buffer(__FUNCTION__, WKT_AFIMOLL_4, "", 1.0, 18);
}

void test_buffer_afimoll_5()
{
    test_buffer(__FUNCTION__, WKT_AFIMOLL_5, "", 1.0, 18);
}

void test_buffer_evropeiskyi_U1()
{
    test_buffer(__FUNCTION__, WKT_EVROPEISKYI_U1, "", 1.0, 18);
}

void test_buffer_evropeiskyi_1()
{
    test_buffer(__FUNCTION__, WKT_EVROPEISKYI_1, "", 1.0, 18);
}

void test_buffer_evropeiskyi_2()
{
    test_buffer(__FUNCTION__, WKT_EVROPEISKYI_2, "", 1.0, 18);
}

void test_buffer_evropeiskyi_3()
{
    test_buffer(__FUNCTION__, WKT_EVROPEISKYI_3, "", 1.0, 18);
}

void test_buffer_evropeiskyi_4()
{
    test_buffer(__FUNCTION__, WKT_EVROPEISKYI_4, "", 1.0, 18);
}

void test_buffer_dubai_mall_1()
{
    test_buffer(__FUNCTION__, WKT_DUBAI_MALL_1, "", 1.0, 18);
}

void test_buffer_dubai_mall_2()
{
    test_buffer(__FUNCTION__, WKT_DUBAI_MALL_2, "", 1.0, 18);
}

void test_buffer_dubai_mall_3()
{
    test_buffer(__FUNCTION__, WKT_DUBAI_MALL_3, "", 1.0, 18);
}

void test_buffer_dubai_mall_4()
{
    test_buffer(__FUNCTION__, WKT_DUBAI_MALL_4, "", 1.0, 18);
}

void test_buffer_dubai_mall_G()
{
    test_buffer(__FUNCTION__, WKT_DUBAI_MALL_G, "", 1.0, 18);
}

void test_buffer_dubai_mall_L()
{
    test_buffer(__FUNCTION__, WKT_DUBAI_MALL_L, "", 1.0, 18);
}

void test_buffer_dubai_mall_LG()
{
    test_buffer(__FUNCTION__, WKT_DUBAI_MALL_LG, "", 1.0, 18);
}
