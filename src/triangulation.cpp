#include "triangulation.h"

namespace navigine {
namespace navigation_core {

static const double INDENT_METERS = 10.0;

std::vector<Triangle> TriangulateVertices(const std::vector<TriangleVertex>& vertices, double maxedge)
{
  std::vector<Triangle> T;
  if (vertices.size() < 3)
    return T;

  // Borders of the rectangle
  double minX =  std::numeric_limits<double>::max();
  double maxX = -std::numeric_limits<double>::max();
  double minY =  std::numeric_limits<double>::max();
  double maxY = -std::numeric_limits<double>::max();

  // find rectangle containing all the vertices
  for (std::vector<TriangleVertex>::const_iterator it = vertices.begin(); it != vertices.end(); it++)
  {
      const TriangleVertex& v = *it;
      if (v.x > maxX)
          maxX = v.x;

      if (v.x < minX)
          minX = v.x;

      if (v.y > maxY)
          maxY = v.y;

      if (v.y < minY)
          minY = v.y;
  }

  // Adding indents to avoid checks for boundary cases
  minX -= INDENT_METERS;
  maxX += INDENT_METERS;
  minY -= INDENT_METERS;
  maxY += INDENT_METERS;

  std::vector<TriangleVertex> V;

  // Defining "super-triangle"
  V.push_back(TriangleVertex(minX + minY - maxY, minY, ReferencePointId("super1")));
  V.push_back(TriangleVertex(maxX + maxY - minY, minY, ReferencePointId("super2")));
  V.push_back(TriangleVertex((minX + maxX) / 2,
                             maxY + (maxX - minX) / 2,
                             ReferencePointId("super3")));

  V.insert(V.end(), vertices.begin(), vertices.end());

  T.push_back(Triangle(V[0], V[1], V[2]));

  // Adding vertices to the triangulation
  for(size_t i = 0; i < V.size(); ++i)
  {
    // Looking for triangles, for which the circumscribed circle contains V[i]
    // Removing those triangles, saving their edges in edge buffer E
    std::vector<TriangleEdge> E;
    for(size_t j = 0; j < T.size(); )
    {
      if (T[j].circumcircle_contains(V[i]))
      {
        E.push_back(T[j].getAB());
        E.push_back(T[j].getBC());
        E.push_back(T[j].getCA());
        T.erase(T.begin() + j);
        continue;
      }
      ++j;
    }

    // Adding all triangles between edges and current vertex
    // ignoring the duplicating edges
    std::sort(E.begin(), E.end());
    for(size_t j = 0; j < E.size(); ++j)
    {
      if (j + 1 < E.size() && E[j] == E[j+1])
        ++j;
      else
        T.push_back(Triangle(V[i], E[j].begin(), E[j].end()));
    }
  }

  double minsin = -1;
  for(size_t i = 0; i < T.size(); ++i)
  {
    if (T[i].getA() == V[0] || T[i].getB() == V[0] || T[i].getC() == V[0] ||
        T[i].getA() == V[1] || T[i].getB() == V[1] || T[i].getC() == V[1] ||
        T[i].getA() == V[2] || T[i].getB() == V[2] || T[i].getC() == V[2] ||
        T[i].getAB().length() > maxedge ||
        T[i].getBC().length() > maxedge ||
        T[i].getAC().length() > maxedge ||
        T[i].sinA() < minsin ||
        T[i].sinB() < minsin ||
        T[i].sinC() < minsin)
      T.erase(T.begin() + i--);
  }
  V.erase(V.begin(), V.begin() + 3); // Erasing elements V[0], V[1], V[2]
  std::sort(V.begin(), V.end());
  std::sort(T.begin(), T.end());

  return T;
}

} } // namespace navigine::navigation_core
