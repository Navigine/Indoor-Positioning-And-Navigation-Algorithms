#include "point.h"
#include "wkt_examples.h"

#include <include/geolib/spatial_search_tree.h>

#include <chrono>
#include <random>
#include <sstream>

namespace {

const int PRECISION = 16;

// Function which generates uniform distribution on [min, max]
double random(double min, double max)
{
    static std::mt19937 rng;
    static std::uniform_real_distribution<double> distribution(0, 1);
    return min + (max - min) * distribution(rng);
}

std::vector<XYPoint> generateRandomPoints(const XYBox& box, size_t count)
{
    std::vector<XYPoint> points;
    for(size_t i = 0; i < count; ++i) {
        const auto x = random(box.minCorner.x, box.maxCorner.x);
        const auto y = random(box.minCorner.y, box.maxCorner.y);
        points.push_back(XYPoint{x, y});
    }

    return points;
}

std::vector<XYSegment> generateRandomSegments(const XYBox& box, size_t count)
{
    std::vector<XYSegment> segments;
    for(size_t i = 0; i < count; ++i) {
        const auto x1 = random(box.minCorner.x, box.maxCorner.x);
        const auto y1 = random(box.minCorner.y, box.maxCorner.y);

        // Segment length and angle
        const auto length = random(0, 10);
        const auto angle = random(-M_PI, M_PI);

        const auto x2 = x1 + length * std::cos(angle);
        const auto y2 = y1 + length * std::sin(angle);

        segments.push_back(XYSegment{
            XYPoint{x1, y1},
            XYPoint{x2, y2}});
    }

    return segments;
}

void test(const std::string& name, const std::string& wkt)
{
    const unsigned int numPoints = 100000;
    const unsigned int numSegments = 100000;

    XYMultiPolygon geom;

    std::stringstream ss;
    ss << std::setprecision(PRECISION);

    try {
        geom = geolib::fromWkt<XYMultiPolygon>(wkt);
    } catch (const geolib::Exception& e) {
        ss << "Error parsing wkt string: " << wkt << " error: " << e.what() << std::endl;
        throw std::runtime_error(ss.str());
    }

    geolib::correct(geom);
    ss << geolib::toWkt(geom, PRECISION) << std::endl;

    const auto box = geolib::boundingBox(geom);

    size_t errorCount = 0;

    std::vector<XYPoint> points = generateRandomPoints(box, numPoints);
    std::vector<bool> pointsResults(numPoints);
    std::vector<bool> pointsExpected(numPoints);

    std::vector<XYSegment> segments = generateRandomSegments(box, numSegments);
    std::vector<bool> segmentsResults(numSegments);
    std::vector<bool> segmentsExpected(numSegments);

    // Measuring processing time
    const auto t0 = std::chrono::steady_clock::now();

    geolib::SpatialSearchTree tree(geom);

    const auto t1 = std::chrono::steady_clock::now();

    for(size_t i = 0; i < numPoints; ++i) {
        pointsResults[i] = tree.containsPoint(points[i]);
    }

    const auto t2 = std::chrono::steady_clock::now();

    for(size_t i = 0; i < numPoints; ++i) {
        pointsExpected[i] = geolib::contains(geom, points[i]);
    }

    const auto t3 = std::chrono::steady_clock::now();

    for(size_t i = 0; i < numSegments; ++i) {
        segmentsResults[i] = tree.containsSegment(segments[i]);
    }

    const auto t4 = std::chrono::steady_clock::now();

    for(size_t i = 0; i < numSegments; ++i) {
        segmentsExpected[i] = geolib::contains(geom, segments[i]);
    }

    const auto t5 = std::chrono::steady_clock::now();

    // Checking points results
    for(size_t i = 0; i < numPoints; ++i) {
        const bool result = pointsResults[i];
        const bool expected = pointsExpected[i];
        if (result != expected) {
            ss << "Error " << ++errorCount << " in " << name << ":" << std::endl
               << "    point:    " << geolib::toWkt(points[i], PRECISION) << std::endl
               << "    result:   " << result << std::endl
               << "    expected: " << expected << std::endl << std::endl;
        }
    }

    // Checking segments results
    for(size_t i = 0; i < numSegments; ++i) {
        const bool result = segmentsResults[i];
        const bool expected = segmentsExpected[i];
        if (result != expected) {
            ss << "Error " << ++errorCount << " in " << name << ":" << std::endl
               << "    segment:  " << geolib::toWkt(segments[i], PRECISION) << std::endl
               << "    result:   " << result << std::endl
               << "    expected: " << expected << std::endl << std::endl;
        }
    }

    if (errorCount > 0) {
        throw std::runtime_error(ss.str());
    }

    tree.print(std::cout);

    std::cout << "containsPoint: building tree : " << std::chrono::duration<double>(t1 - t0).count() << " sec\n";
    std::cout << "containsPoint: tree result   : " << std::chrono::duration<double>(t2 - t1).count() << " sec\n";
    std::cout << "containsPoint: geolib result : " << std::chrono::duration<double>(t3 - t2).count() << " sec\n";

    std::cout << "containsSegment: tree result   : " << std::chrono::duration<double>(t4 - t3).count() << " sec\n";
    std::cout << "containsSegment: geolib result : " << std::chrono::duration<double>(t5 - t4).count() << " sec\n";
}

} // namespace

