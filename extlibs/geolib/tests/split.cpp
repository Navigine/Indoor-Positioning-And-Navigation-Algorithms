#include "point.h"
#include "wkt_examples.h"

namespace {

const auto PRECISION = 16;
const auto EPSILON = 1e-8;

void test_split(const std::string& wkt, const std::string& split)
{
    XYMultiPolygon geom;

    std::stringstream ss;
    ss << std::setprecision(PRECISION);
    ss << "test_split: split=" << split << std::endl;

    try {
        geom = geolib::fromWkt<XYMultiPolygon>(wkt);
    } catch (const geolib::Exception& e) {
        ss << "Error parsing wkt string: " << wkt << " error: " << e.what() << std::endl;
        throw std::runtime_error(ss.str());
    }

    geolib::correct(geom);

    const auto area = geolib::area(geom);

    ss << geolib::toWkt(geom, PRECISION) << std::endl;

    auto box = geolib::boundingBox(geom);
    box.minCorner.x -= 1.0;
    box.minCorner.y -= 1.0;
    box.maxCorner.x += 1.0;
    box.maxCorner.y += 1.0;

    auto boxMin = box;
    auto boxMax = box;

    if (split == "X") {
        const auto xmid = (box.minCorner.x + box.maxCorner.x) / 2;
        boxMin.maxCorner.x = xmid;
        boxMax.minCorner.x = xmid;
    } else if (split == "Y") {
        const auto ymid = (box.minCorner.y + box.maxCorner.y) / 2;
        boxMin.maxCorner.y = ymid;
        boxMax.minCorner.y = ymid;
    } else {
        throw std::runtime_error(ss.str());
    }

    const auto geomMin = geolib::intersection(geom, boxMin);
    const auto geomMax = geolib::intersection(geom, boxMax);

    const auto areaMin = geolib::area(geomMin);
    const auto areaMax = geolib::area(geomMax);
    const auto areaSum = areaMin + areaMax;

    const auto relativeError = std::fabs(areaSum / area - 1.0);

    if (relativeError > EPSILON) {
        ss << "boxMin: " << geolib::toWkt(boxMin, PRECISION) << std::endl;
        ss << "boxMax: " << geolib::toWkt(boxMax, PRECISION) << std::endl;
        ss << "areaMin: " << areaMin << std::endl;
        ss << "areaMax: " << areaMax << std::endl;
        ss << "original area: " << area << std::endl;
        ss << "area of parts: " << areaSum << std::endl;
        ss << "relative error: " << relativeError << std::endl;
        throw std::runtime_error(ss.str());
    }

    std::cout << ss.str() << std::endl;
}

} // namespace

void test_split_metropolis_1()
{
    test_split(WKT_METROPOLIS_1, "X");
    test_split(WKT_METROPOLIS_1, "Y");
}

void test_split_metropolis_2()
{
    test_split(WKT_METROPOLIS_2, "X");
    test_split(WKT_METROPOLIS_2, "Y");
}

void test_split_metropolis_3()
{
    test_split(WKT_METROPOLIS_3, "X");
    test_split(WKT_METROPOLIS_3, "Y");
}

void test_split_afimoll_1()
{
    test_split(WKT_AFIMOLL_1, "X");
    test_split(WKT_AFIMOLL_1, "Y");
}

void test_split_afimoll_2()
{
    test_split(WKT_AFIMOLL_2, "X");
    test_split(WKT_AFIMOLL_2, "Y");
}

void test_split_afimoll_3()
{
    test_split(WKT_AFIMOLL_3, "X");
    test_split(WKT_AFIMOLL_3, "Y");
}

void test_split_afimoll_4()
{
    test_split(WKT_AFIMOLL_4, "X");
    test_split(WKT_AFIMOLL_4, "Y");
}

void test_split_afimoll_5()
{
    test_split(WKT_AFIMOLL_5, "X");
    test_split(WKT_AFIMOLL_5, "Y");
}

void test_split_evropeiskyi_U1()
{
    test_split(WKT_EVROPEISKYI_U1, "X");
    test_split(WKT_EVROPEISKYI_U1, "Y");
}

void test_split_evropeiskyi_1()
{
    test_split(WKT_EVROPEISKYI_1, "X");
    test_split(WKT_EVROPEISKYI_1, "Y");
}

void test_split_evropeiskyi_2()
{
    test_split(WKT_EVROPEISKYI_2, "X");
    test_split(WKT_EVROPEISKYI_2, "Y");
}

void test_split_evropeiskyi_3()
{
    test_split(WKT_EVROPEISKYI_3, "X");
    test_split(WKT_EVROPEISKYI_3, "Y");
}

void test_split_evropeiskyi_4()
{
    test_split(WKT_EVROPEISKYI_4, "X");
    test_split(WKT_EVROPEISKYI_4, "Y");
}

void test_split_dubai_mall_1()
{
    test_split(WKT_DUBAI_MALL_1, "X");
    test_split(WKT_DUBAI_MALL_1, "Y");
}

void test_split_dubai_mall_2()
{
    test_split(WKT_DUBAI_MALL_2, "X");
    test_split(WKT_DUBAI_MALL_2, "Y");
}

void test_split_dubai_mall_3()
{
    test_split(WKT_DUBAI_MALL_3, "X");
    test_split(WKT_DUBAI_MALL_3, "Y");
}

void test_split_dubai_mall_4()
{
    test_split(WKT_DUBAI_MALL_4, "X");
    test_split(WKT_DUBAI_MALL_4, "Y");
}

void test_split_dubai_mall_G()
{
    test_split(WKT_DUBAI_MALL_G, "X");
    test_split(WKT_DUBAI_MALL_G, "Y");
}

void test_split_dubai_mall_L()
{
    test_split(WKT_DUBAI_MALL_L, "X");
    test_split(WKT_DUBAI_MALL_L, "Y");
}

void test_split_dubai_mall_LG()
{
    test_split(WKT_DUBAI_MALL_LG, "X");
    test_split(WKT_DUBAI_MALL_LG, "Y");
}
