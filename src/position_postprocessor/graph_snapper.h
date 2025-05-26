#pragma once

#include <navigine/navigation-core/geometry.h>
#include <navigine/navigation-core/graph.h>

namespace navigine::navigation_core {

class GraphSnapper {
public:
    GraphSnapper(
        const XYGraph& graph,
        const double maxProjectionDistance,
        const double maxProjectionSpeed);

    XYPoint addPoint(const XYPoint& point, Timestamp ts);

private:
    std::optional<XYPoint> makeProjection(const XYPoint& point) const;

    std::optional<XYPoint> advanceProjection(
        const XYPoint& sourcePoint,
        const XYPoint& targetPoint,
        double maxAdvance) const;

private:
    const XYGraph graph_;
    const double maxProjectionDistance_;    // [m]
    const double maxProjectionSpeed_;       // [m/s]

    std::optional<Timestamp> currentTs_;
    std::optional<XYPoint> currentPoint_;
};

class GraphSnapper2 {
public:
    GraphSnapper2(
        const XYGraph& graph,
        const double maxProjectionDistance,
        const double maxProjectionSpeed);

    XYPoint addPoint(const XYPoint& point, Timestamp ts);

private:
    const XYGraph graph_;
    const std::vector<std::vector<std::pair<int, double>>> adjList_;
    const std::vector<std::vector<double>> dmatrix_;
    const double maxProjectionDistance_;    // [m]
    const double maxProjectionSpeed_;       // [m/s]

    struct Projection {
        XYGraph::Vertex src;
        XYGraph::Vertex dst;
        double ratio;
        double distance;
        XYPoint point;
    };

    std::optional<XYPoint> point_;
    std::optional<Projection> projection_;
    std::optional<Timestamp> currentTs_;

private:
    std::optional<Projection> makeProjection(const XYPoint& point) const;

    std::optional<Projection> advanceProjection(
        const XYPoint& point,
        const Projection& projection,
        double maxAdvance) const;
};

} // namespace navigine::navigation_core