void test_sst_update()
{
    std::stringstream ss;

    geolib::SpatialSearchTree tree(XYMultiPolygon{});

    for(int i = 0; i < 1000; ++i) {
        XYPolygon poly;
        poly.outer.points.push_back(XYPoint{static_cast<double>(i), 0 + i/2.});
        poly.outer.points.push_back(XYPoint{static_cast<double>(i+1), 0 + i/2.});
        poly.outer.points.push_back(XYPoint{static_cast<double>(i+1), 1 + i/2.});
        poly.outer.points.push_back(XYPoint{static_cast<double>(i), 1 + i/2.});
        poly.outer.points.push_back(XYPoint{static_cast<double>(i), 0 + i/2.});

        XYMultiPolygon mp;
        mp.parts.push_back(poly);

        std::cout << "Add geometry: " << geolib::toWkt(poly) << "\n";

        tree.addGeometry(mp);
    }

    tree.print(std::cout);
}

void test_sst_squares()
{
    test(__FUNCTION__, "MULTIPOLYGON(((0 0, 1 0, 1 1, 0 1, 0 0)), ((1 0, 1 -1, 2 -1, 2 0, 1 0)))");
}

void test_sst_metropolis_1()
{
    test(__FUNCTION__, WKT_METROPOLIS_1);
}

void test_sst_metropolis_2()
{
    test(__FUNCTION__, WKT_METROPOLIS_2);
}

void test_sst_metropolis_3()
{
    test(__FUNCTION__, WKT_METROPOLIS_3);
}

void test_sst_afimoll_1()
{
    test(__FUNCTION__, WKT_AFIMOLL_1);
}

void test_sst_afimoll_2()
{
    test(__FUNCTION__, WKT_AFIMOLL_2);
}

void test_sst_afimoll_3()
{
    test(__FUNCTION__, WKT_AFIMOLL_3);
}

void test_sst_afimoll_4()
{
    test(__FUNCTION__, WKT_AFIMOLL_4);
}

void test_sst_afimoll_5()
{
    test(__FUNCTION__, WKT_AFIMOLL_5);
}

void test_sst_evropeiskyi_U1()
{
    test(__FUNCTION__, WKT_EVROPEISKYI_U1);
}

void test_sst_evropeiskyi_1()
{
    test(__FUNCTION__, WKT_EVROPEISKYI_1);
}

void test_sst_evropeiskyi_2()
{
    test(__FUNCTION__, WKT_EVROPEISKYI_2);
}

void test_sst_evropeiskyi_3()
{
    test(__FUNCTION__, WKT_EVROPEISKYI_3);
}

void test_sst_evropeiskyi_4()
{
    test(__FUNCTION__, WKT_EVROPEISKYI_4);
}

void test_sst_dubai_mall_1()
{
    test(__FUNCTION__, WKT_DUBAI_MALL_1);
}

void test_sst_dubai_mall_2()
{
    test(__FUNCTION__, WKT_DUBAI_MALL_2);
}

void test_sst_dubai_mall_3()
{
    test(__FUNCTION__, WKT_DUBAI_MALL_3);
}

void test_sst_dubai_mall_4()
{
    test(__FUNCTION__, WKT_DUBAI_MALL_4);
}

void test_sst_dubai_mall_G()
{
    test(__FUNCTION__, WKT_DUBAI_MALL_G);
}

void test_sst_dubai_mall_L()
{
    test(__FUNCTION__, WKT_DUBAI_MALL_L);
}

void test_sst_dubai_mall_LG()
{
    test(__FUNCTION__, WKT_DUBAI_MALL_LG);
}
