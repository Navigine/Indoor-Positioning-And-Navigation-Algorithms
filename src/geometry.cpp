#include <cmath>

#include "geometry.h"

namespace navigine {
namespace navigation_core {

// Get distance from A to B
double GetDist(double ax, double ay, double bx, double by)
{
  return std::sqrt((ax - bx) * (ax - bx) + (ay - by) * (ay - by));
}

// Get distance from A to B
double GetDist(const LocationPoint& A, const LocationPoint& B)
{
  return A.level == B.level ? GetDist(A.x, A.y, B.x, B.y) : NAN;
}

// Get distance from segment to point
double SegmentPointDist(const XYPoint& p1, const XYPoint& p2, const XYPoint& p)
{
  double v1x = p.x - p1.x;
  double v1y = p.y - p1.y;

  double v2x = p2.x - p1.x;
  double v2y = p2.y - p1.y;

  double len = v2x * v2x + v2y * v2y;

  if (len < GEOMETRY_DOUBLE_EPSILON)
    return GetDist(p1.x, p1.y, p.x, p.y);

  double t = (v1x * v2x + v1y * v2y) / len;
  t = std::min(std::max(t, 0.0), 1.0);

  double sx = p1.x + t * (p2.x - p1.x);
  double sy = p1.y + t * (p2.y - p1.y);

  double dist = std::sqrt((sx - p.x) * (sx - p.x) + (sy - p.y) * (sy - p.y));
  return dist;
}

// Get distance from C to line (AB)
double GetDist(const LocationPoint& A, const LocationPoint& B, const LocationPoint& C)
{
  if (A.level != B.level || A.level != C.level)
    return NAN;
  
  double d = GetDist(A, B);
  return d >= GEOMETRY_DOUBLE_EPSILON ? std::fabs((B.x - A.x) * (C.y - A.y) - (C.x - A.x) * (B.y - A.y)) / d : NAN;
}

// Calculate the oriented area of triangle (ABC).
extern double GetArea(const LocationPoint& A,
                      const LocationPoint& B,
                      const LocationPoint& C)
{
  if (A.level != B.level || A.level != C.level)
    return NAN;
  return GetArea(A.x, A.y, B.x, B.y, C.x, C.y);
}

// Calculate the oriented area of triangle (ABC).
extern double GetArea(double ax, double ay,
                      double bx, double by,
                      double cx, double cy)
{
  return Determinant(ax - bx, ay - by, ax - cx, ay - cy) / 2;
}

// Calculate determinant of the 2x2 matrix:
//  a11 a12
//  a21 a22
double Determinant(double a11, double a12,
                   double a21, double a22)
{
  return a11 * a22 - a12 * a21;
}

// Calculate determinant of the 3x3 matrix:
//  a11 a12 a13
//  a21 a22 a23
//  a31 a32 a33
double Determinant(double a11, double a12, double a13,
                   double a21, double a22, double a23,
                   double a31, double a32, double a33)
{
  return (a11 * a22 * a33 + a12 * a23 * a31 + a21 * a32 * a13) -
         (a21 * a12 * a33 + a11 * a23 * a32 + a13 * a22 * a31);
}

// Get projection from (ox,oy) to segment [(ax,ay), (bx,by)].
double GetProjection(double ax, double ay,
                     double bx, double by,
                     double ox, double oy,
                     double* px, double* py)
{
  // Find such point P, that vector OP is orthogonal to the vector AB
  // Decompose vector OP for basis (OA, OB):
  //  OP = (1-k) * OA + k * OB
  // Return value: k
  double ab = ((bx - ax) * (bx - ax) + (by - ay) * (by - ay));
  double ao = ((ox - ax) * (bx - ax) + (oy - ay) * (by - ay));
  double k  = ab > GEOMETRY_DOUBLE_EPSILON ? ao / ab : 0.0;
  if (px) { *px = ax + k * (bx - ax); }
  if (py) { *py = ay + k * (by - ay); }
  return k;
}

double GetIntersection(double  px, double  py,
                       double  qx, double  qy,
                       double  ax, double  ay,
                       double  bx, double  by,
                       double* ox, double* oy)
{
  double pqx = qx - px;
  double pqy = qy - py;
  double abx = bx - ax;
  double aby = by - ay;
  double pax = ax - px;
  double pay = ay - py;
  
  double det = Determinant(pqx, abx, pqy, aby);
  if (std::fabs(det) < GEOMETRY_DOUBLE_EPSILON)
    return NAN;
  
  double k =  Determinant(pax, abx, pay, aby) / det;
  double l = -Determinant(pqx, pax, pqy, pay) / det;
  
  if (k < 0 || k > 1.0 || l < 0 || l > 1.0)
    return NAN;
  
  if (ox) { *ox = px + k * pqx; }
  if (oy) { *oy = py + k * pqy; }
  return k;
}

// Check if segments [AB] and [CD] has intersection
bool CheckIntersection(double ax, double ay,
                       double bx, double by,
                       double cx, double cy,
                       double dx, double dy)
{
  auto boundBoxCheck = [](double a, double b, double c, double d) -> bool
  {
    if (a > b) std::swap(a, b);
    if (c > d) std::swap(c, d);
    return std::max(a,c) < std::min(b,d);
  };
  return boundBoxCheck(ax, bx, cx, dx) &&
         boundBoxCheck(ay, by, cy, dy) &&
         GetArea(ax, ay, bx, by, cx, cy) * GetArea(ax, ay, bx, by, dx, dy) < 0 &&
         GetArea(cx, cy, dx, dy, ax, ay) * GetArea(cx, cy, dx, dy, bx, by) < 0;
}

bool PointOnLine(double x, double y, double x1, double y1, double x2, double y2)
{
  const double Ax = x1 - x;
  const double Ay = y1 - y;
  const double Bx = x2 - x;
  const double By = y2 - y;
  
  return (std::fabs(Ax * By - Ay * Bx) < GEOMETRY_DOUBLE_EPSILON &&
          Ax * Bx + Ay * By < GEOMETRY_DOUBLE_EPSILON);
}

bool XRayIntersectsLine(double x, double y, double x1, double y1, double x2, double y2)
{
  static const double DELTA = std::sqrt(GEOMETRY_DOUBLE_EPSILON);
  
  if (std::fabs(y - y1) < GEOMETRY_DOUBLE_EPSILON)
    y1 += DELTA;
  
  if (std::fabs(y - y2) < GEOMETRY_DOUBLE_EPSILON)
    y2 += DELTA;
  
  if (y1 < y && y < y2)
    return Determinant(x2, y2, 1, x, y, 1, x1, y1, 1) + GEOMETRY_DOUBLE_EPSILON > 0;
  
  if (y2 < y && y < y1)
    return Determinant(x1, y1, 1, x, y, 1, x2, y2, 1) + GEOMETRY_DOUBLE_EPSILON > 0;
  
  return false;
}

} } // namespace navigine::navigation_core
