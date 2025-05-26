#include "graph_snapper.h"

namespace navigine::navigation_core {

namespace {

const double LAST_MILE_WEIGHT = 3.0;

} // namespace

GraphSnapper::GraphSnapper(
    const XYGraph& graph,
    const double maxProjectionDistance,
    const double maxProjectionSpeed)
    : graph_(graph)
    , maxProjectionDistance_(maxProjectionDistance)
    , maxProjectionSpeed_(maxProjectionSpeed)
{
}

XYPoint GraphSnapper::addPoint(const XYPoint& point, Timestamp ts)
{
    assert(!currentTs_ || *currentTs_ <= ts);

    if (maxProjectionSpeed_ > 0.0 && currentPoint_ && currentTs_) {
        const auto maxAdvance = GetDeltaTimeSec(ts, *currentTs_) * maxProjectionSpeed_;
        const auto nextPoint = advanceProjection(*currentPoint_, point, maxAdvance);
        if (nextPoint && GetDist(*nextPoint, point) <= maxProjectionDistance_) {
            currentPoint_ = *nextPoint;
            currentTs_ = ts;
            return *nextPoint;
        }
    }

    const auto projPoint = makeProjection(point);

    if (!projPoint || GetDist(*projPoint, point) > maxProjectionDistance_) {
        currentPoint_ = std::nullopt;
        currentTs_ = ts;
        return point;
    }

    currentPoint_ = *projPoint;
    currentTs_ = ts;
    return *projPoint;
}

std::optional<XYPoint> GraphSnapper::makeProjection(const XYPoint& point) const
{
    std::optional<XYPoint> result;
    std::optional<double> minDist;

    for(auto iter = graph_.edgesBegin(); iter != graph_.edgesEnd(); ++iter) {
        const auto src = graph_.getVertex(iter->first);
        const auto dst = graph_.getVertex(iter->second);
        const auto segment = XYSegment{src.point, dst.point};
        const auto ratio = std::clamp(GetProjectionRatio(segment, point), 0.0, 1.0);
        const auto ppoint = GetRatioPoint(segment, ratio);
        const auto dist = GetDist(point, ppoint);

        if (!minDist || *minDist > dist) {
            minDist = dist;
            result = ppoint;
        }
    }

    return result;
}

std::optional<XYPoint> GraphSnapper::advanceProjection(const XYPoint& src, const XYPoint& dst, double maxAdvance) const
{
    if (maxAdvance < EPSILON) {
        return src;
    }

    const auto options = RouteOptions{
        .firstMileWeight = 1000,                // src-projection must be made to the closest edge
        .lastMileWeight = LAST_MILE_WEIGHT,     // dst-projection will be made to the balanced edge
        .includeFirstMile = false,
        .includeLastMile = false,
    };

    return GetPathPoint(
        GetGraphPath(graph_, src, dst, options),
        maxAdvance);
}


///
/// GraphSnapper2
///

GraphSnapper2::GraphSnapper2(
    const XYGraph& graph,
    const double maxProjectionDistance,
    const double maxProjectionSpeed)
    : graph_(graph)
    , adjList_(BuildAdjacencyList(graph))
    , dmatrix_(CalculateDistanceMatrix(adjList_))
    , maxProjectionDistance_(maxProjectionDistance)
    , maxProjectionSpeed_(maxProjectionSpeed)
{
}

XYPoint GraphSnapper2::addPoint(const XYPoint& point, Timestamp ts)
{
    assert(!currentTs_ || *currentTs_ <= ts);

    if (!currentTs_) {
        currentTs_ = ts;
    }

    if (GetDeltaTimeSec(ts, *currentTs_) < 1.0) {
        return projection_ ? projection_->point : point;
    }

    const auto maxAdvance = GetDeltaTimeSec(ts, *currentTs_) * maxProjectionSpeed_;

    point_ = point;
    currentTs_ = ts;

    if (projection_) {
        if ((projection_ = advanceProjection(point, *projection_, maxAdvance))) {
            return projection_->point;
        }
    }

    if ((projection_ = makeProjection(point))) {
        return projection_->point;
    }

    return point;
}

std::optional<GraphSnapper2::Projection> GraphSnapper2::makeProjection(const XYPoint& point) const
{
    std::optional<Projection> projection;

    for(auto iter = graph_.edgesBegin(); iter != graph_.edgesEnd(); ++iter) {
        const auto src = graph_.getVertex(iter->first);
        const auto dst = graph_.getVertex(iter->second);
        const auto segment = XYSegment{src.point, dst.point};
        const auto ratio = std::clamp(GetProjectionRatio(segment, point), 0.0, 1.0);
        const auto ppoint = GetRatioPoint(segment, ratio);
        const auto dist = GetDist(point, ppoint);

        if (dist > maxProjectionDistance_) {
            continue;
        }

        if (!projection || projection->distance > dist) {
            projection = Projection{
                .src = src,
                .dst = dst,
                .ratio = ratio,
                .distance = dist,
                .point = ppoint,
            };
        }
    }

    return projection;
}

std::optional<GraphSnapper2::Projection> GraphSnapper2::advanceProjection(
    const XYPoint& point,
    const Projection& projection,
    double maxAdvance) const
{
    if (maxAdvance < EPSILON) {
        return projection;
    }

    const double edgeLength = GetDist(projection.src.point, projection.dst.point);
    const double lengthSrc = edgeLength * projection.ratio;
    const double lengthDst = edgeLength * (1 - projection.ratio);

    std::optional<Projection> result;

    const auto updateResult =
        [&](const XYGraph::Vertex& src, const XYGraph::Vertex& dst, double srcRatio, double dstRatio) {
            const auto segment = XYSegment{src.point, dst.point};
            const auto ratio = std::clamp(GetProjectionRatio(segment, point), srcRatio, dstRatio);
            const auto ppoint = GetRatioPoint(segment, ratio);
            const auto dist = GetDist(point, ppoint);
            if (dist > maxProjectionDistance_) {
                return;
            }
            if (!result || result->distance > dist) {
                result = Projection{
                    .src = src,
                    .dst = dst,
                    .ratio = ratio,
                    .distance = dist,
                    .point = ppoint,
                };
            }
        };

    updateResult(
        projection.src,
        projection.dst,
        std::max(projection.ratio - maxAdvance / edgeLength, 0.0),
        std::min(projection.ratio + maxAdvance / edgeLength, 1.0));

    for(int u = 0; u < graph_.verticesNumber(); ++u) {
        const auto dist = std::min(
            dmatrix_[projection.src.id][u] + lengthSrc,
            dmatrix_[projection.dst.id][u] + lengthDst);

        if (dist < maxAdvance) {
            for(const auto& [v, length] : adjList_[u]) {
                updateResult(
                    graph_.getVertex(u),
                    graph_.getVertex(v),
                    0.0,
                    std::min((maxAdvance - dist) / length, 1.0));
            }
        }
    }

    return result;
}

} // namespace navigine::navigation_core
