#pragma once

#include <cmath>

#include <navigine/navigation_core/geometry.h>
#include <navigine/navigation_core/graph.h>

namespace navigine::navigation_core {

struct NavigationGraph
{
    struct Vertex {
        int id = 0;
        XYPoint point;
    };

    struct Edge {
        int id = 0;
        int src = 0;
        int dst = 0;
        double len = 0.0;

        bool operator<(const Edge& e) const { return id < e.id; }
    };

    std::set<NavigationGraph::Edge> edges;
    std::map<int, NavigationGraph::Vertex> vertices;                    // {vertexId -> vertex}
    std::map<int, std::vector<NavigationGraph::Edge>> adjacentEdges;    // {vertexId -> set of adjacent edges}

    bool isEmpty() const;

    NavigationGraph::Vertex getVertex(int v) const;

    std::vector<NavigationGraph::Edge> getAdjacentEdges(int v) const;
};

NavigationGraph createNavigationGraph(const XYGraph& graph);

} // namespace navigine::navigation_core
