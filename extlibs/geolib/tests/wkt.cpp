#include "point.h"

namespace {

const int PRECISION = 20;

template <typename Geometry>
void test_read_wkt_geometry(
    const std::string& wkt,
    const Geometry& expectedGeom)
{
    Geometry geom;

    std::stringstream ss;
    ss << std::setprecision(PRECISION);
    ss << "fromWkt: " << wkt << std::endl;

    try {
        geom = geolib::fromWkt<Geometry>(wkt);
    } catch (geolib::Exception& e) {
        ss << "Error parsing wkt string: " << wkt << " error: " << e.what() << std::endl;
        throw std::runtime_error(ss.str());
    }

    if (!geolib::equal(geom, expectedGeom)) {
        ss << "geometry: " << geolib::toWkt(geom, PRECISION) << std::endl;
        ss << "expected: " << geolib::toWkt(expectedGeom, PRECISION) << std::endl;
        throw std::runtime_error(ss.str());
    }

    std::cout << ss.str() << std::endl;
}

} // namespace

void test_read_wkt_point()
{
    test_read_wkt_geometry(
        "POINT(1.23456789 -3.987654321e2)",
        XYPoint{1.23456789, -3.987654321e2});
}

void test_read_wkt_segment()
{
    test_read_wkt_geometry(
        "SEGMENT(1 2, 3 4)",
        XYSegment{
            .first = XYPoint{1, 2},
            .second = XYPoint{3, 4}});
}

void test_read_wkt_ring()
{
    XYRing ring;

    ring.points.push_back(XYPoint{0, 4});
    ring.points.push_back(XYPoint{2, 2});
    ring.points.push_back(XYPoint{0, 0});
    ring.points.push_back(XYPoint{5, 0});
    ring.points.push_back(XYPoint{5, 4});
    ring.points.push_back(XYPoint{0, 4});

    test_read_wkt_geometry(
        "LINEARRING(0 4, 2 2, 0 0, 5 0, 5 4, 0 4)",
        ring);
}

void test_read_wkt_polygon()
{
    XYPolygon poly;

    poly.outer.points.push_back(XYPoint{0, 4});
    poly.outer.points.push_back(XYPoint{0, 0});
    poly.outer.points.push_back(XYPoint{4, 0});
    poly.outer.points.push_back(XYPoint{4, 4});
    poly.outer.points.push_back(XYPoint{0, 4});

    poly.inners.push_back({});
    poly.inners.back().points.push_back(XYPoint{1, 1});
    poly.inners.back().points.push_back(XYPoint{1, 3});
    poly.inners.back().points.push_back(XYPoint{3, 3});
    poly.inners.back().points.push_back(XYPoint{3, 1});
    poly.inners.back().points.push_back(XYPoint{1, 1});

    test_read_wkt_geometry(
        "POLYGON((0 4, 0 0, 4 0, 4 4, 0 4), (1 1, 1 3, 3 3, 3 1, 1 1))",
        poly);
}

void test_read_wkt_multipolygon()
{
    XYMultiPolygon geom;

    geom.parts.resize(3);

    geom.parts[0].outer.points.push_back(XYPoint{0, 0});
    geom.parts[0].outer.points.push_back(XYPoint{2, 0});
    geom.parts[0].outer.points.push_back(XYPoint{1.33, 1.33});
    geom.parts[0].outer.points.push_back(XYPoint{0, 0});

    geom.parts[1].outer.points.push_back(XYPoint{4, 0});
    geom.parts[1].outer.points.push_back(XYPoint{5, 0});
    geom.parts[1].outer.points.push_back(XYPoint{5, 4});
    geom.parts[1].outer.points.push_back(XYPoint{4, 0});

    geom.parts[2].outer.points.push_back(XYPoint{4, 3});
    geom.parts[2].outer.points.push_back(XYPoint{5, 4});
    geom.parts[2].outer.points.push_back(XYPoint{4, 4});
    geom.parts[2].outer.points.push_back(XYPoint{4, 3});

    test_read_wkt_geometry(
        "MULTIPOLYGON(((0 0, 2 0, 1.33 1.33, 0 0)), ((4 0, 5 0, 5 4, 4 0)), ((4 3, 5 4, 4 4, 4 3)))",
        geom);
}
