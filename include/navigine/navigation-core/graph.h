#pragma once

#include <vector>
#include <set>

#include <navigine/navigation-core/geometry.h>

namespace navigine::navigation_core {

template <typename Point>
class Graph
{
public:
    struct Vertex {
        int id;
        Point point;
    };

    using VertexIterator = typename std::vector<Vertex>::const_iterator;
    using EdgeIterator = typename std::set<std::pair<int,int>>::const_iterator;
    using Path = std::vector<Point>;

public:
    int verticesNumber() const {
        return vertices_.size();
    }

    int edgesNumber() const {
        return edges_.size();
    }

    bool hasVertex(int index) const {
        return 0 <= index && index < static_cast<int>(vertices_.size());
    }

    Vertex getVertex(int index) const {
        return vertices_.at(index);
    }

    VertexIterator vertexBegin() const {
        return vertices_.begin();
    }

    VertexIterator vertexEnd() const {
        return vertices_.end();
    }

    EdgeIterator edgesBegin() const {
        return edges_.begin();
    }

    EdgeIterator edgesEnd() const {
        return edges_.end();
    }

    Vertex addVertex(const Point& point) {
        const auto vertex = Vertex {
            .id = static_cast<int>(vertices_.size()),
            .point = point
        };
        vertices_.push_back(vertex);
        return vertex;
    }

    void addEdge(int v1, int v2)
    {
        if (!hasVertex(v1) || !hasVertex(v2)) {
            throw std::logic_error("vertex does not exist!");
        }

        if (v1 == v2) {
            return;
        }

        edges_.insert(v1 < v2 ?
            std::make_pair(v1, v2) :
            std::make_pair(v2, v1));
    }

    bool isEmpty() const {
        return vertices_.empty();
    }

private:
    std::vector<Vertex> vertices_;
    std::set<std::pair<int,int>> edges_;  // contains indices of elements in vertices vector
};

using XYGraph = Graph<XYPoint>;
using XYPath = Graph<XYPoint>::Path;
using GeoGraph = Graph<GeoPoint>;
using AdjacencyList = std::vector<std::vector<std::pair<int, double>>>;

std::ostream& operator<<(std::ostream& os, const AdjacencyList& adjList);

// Implementation of Dijkstra's algorithm: https://en.wikipedia.org/wiki/Dijkstra's_algorithm
// This function calculates the shortest paths from a specified starting vertex in a weighted graph.
// The graph should be represented as an adjacency list.
// The function returns a pair of arrays, `dist` and `prev`, both of length N, where N is the number of vertices in the graph.
// The contents of these arrays are defined as follows. For every vertex v != start:
//  - dist[v] contains the length of the shortest path from `start` to `v`, or infinity if no such path exists;
//  - prev[v] is equal to the vertex `u` such that the edge (u, v) is part of the shortest path from `start` to `v`,
//    or -1 if no such path exists.
// If the optional `stop` parameter is specified (default is -1), the function will stop and return results once the shortest
// path to the `stop` vertex has been computed, thus avoiding unnecessary computations for other vertices.
std::pair<std::vector<double>, std::vector<int>> Dijkstra(const AdjacencyList& adjList, int start, int stop = -1);

// Calculate the distance matrix for the specified graph (i.e. distances between every pair of graph vertices).
// The graph should be represented as an adjacency list.
std::vector<std::vector<double>> CalculateDistanceMatrix(const AdjacencyList& adjList);

// Build graph adjacency list.
AdjacencyList BuildAdjacencyList(const XYGraph& graph);

// Calculate the closest graph point to the specified point
XYProjection GetGraphProjection(const XYGraph& graph, const XYPoint& point);

struct RouteOptions {
    double firstMileWeight = 1000.0;
    double lastMileWeight = 1000.0;
    bool includeFirstMile = true;
    bool includeLastMile = true;
};

// Calculate the shortest path between the two points (using Dijkstra algorithm)
XYPath GetGraphPath(const XYGraph& graph, const XYPoint& src, const XYPoint& dst, const RouteOptions& options);

// Get the point on the specified path with the specified advance
std::optional<XYPoint> GetPathPoint(const XYPath& path, double advance);

// Calculate the path length
double GetPathLength(const XYPath& path);

} // namespace navigine::navigation_core
