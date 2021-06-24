/** geometry.h
 *
 * Copyright (c) 2018 Navigine.
 *
 */

#ifndef NAVIGINE_GEOMETRY_H
#define NAVIGINE_GEOMETRY_H

#include <navigine/navigation-core/location_point.h>

namespace navigine {
namespace navigation_core {

static const double GEOMETRY_DOUBLE_EPSILON = 1e-8;

// Get distance from segment to point
extern double SegmentPointDist(const XYPoint &p1,
                               const XYPoint &p2,
                               const XYPoint &p);

// Get distance from A to B
extern double GetDist(const LocationPoint& A, const LocationPoint& B);
extern double GetDist(double ax, double ay, double bx, double by);

// Get distance from C to line (AB)
extern double GetDist(const LocationPoint& A,
                      const LocationPoint& B,
                      const LocationPoint& C);

// Calculate the oriented area of triangle (ABC).
extern double GetArea(const LocationPoint& A,
                      const LocationPoint& B,
                      const LocationPoint& C);

// Calculate the oriented area of triangle (ABC).
extern double GetArea(double ax, double ay,
                      double bx, double by,
                      double cx, double cy);

// Calculate determinant of the 2x2 matrix:
//  a11 a12
//  a21 a22
extern double Determinant(double a11, double a12,
                          double a21, double a22);

// Calculate determinant of the 3x3 matrix:
//  a11 a12 a13
//  a21 a22 a23
//  a31 a32 a33
extern double Determinant(double a11, double a12, double a13,
                          double a21, double a22, double a23,
                          double a31, double a32, double a33);

// Get distance from O to segment [AB].
// Px - is the closest point on [AB].
extern double GetProjection(double ax, double ay,
                            double bx, double by,
                            double ox, double oy,
                            double* px, double* py);

// Get intersection of vector PQ and segment [AB].
extern double GetIntersection(double px, double py,
                              double qx, double qy,
                              double ax, double ay,
                              double bx, double by,
                              double* x, double* y);

// Check if segments [AB] and [CD] has intersection
extern bool CheckIntersection(double ax, double ay,
                              double bx, double by,
                              double cx, double cy,
                              double dx, double dy);

extern bool PointOnLine        ( double x, double y, double x1, double y1, double x2, double y2 );
extern bool XRayIntersectsLine ( double x, double y, double x1, double y1, double x2, double y2 );

} } // namespace navigine::navigation_core

#endif
