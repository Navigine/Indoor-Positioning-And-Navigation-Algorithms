#pragma once

#define GEOLIB_NAMESPACE navigine::navigation_core::geolib

#include <cmath>

#include <extlibs/geolib/include/geolib/models.h>
#include <extlibs/geolib/include/geolib/operations.h>
#include <extlibs/geolib/include/geolib/spatial_search_tree.h>

// #include <navigine/navigation-core/random.h>
#include <navigine/navigation-core/vector2d.h>

namespace navigine::navigation_core {

struct GeoPoint
{
    double latitude = 0.0;
    double longitude = 0.0;
};

struct XYPoint
{
    double x = 0.0;
    double y = 0.0;

    XYPoint& operator+=(const XYPoint& p);
    XYPoint& operator-=(const XYPoint& p);
    XYPoint& operator*=(double multiplier);
    XYPoint& operator/=(double divisor);
};

bool operator==(const GeoPoint& p1, const GeoPoint& p2);
bool operator!=(const GeoPoint& p1, const GeoPoint& p2);

bool operator==(const XYPoint& p1, const XYPoint& p2);
bool operator!=(const XYPoint& p1, const XYPoint& p2);

XYPoint operator+(const XYPoint& p1, const XYPoint& p2);
XYPoint operator-(const XYPoint& p1, const XYPoint& p2);
XYPoint operator*(const XYPoint& p, double multiplier);
XYPoint operator*(double multiplier, const XYPoint& p);
XYPoint operator/(const XYPoint& p, double divisor);

XYPoint operator+(const XYPoint& p, const Vector2d& v);
XYPoint operator-(const XYPoint& p, const Vector2d& v);

// namespace geolib::util {

// template <> inline double getX(const XYPoint& point) { return point.x; }
// template <> inline double getY(const XYPoint& point) { return point.y; }
// template <> inline double& getX(XYPoint& point) { return point.x; }
// template <> inline double& getY(XYPoint& point) { return point.y; }
// template <> inline double maxX<XYPoint>() { return 5e7; }
// template <> inline double maxY<XYPoint>() { return 5e7; }

// template <> inline double getX(const GeoPoint& point) { return point.longitude; }
// template <> inline double getY(const GeoPoint& point) { return point.latitude; }
// template <> inline double& getX(GeoPoint& point) { return point.longitude; }
// template <> inline double& getY(GeoPoint& point) { return point.latitude; }
// template <> inline double maxX<GeoPoint>() { return 1000; }
// template <> inline double maxY<GeoPoint>() { return 1000; }

// } // namespace geolib::util

using GeoBox = geolib::Box<GeoPoint>;
using GeoRing = geolib::Ring<GeoPoint>;
using GeoPolygon = geolib::Polygon<GeoPoint>;
using GeoMultiPolygon = geolib::MultiPolygon<GeoPoint>;
using GeoLineString = geolib::LineString<GeoPoint>;
using GeoMultiLineString = std::vector<GeoLineString>;
using GeoSegment = geolib::Segment<GeoPoint>;
using GeoGeometry = geolib::Geometry<GeoPoint>;

using XYBox = geolib::Box<XYPoint>;
using XYRing = geolib::Ring<XYPoint>;
using XYPolygon = geolib::Polygon<XYPoint>;
using XYMultiPolygon = geolib::MultiPolygon<XYPoint>;
using XYLineString = geolib::LineString<XYPoint>;
using XYMultiLineString = std::vector<XYLineString>;
using XYSegment = geolib::Segment<XYPoint>;
using XYGeometry = geolib::Geometry<XYPoint>;

XYPoint GeoToLocal(const GeoPoint& point, const GeoPoint& bindPoint);
GeoPoint LocalToGeo(const XYPoint& point, const GeoPoint& bindPoint);

XYRing GeoToLocal(const GeoRing& ring, const GeoPoint& bindPoint);
GeoRing LocalToGeo(const XYRing& ring, const GeoPoint& bindPoint);

XYPolygon GeoToLocal(const GeoPolygon& polygon, const GeoPoint& bindPoint);
GeoPolygon LocalToGeo(const XYPolygon& polygon, const GeoPoint& bindPoint);

XYMultiPolygon GeoToLocal(const GeoMultiPolygon& geometry, const GeoPoint& bindPoint);
GeoMultiPolygon LocalToGeo(const XYMultiPolygon& geometry, const GeoPoint& bindPoint);
void IterateVertices(
    const XYMultiPolygon& geometry,
    const std::function<void(const XYPoint&)>& func);

double GetBoxWidth(const XYBox& box);
double GetBoxHeight(const XYBox& box);

// Calculate norm of the point-vector
double GetNorm(const XYPoint& point);

// Calculate distance between points
double GetDist(const XYPoint& point1, const XYPoint& point2);

// Calculate squared distance between points
double GetSqrDist(const XYPoint& point1, const XYPoint& point2);

// Calculate distance between geo-points on the geoid
double GetGeoDist(const GeoPoint& point1, const GeoPoint& point2);

struct XYProjection
{
    XYPoint point;
    double distance = 0;
};

// Get ratio of segments separated by a point
double GetProjectionRatio(const XYSegment& segment, const XYPoint& point);

// Get projection point by ratio
XYPoint GetRatioPoint(const XYSegment& segment, double ratio);

// Get the closest point on the segment to the specified point
XYProjection GetProjection(const XYSegment& segment, const XYPoint& point, double* advance = nullptr);

// Get the closest point and advance on the linestring to the specified point
XYProjection GetProjection(const XYLineString& line, const XYPoint& point, double* advance = nullptr);

XYProjection GetProjection(const XYRing& ring, const XYPoint& p);

XYProjection GetProjection(const XYPolygon& poly, const XYPoint& p);

XYProjection GetProjection(const XYMultiPolygon& geom, const XYPoint& p);

// Calculate distance from segment to point
double SegmentPointDist(const XYSegment& segment, const XYPoint& point);

// Get distance from line to point
double LinePointDist(const XYSegment& line, const XYPoint& point);

// Check if segments [AB] and [CD] have intersection
bool CheckIntersection(const XYSegment& segmentFirst, const XYSegment& segmentSecond);

// Function calculates the intersection of lines (p1,p2) and (q1,q2).
// If lines are parallel or coincide, function returns std::nullopt.
// Otherwise, function returns the intersection point and (u,v) are
// set so that the following equation is true:
//  p1 + u * (p2 - p1) = q1 + v * (q2 - q1) = <itersection point>.
std::optional<XYPoint> LineIntersection(
    const XYPoint& p1,
    const XYPoint& p2,
    const XYPoint& q1,
    const XYPoint& q2,
    double& u,
    double& v);

std::optional<XYPoint> SegmentIntersection(
    const XYPoint& p1,
    const XYPoint& p2,
    const XYPoint& q1,
    const XYPoint& q2,
    double& u,
    double& v);

// Calculate determinant of the 2x2 matrix:
//  |a11 a12|
//  |a21 a22|
double Determinant(
    double a11, double a12,
    double a21, double a22);

// Calculate determinant of the 3x3 matrix:
//  |a11 a12 a13|
//  |a21 a22 a23|
//  |a31 a32 a33|
double Determinant(
    double a11, double a12, double a13,
    double a21, double a22, double a23,
    double a31, double a32, double a33);

// Convert degrees to radians and vice versa
double DegToRad(double deg);
double RadToDeg(double rad);

// Adjust angle to [min, max) interval
double NormalizedAngle(
    double value,
    double min = -M_PI,
    double max = +M_PI);

double GetAverageAngle(
    const std::vector<double>& angles,
    const std::vector<double>& weights);

struct XYTriangle
{
    XYPoint a;
    XYPoint b;
    XYPoint c;
};

double TriangleArea(const XYTriangle& t);

// XYPoint TriangleRandomPoint(const XYTriangle& t, Rng* rng);

XYPoint TriangleCenter(const XYTriangle& t);

// Triangulate multipolygon
std::vector<XYTriangle> Triangulate(const XYMultiPolygon& geometry);

} // namespace navigine::navigation_core
