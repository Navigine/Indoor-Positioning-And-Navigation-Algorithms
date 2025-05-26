#pragma once

#include "contains.h"
#include "correct.h"
#include "graph.h"
#include "grid_point.h"
#include "iterate_edges.h"
#include "point_helper.h"
#include "type_checks.h"
#include "util.h"
#include "wkt_write.h"

namespace GEOLIB_NAMESPACE::detail {

enum EdgeType {
    INNER = 0,
    OUTER = 1,
    BORDER = 2,
    BORDER_REVERSE = 3,
};

template <typename Point>
EdgeType getEdgeType(
    const Graph<Point>& graph,
    const typename Graph<Point>::Edge& edge,
    typename Graph<Point>::TColor color)
{
    static_assert(isPoint<Point>());

    assert(edge.color != color);

    // Searching for the straight edge
    if (graph.edgeFind(color, edge.src, edge.dst) != graph.edgeEnd()) {
#ifdef GEOLIB_DEBUG_ENABLED
        std::cout << "getEdgeType: [color=" << edge.color << ", src=" << edge.src << ", dst=" << edge.dst << "]: BORDER\n";
#endif
        return EdgeType::BORDER;
    }

    // Searching for the reverse edge
    if (graph.edgeFind(color, edge.dst, edge.src) != graph.edgeEnd()) {
#ifdef GEOLIB_DEBUG_ENABLED
        std::cout << "getEdgeType: [color=" << edge.color << ", src=" << edge.src << ", dst=" << edge.dst << "]: BORDER_REVERSE\n";
#endif
        return EdgeType::BORDER_REVERSE;
    }

    const auto index = graph.edgeIndex(edge, color);

#ifdef GEOLIB_DEBUG_ENABLED
    std::cout << "getEdgeType: [color=" << edge.color << ", src=" << edge.src << ", dst=" << edge.dst << ", index = " << index << "\n";
#endif

    if (!index) {
        throw Exception("Error in getEdgeType: invalid edge index. Check input geometries.");
    }

    return index > 0 ?
        EdgeType::INNER :
        EdgeType::OUTER;
}

template <typename Point, typename VertexIterator>
MultiPolygon<Point> makeGeometryFromRings(const std::vector<std::vector<VertexIterator>>& rings)
{
    static_assert(isPoint<Point>());

    if (rings.empty()) {
        return {};
    }

    static const auto gridArea =
        [](const std::vector<VertexIterator>& ring) {
            int128_t result(0);
            for(size_t i = 0; i + 1 < ring.size(); ++i) {
                int128_t tmp1(ring[i]->gpoint.x - ring[i+1]->gpoint.x);
                int128_t tmp2(ring[i]->gpoint.y + ring[i+1]->gpoint.y);
                result += tmp1 * tmp2;
            }
            return result;
        };

    static const auto isInnerRing =
        [](const std::vector<VertexIterator>& innerRing, const std::vector<VertexIterator>& outerRing) {
            // NB: Doubling all coordinates to ensure that edge center is integer
            // NB: This is possible due to the fact that all coordinates are less than 2^61
            const auto innerRingPoint = GridPoint{
                .x = innerRing[0]->gpoint.x + innerRing[1]->gpoint.x,
                .y = innerRing[0]->gpoint.y + innerRing[1]->gpoint.y};

            int index = -1;

            for(size_t i = 0; i + 1 < outerRing.size(); ++i) {
                index = updateGridPointIndex(innerRingPoint,
                    GridPoint{2 * outerRing[i]->gpoint.x, 2 * outerRing[i]->gpoint.y},
                    GridPoint{2 * outerRing[i+1]->gpoint.x, 2 * outerRing[i+1]->gpoint.y},
                    index);

                if (!index) {
                    throw Exception("Error in makeGeometryFromRings: inner-outer ring overlap. Check input geometries.");
                }
            }

            return index > 0;
        };

    std::vector<std::pair<size_t, int128_t>> inners;
    std::vector<std::pair<size_t, int128_t>> outers;

    for(size_t i = 0; i < rings.size(); ++i) {
        if (rings[i].size() < 4) {
            // skipping invalid ring
            throw Exception("Error in makeGeometryFromRings: invalid ring. Check input geometries.");
        }
        const auto area = gridArea(rings[i]);
        if (area > 0) {
            // counter-clockwise ring
            outers.push_back(std::make_pair(i, area));
        } else if (area < 0) {
            // clockwise ring
            inners.push_back(std::make_pair(i, -area));
        } else {
            // skipping invalid ring
            throw Exception("Error in makeGeometryFromRings: invalid ring. Check input geometries.");
        }
    }

    std::stable_sort(inners.begin(), inners.end(),
        [](const auto& lhs, const auto& rhs) {
            return lhs.second < rhs.second;
        });

    std::stable_sort(outers.begin(), outers.end(),
        [](const auto& lhs, const auto& rhs) {
            return lhs.second < rhs.second;
        });

    if (outers.empty()) {
        // In fact, this is an error.
        throw Exception("Error in makeGeometryFromRings: missing outer rings. Check input geometries.");
    }

    MultiPolygon<Point> result;

    // Traversing outer rings in the order of increasing area
    for(const auto& outer : outers) {
        const auto& outerRing = rings[outer.first];

        Polygon<Point> poly;
        for(auto vIter : outerRing) {
            poly.outer.points.push_back(vIter->point);
        }

        for(auto& inner : inners) {
            if (inner.first >= rings.size()) {
                // Inner ring is occupied
                continue;
            }

            if (inner.second >= outer.second) {
                // Inner ring is greater than the outer ring
                continue;
            }

            const auto& innerRing = rings[inner.first];
            if (!isInnerRing(innerRing, outerRing)) {
                // Inner ring is not inside the outer ring
                continue;
            }

            Ring<Point> polyInner;
            for(auto vIter : innerRing) {
                polyInner.points.push_back(vIter->point);
            }

            poly.inners.push_back(std::move(polyInner));
            inner.first = SIZE_MAX; // Mark the inner ring as occupied
        }

        result.parts.push_back(std::move(poly));
    }

    // Check that all inner rings are occupied
    for(const auto& inner : inners) {
        if (inner.first < rings.size()) {
            throw Exception("Error in makeGeometryFromRings: found inner rings outside the outers. Check input geometries.");
        }
    }

    detail::correct(result);

    return result;
}

enum class BinaryOperationType {
    UNION = 0,
    INTERSECTION = 1,
    DIFFERENCE = 2,
};

template <typename Geometry1, typename Geometry2>
MultiPolygon<typename Geometry1::point_type> binaryOperation(
    BinaryOperationType op,
    const Geometry1& geometry1,
    const Geometry2& geometry2)
{
#ifdef GEOLIB_DEBUG_ENABLED
    std::cout << "binaryOperation: " << static_cast<int>(op) << "\n";
    std::cout << toWkt(geometry1, 20) << "\n";
    std::cout << toWkt(geometry2, 20) << "\n";
#endif

    static_assert(isBox<Geometry1>() || isRing<Geometry1>() || isPolygon<Geometry1>() || isMultiPolygon<Geometry1>() || isGeometry<Geometry1>());
    static_assert(isBox<Geometry2>() || isRing<Geometry2>() || isPolygon<Geometry2>() || isMultiPolygon<Geometry2>() || isGeometry<Geometry2>());
    static_assert(std::is_same<typename Geometry1::point_type, typename Geometry2::point_type>::value == true);

    using Point = typename Geometry1::point_type;
    using VertexIterator = typename Graph<Point>::VertexIterator;
    using EdgeIterator = typename Graph<Point>::EdgeIterator;
    using TId = typename Graph<Point>::TId;
    using TColor = typename Graph<Point>::TColor;
    using Edge = typename Graph<Point>::Edge;

    static const auto COLOR1 = TColor(0);
    static const auto COLOR2 = TColor(1);

    Graph<Point> graph(2);

    iterateEdges(geometry1, false,
        [&](const Point& p1, const Point& p2) {
            auto srcIter = graph.vertexAdd(p1);
            auto dstIter = graph.vertexAdd(p2);
            if (srcIter != dstIter) {
                graph.edgeAdd(Edge{
                    .color = COLOR1,
                    .src = srcIter->id,
                    .dst = dstIter->id,
                    .srcIter = srcIter,
                    .dstIter = dstIter});
            }
            return true;
        });

    iterateEdges(geometry2, op == BinaryOperationType::DIFFERENCE,
        [&](const Point& p1, const Point& p2) {
            auto srcIter = graph.vertexAdd(p1);
            auto dstIter = graph.vertexAdd(p2);
            if (srcIter != dstIter) {
                graph.edgeAdd(Edge{
                    .color = COLOR2,
                    .src = srcIter->id,
                    .dst = dstIter->id,
                    .srcIter = srcIter,
                    .dstIter = dstIter});
            }
            return true;
        });

#ifdef GEOLIB_DEBUG_ENABLED
    std::cout << graph << "\n";
#endif

    graph.crossEdges(COLOR1, COLOR2);
    graph.paintVertices();

//#ifdef GEOLIB_DEBUG_ENABLED
//    std::cout << graph << "\n";
//#endif

    static const auto filterEdge =
        [](EdgeType type, TColor color, BinaryOperationType op) {
            switch (op) {
                case BinaryOperationType::UNION:
                    return type == EdgeType::OUTER || (type == EdgeType::BORDER && color == COLOR1);
                case BinaryOperationType::INTERSECTION:
                    return type == EdgeType::INNER || (type == EdgeType::BORDER && color == COLOR1);
                case BinaryOperationType::DIFFERENCE:
                    return (type == EdgeType::OUTER && color == COLOR1) ||
                           (type == EdgeType::INNER && color == COLOR2) ||
                           (type == EdgeType::BORDER && color == COLOR1);
                default:
                    return false;
            }
        };

    std::vector<EdgeIterator> edgesToRemove;

    static const auto COLOR_PAIRS = std::vector<std::pair<TColor, TColor>>{
        {COLOR1, COLOR2},
        {COLOR2, COLOR1},
    };

    for(auto colors : COLOR_PAIRS) {
        std::optional<Edge> prevEdge;
        std::optional<EdgeType> prevType;
        graph.iterateEdgesByColor(colors.first,
            [&](auto eIter) {
                auto type = (prevEdge && prevEdge->dst == eIter->src && eIter->srcIter->color == colors.first) ?
                    *prevType : getEdgeType(graph, *eIter, colors.second);
                if (!filterEdge(type, colors.first, op)) {
                    edgesToRemove.push_back(eIter);
                }
                prevEdge = *eIter;
                prevType = type;
            });
    }

    for(auto eIter : edgesToRemove) {
#ifdef GEOLIB_DEBUG_ENABLED
        std::cout << "Filtering out edge [color=" << eIter->color << ", src=" << eIter->src << ", dst=" << eIter->dst << "]\n";
#endif
        graph.edgeRemove(eIter);
    }

    std::vector<std::vector<VertexIterator>> rings;

    while (graph.edgeBegin() != graph.edgeEnd()) {
        std::vector<VertexIterator> ipath;
        std::unordered_set<TId> vids;

        auto eIter = graph.edgeBegin();
        ipath.push_back(eIter->srcIter);
        vids.insert(eIter->src);

        while (eIter != graph.edgeEnd()) {
            const auto dst = eIter->dst;
            ipath.push_back(eIter->dstIter);

            if (!vids.insert(eIter->dst).second) {
                auto startIter = std::find(ipath.begin(), ipath.end(), ipath.back());
                assert(std::next(startIter) != ipath.end());

#ifdef GEOLIB_DEBUG_ENABLED
                std::cout << "ipath: ";
                for(auto vIter : ipath) {
                    std::cout << vIter->id << " ";
                }
                std::cout << "\n";
#endif

                std::vector<VertexIterator> ring;
                for(auto iter = startIter; iter != ipath.end(); ++iter) {
                    ring.push_back(*iter);
                }
                rings.push_back(std::move(ring));

                ipath.erase(std::next(startIter), ipath.end());
                vids.clear();
                for(const auto& iter : ipath) {
                    vids.insert(iter->id);
                }
            }
            graph.edgeRemove(eIter);
            eIter = graph.edgeFind(std::nullopt, dst);
        }

        if (ipath.size() >= 2) {
#ifdef GEOLIB_DEBUG_ENABLED
            std::cout << "incomplete ring: ";
            for(auto vIter : ipath) {
                std::cout << vIter->id << " ";
            }
            std::cout << "\n";
#endif
            throw Exception("Error in binaryOperation: incomplete ring. Check input geometries.");
        }
    }

    return makeGeometryFromRings<Point, VertexIterator>(rings);
}

} // namespace GEOLIB_NAMESPACE::detail
