#include "navigation_graph.h"

namespace navigine::navigation_core {

bool NavigationGraph::isEmpty() const
{
    return edges.empty();
}

NavigationGraph::Vertex NavigationGraph::getVertex(int v) const
{
    auto iter = vertices.find(v);
    if (iter != vertices.end()) {
        return iter->second;
    }

    return NavigationGraph::Vertex();
}

std::vector<NavigationGraph::Edge> NavigationGraph::getAdjacentEdges(int v) const
{
    auto iter = adjacentEdges.find(v);
    if (iter != adjacentEdges.end()) {
        return iter->second;
    }

    return {};
}

NavigationGraph createNavigationGraph(const XYGraph& graph)
{
    NavigationGraph navigationGraph;
    for(auto iter = graph.vertexBegin(); iter != graph.vertexEnd(); ++iter) {
        navigationGraph.vertices[iter->id] = NavigationGraph::Vertex{
            .id = iter->id, .point = iter->point};
    }

    int edgeId = 1;
    for(auto iter = graph.edgesBegin(); iter != graph.edgesEnd(); ++iter) {
        const auto src = navigationGraph.vertices.at(iter->first);
        const auto dst = navigationGraph.vertices.at(iter->second);
        const auto edge = NavigationGraph::Edge{
            .id = edgeId++,
            .src = src.id,
            .dst = dst.id,
            .len = GetDist(src.point, dst.point)};

        navigationGraph.edges.insert(edge);
        navigationGraph.adjacentEdges[src.id].push_back(edge);
        navigationGraph.adjacentEdges[dst.id].push_back(edge);
    }

    return navigationGraph;
}

} // namespace navigine::navigation_core
