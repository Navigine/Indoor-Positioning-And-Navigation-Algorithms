#ifndef NAVIGINE_TRIANGULATION_H
#define NAVIGINE_TRIANGULATION_H

#include <vector>
#include <string>
#include <map>

#include "geometry.h"
#include <navigine/navigation-core/reference_point.h>

namespace navigine {
namespace navigation_core {

struct TriangleVertex
{
  TriangleVertex(double x_, double y_, ReferencePointId id_):
    x(x_), y(y_), id(id_) { }

  bool operator==(const TriangleVertex& v)const { return id == v.id; }
  bool operator!=(const TriangleVertex& v)const { return id != v.id; }
  bool operator< (const TriangleVertex& v)const { return x < v.x || (x == v.x && y < v.y); }
  bool operator> (const TriangleVertex& v)const { return x > v.x || (x == v.x && y > v.y); }
  bool operator<=(const TriangleVertex& v)const { return !(*this > v); }
  bool operator>=(const TriangleVertex& v)const { return !(*this < v); }

  double x;
  double y;
  ReferencePointId id;
};

class TriangleEdge
{
  public:
    TriangleEdge(const TriangleVertex& a, const TriangleVertex& b)
      : mBegin(std::min(a, b)), mEnd(std::max(a, b))
    { }

    TriangleVertex begin()const { return mBegin; }
    TriangleVertex end()const   { return mEnd;   }

    double length()const { return sqrt((mBegin.x - mEnd.x) * (mBegin.x - mEnd.x) +
                                       (mBegin.y - mEnd.y) * (mBegin.y - mEnd.y)); }

    bool operator==(const TriangleEdge& e)const { return mBegin == e.mBegin && mEnd == e.mEnd; }
    bool operator!=(const TriangleEdge& e)const { return mBegin != e.mBegin || mEnd != e.mEnd; }
    bool operator< (const TriangleEdge& e)const { return mBegin < e.mBegin || (mBegin == e.mBegin && mEnd < e.mEnd); }
    bool operator> (const TriangleEdge& e)const { return mBegin > e.mBegin || (mBegin == e.mBegin && mEnd > e.mEnd); }
    bool operator<=(const TriangleEdge& e)const { return !(*this > e); }
    bool operator>=(const TriangleEdge& e)const { return !(*this < e); }

  private:
    TriangleVertex mBegin;
    TriangleVertex mEnd;
};

class Triangle
{
  public:
    Triangle(const TriangleVertex& a,
             const TriangleVertex& b,
             const TriangleVertex& c)
      : mA(a), mB(b), mC(c)
    { }

    TriangleVertex getA()const { return mA; }
    TriangleVertex getB()const { return mB; }
    TriangleVertex getC()const { return mC; }

    TriangleEdge getAB()const { return TriangleEdge(mA, mB); }
    TriangleEdge getBA()const { return TriangleEdge(mA, mB); }

    TriangleEdge getBC()const { return TriangleEdge(mB, mC); }
    TriangleEdge getCB()const { return TriangleEdge(mB, mC); }

    TriangleEdge getAC()const { return TriangleEdge(mA, mC); }
    TriangleEdge getCA()const { return TriangleEdge(mA, mC); }

    bool operator==(const Triangle& t)const { return mA == t.mA && mB == t.mB && mC == t.mC; }
    bool operator!=(const Triangle& t)const { return mA != t.mA || mB != t.mB || mC != t.mC; }
    bool operator< (const Triangle& t)const { return mA < t.mA || (mA == t.mA && mB < t.mB) || (mA == t.mA && mB == t.mB && mC < t.mC); }
    bool operator> (const Triangle& t)const { return mA > t.mA || (mA == t.mA && mB > t.mB) || (mA == t.mA && mB == t.mB && mC > t.mC); }
    bool operator<=(const Triangle& t)const { return !(*this > t); }
    bool operator>=(const Triangle& t)const { return !(*this < t); }

    double square()const
    {
      return 0.5 * fabs(Determinant(mA.x, mB.x, mC.x, mA.y, mB.y, mC.y, 1, 1, 1));
    }

    double sinA()const
    {
      return 2 * square() / (getAC().length() * getAB().length());
    }

    double sinB()const
    {
      return 2 * square() / (getBC().length() * getAB().length());
    }

    double sinC()const
    {
      return 2 * square() / (getBC().length() * getAC().length());
    }

    bool circumcircle_contains(const TriangleVertex& X)const
    {
      if (X == mA || X == mB || X == mC)
        return false;
      double D1 = Determinant(mA.x,  mB.x,  mC.x, mA.y, mB.y, mC.y, 1, 1, 1);
      double D2 = Determinant(mA.x - X.x, mA.y - X.y, (mA.x * mA.x - X.x * X.x) + (mA.y * mA.y - X.y * X.y),
                       mB.x - X.x, mB.y - X.y, (mB.x * mB.x - X.x * X.x) + (mB.y * mB.y - X.y * X.y),
                       mC.x - X.x, mC.y - X.y, (mC.x * mC.x - X.x * X.x) + (mC.y * mC.y - X.y * X.y));
      return D1 * D2 > GEOMETRY_DOUBLE_EPSILON;
    }

  private:
    TriangleVertex mA;
    TriangleVertex mB;
    TriangleVertex mC;
};

std::vector<Triangle> TriangulateVertices(const std::vector<TriangleVertex> &vertices, double maxedge);

} } // namespace navigine::navigation_core

#endif // TRIANGULATION_H
