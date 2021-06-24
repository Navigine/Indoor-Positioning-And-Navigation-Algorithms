/** graph.h
*
* Copyright (c) 2019 Navigine.
*
*/

#ifndef NAVIGINE_GRAPH_H
#define NAVIGINE_GRAPH_H

#include <vector>
#include <map>
#include <set>

namespace navigine {
namespace navigation_core {

template <typename Point>
class Graph
{
public:
  struct Vertex
  {
    int id;
    Point point;
  };

public:
  typedef typename std::vector<Vertex>::const_iterator VertexIterator;
  typedef std::set<std::pair<int, int> >::const_iterator EdgeIterator;

public:
  Graph() {}

  bool hasVertex(int id) const
  {
    return id >= 0 && id < static_cast<int>(mVertices.size());
  }

  const Vertex& vertex(int id) const
  {
    return mVertices.at(id);
  }

  const Vertex getVertex(int id) const
  {
    return mVertices.at(id);
  }

  const VertexIterator vertexBegin() const
  {
    return mVertices.begin();
  }

  const VertexIterator vertexEnd() const
  {
    return mVertices.end();
  }

  const EdgeIterator edgesBegin() const
  {
    return mEdges.begin();
  }

  const EdgeIterator edgesEnd() const
  {
    return mEdges.end();
  }

  Vertex addVertex(const Point& p)
  {
    Vertex v = {static_cast<int>(mVertices.size()), p};
    mVertices.push_back(v);
    return v;
  }

  void addEdge(int v1, int v2)
  {
    if (!hasVertex(v1) || !hasVertex(v2)) {
        throw std::logic_error("vertex does not exist!");
    }

    std::pair<int, int> p = v1 < v2 ? std::make_pair(v1, v2) : std::make_pair(v2, v1);
    mEdges.insert(std::move(p));
  }

private:
  std::vector<Vertex> mVertices;
  std::set<std::pair<int, int>> mEdges;  // contains indices of elements in vertices vector
};

} } // namespace navigine::navigation_core

#endif // NAVIGINE_GRAPH_H
