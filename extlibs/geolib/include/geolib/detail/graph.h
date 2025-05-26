#pragma once

#include "compare.h"
#include "contains.h"
#include "grid_point.h"
#include "point_helper.h"
#include "type_checks.h"
#include "util.h"

namespace GEOLIB_NAMESPACE::detail {

template <typename Point>
class Graph
{
public:
    using TId = unsigned int;
    using TColor = unsigned int;

    static const auto GRID_MAX_BITS = 53;
    static const auto GRID_MAX_VALUE = 1LL << GRID_MAX_BITS;

    struct Vertex
    {
        TId id;
        Point point;
        GridPoint gpoint;

        std::optional<TColor> color = std::nullopt;

        inline bool operator==(const Vertex& v) const {
            return id == v.id;
        }

        inline bool operator<(const Vertex& v) const {
            return id < v.id;
        }
    };

    using VertexIterator = typename std::set<Vertex>::const_iterator;

    struct Edge
    {
        TColor color;
        TId src;
        TId dst;
        VertexIterator srcIter;
        VertexIterator dstIter;

        inline bool operator==(const Edge& e) const {
            return color == e.color && src == e.src && dst == e.dst;
        }

        inline bool operator<(const Edge& e) const {
            return (color < e.color) ||
                   (color == e.color && src < e.src) ||
                   (color == e.color && src == e.src && dst < e.dst);
        }
    };

    using EdgeIterator = typename std::set<Edge>::const_iterator;
    using EdgeFunction = std::function<void(EdgeIterator)>;

    inline Graph(TColor maxColors)
        : maxColors_(maxColors)
        , gridFactorX_(getGridFactor(util::maxX<Point>()))
        , gridFactorY_(getGridFactor(util::maxY<Point>()))
        , nextId_(0)
    {}

    inline TColor maxColors() const {
        return maxColors_;
    }

    inline size_t vertexCount() const {
        return vertices_.size();
    }

    inline VertexIterator vertexAdd(const Point& point) {
        const auto x = util::getX(point);
        const auto y = util::getY(point);
        if (!std::isfinite(x) || std::fabs(x) > util::maxX<Point>() ||
            !std::isfinite(y) || std::fabs(y) > util::maxY<Point>()) {
            throw Exception(GEOLIB_FORMAT("Invalid point coordinates, should be within bounds "
                "[-" << util::maxX<Point>() << ", " << util::maxX<Point>() << "] x "
                "[-" << util::maxY<Point>() << ", " << util::maxY<Point>() << "]"));
        }
        return vertexAdd(GridPoint{
            .x = static_cast<int64_t>(std::round(x * gridFactorX_)),
            .y = static_cast<int64_t>(std::round(y * gridFactorY_))});
    }

    inline VertexIterator vertexBegin() const {
        return vertices_.begin();
    }

    inline VertexIterator vertexEnd() const {
        return vertices_.end();
    }

    inline size_t edgeCount() const {
        return edges_.size();
    }

    inline EdgeIterator edgeAdd(const Edge& e) {
        assert(e.color < maxColors_);
        assert(e.src != e.dst);
        return edges_.insert(e).first;
    }

    inline void edgeRemove(EdgeIterator iter) {
        edges_.erase(iter);
    }

    inline EdgeIterator edgeBegin() const {
        return edges_.begin();
    }

    inline EdgeIterator edgeEnd() const {
        return edges_.end();
    }

    inline EdgeIterator edgeFind(TColor color) const {
        auto iter = edges_.lower_bound(makeFakeEdge(color, 0, 0));
        if (iter != edges_.end() && iter->color == color) {
            return iter;
        }
        return edges_.end();
    }

    inline EdgeIterator edgeFind(std::optional<TColor> color, TId src) const {
        if (color) {
            auto iter = edges_.lower_bound(makeFakeEdge(*color, src, 0));
            if (iter != edges_.end() && iter->color == *color && iter->src == src) {
                return iter;
            }
        } else {
            // Any color
            for(TColor color = 0; color < maxColors_; ++color) {
                auto iter = edges_.lower_bound(makeFakeEdge(color, src, 0));
                if (iter != edges_.end() && iter->src == src) {
                    return iter;
                }
            }
        }
        return edges_.end();
    }

    inline EdgeIterator edgeFind(std::optional<TColor> color, TId src, TId dst) const {
        if (color) {
            auto iter = edges_.lower_bound(makeFakeEdge(*color, src, dst));
            if (iter != edges_.end() && iter->color == *color && iter->src == src && iter->dst == dst) {
                return iter;
            }
        } else {
            // Any color
            for(TColor color = 0; color < maxColors_; ++color) {
                auto iter = edges_.lower_bound(makeFakeEdge(color, src, dst));
                if (iter != edges_.end() && iter->src == src && iter->dst == dst) {
                    return iter;
                }
            }
        }
        return edges_.end();
    }

    inline int edgeIndex(const Edge& e, TColor color) const {
        // NB: Doubling all coordinates to ensure that edge center is integer
        // NB: This is possible due to the fact that all coordinates are less than 2^61
        const auto point = GridPoint{
            .x = e.srcIter->gpoint.x + e.dstIter->gpoint.x,
            .y = e.srcIter->gpoint.y + e.dstIter->gpoint.y};

        int index = -1;

        for(auto eIter = edgeFind(color); eIter != edges_.end() && eIter->color == color && index != 0; ++eIter) {
            const auto A = GridPoint{
                .x = 2 * eIter->srcIter->gpoint.x,
                .y = 2 * eIter->srcIter->gpoint.y};
            const auto B = GridPoint{
                .x = 2 * eIter->dstIter->gpoint.x,
                .y = 2 * eIter->dstIter->gpoint.y};
            index = updateGridPointIndex(point, A, B, index);
        }

        return index;
    }

    inline void iterateEdgesByColor(TColor color, const EdgeFunction& func) const {
        auto iter = edges_.lower_bound(makeFakeEdge(color, 0, 0));
        while (iter != edges_.end() && iter->color == color) {
            func(iter++);
        }
    }

    inline void crossEdges(TColor color1, TColor color2) {
        std::vector<EdgeIterator> edges1;
        std::vector<EdgeIterator> edges2;
        std::vector<size_t> state1;   // Number of checks for the edge or SIZE_MAX if edge is deleted
        std::vector<size_t> state2;   // Number of checks for the edge or SIZE_MAX if edge is deleted

        int64_t xMin1 = std::numeric_limits<int64_t>::max();
        int64_t xMax1 = std::numeric_limits<int64_t>::min();
        int64_t yMin1 = std::numeric_limits<int64_t>::max();
        int64_t yMax1 = std::numeric_limits<int64_t>::min();

        int64_t xMin2 = std::numeric_limits<int64_t>::max();
        int64_t xMax2 = std::numeric_limits<int64_t>::min();
        int64_t yMin2 = std::numeric_limits<int64_t>::max();
        int64_t yMax2 = std::numeric_limits<int64_t>::min();

        iterateEdgesByColor(color1,
            [&](EdgeIterator eIter) {
                xMin1 = std::min(xMin1, eIter->srcIter->gpoint.x);
                xMax1 = std::max(xMax1, eIter->srcIter->gpoint.x);
                yMin1 = std::min(yMin1, eIter->srcIter->gpoint.y);
                yMax1 = std::max(yMax1, eIter->srcIter->gpoint.y);
            });

        iterateEdgesByColor(color2,
            [&](EdgeIterator eIter) {
                xMin2 = std::min(xMin2, eIter->srcIter->gpoint.x);
                xMax2 = std::max(xMax2, eIter->srcIter->gpoint.x);
                yMin2 = std::min(yMin2, eIter->srcIter->gpoint.y);
                yMax2 = std::max(yMax2, eIter->srcIter->gpoint.y);
            });

        iterateEdgesByColor(color1,
            [&](EdgeIterator eIter) {
                if ((eIter->srcIter->gpoint.x < xMin2 && eIter->dstIter->gpoint.x < xMin2) ||
                    (eIter->srcIter->gpoint.x > xMax2 && eIter->dstIter->gpoint.x > xMax2) ||
                    (eIter->srcIter->gpoint.y < yMin2 && eIter->dstIter->gpoint.y < yMin2) ||
                    (eIter->srcIter->gpoint.y > yMax2 && eIter->dstIter->gpoint.y > yMax2)) {
                    return;
                }
                edges1.push_back(eIter);
                state1.push_back(0);
            });

        iterateEdgesByColor(color2,
            [&](EdgeIterator eIter) {
                if ((eIter->srcIter->gpoint.x < xMin1 && eIter->dstIter->gpoint.x < xMin1) ||
                    (eIter->srcIter->gpoint.x > xMax1 && eIter->dstIter->gpoint.x > xMax1) ||
                    (eIter->srcIter->gpoint.y < yMin1 && eIter->dstIter->gpoint.y < yMin1) ||
                    (eIter->srcIter->gpoint.y > yMax1 && eIter->dstIter->gpoint.y > yMax1)) {
                    return;
                }
                edges2.push_back(eIter);
                state2.push_back(0);
            });

        size_t index1 = 0;

        auto getNextPair =
            [&]() -> std::optional<std::pair<size_t, size_t>> {
                for(; index1 < state1.size(); ++index1) {
                    while (state1[index1] < state2.size()) {
                        if (state2[state1[index1]] == SIZE_MAX) {
                            ++state1[index1];
                            continue;
                        }
                        return std::make_pair(index1, state1[index1]++);
                    }
                }
                return std::nullopt;
            };

        const auto splitByOne =
            [&](size_t eIndex, TColor color, const GridPoint& i) {
                auto eIter = color == color1 ? edges1[eIndex] : edges2[eIndex];
                auto vIter = vertexAdd(i);
                assert(eIter->color == color);
                assert(vIter->id != eIter->src && vIter->id != eIter->dst);
#ifdef GEOLIB_DEBUG_ENABLED
                std::cout << " - adding point i1=(" << i.x << ", " << i.y << ") => Vertex[" << vIter->id << "], Point(" << util::getX(vIter->point) << ", " << util::getY(vIter->point) << ")\n";
                std::cout << " - adding edge: " << eIter->srcIter->id << "->" << vIter->id << " (color=" << color << ")\n";
                std::cout << " - adding edge: " << vIter->id << "->" << eIter->dstIter->id << " (color=" << color << ")\n";
                std::cout << " - removing edge: " << eIter->srcIter->id << "->" << eIter->dstIter->id << " (color=" << color << ")\n";
#endif
                auto eIter1 = edgeAdd(Edge{
                    .color = color,
                    .src = eIter->srcIter->id,
                    .dst = vIter->id,
                    .srcIter = eIter->srcIter,
                    .dstIter = vIter});
                auto eIter2 = edgeAdd(Edge{
                    .color = color,
                    .src = vIter->id,
                    .dst = eIter->dstIter->id,
                    .srcIter = vIter,
                    .dstIter = eIter->dstIter});

                edges_.erase(eIter);

                if (color == color1) {
                    edges1.push_back(eIter1);
                    edges1.push_back(eIter2);
                    state1.push_back(0);
                    state1.push_back(0);
                    state1[eIndex] = SIZE_MAX;
                } else {
                    edges2.push_back(eIter1);
                    edges2.push_back(eIter2);
                    state2.push_back(0);
                    state2.push_back(0);
                    state2[eIndex] = SIZE_MAX;
                    index1 = 0;
                }
            };

        const auto splitByTwo =
            [&](size_t eIndex, TColor color, const GridPoint& i1, const GridPoint& i2) {
                auto eIter = color == color1 ? edges1[eIndex] : edges2[eIndex];
                auto vIter1 = vertexAdd(i1);
                auto vIter2 = vertexAdd(i2);
                assert(eIter->color == color);
                assert(vIter1->id != eIter->src && vIter1->id != eIter->dst);
                assert(vIter2->id != eIter->src && vIter2->id != eIter->dst);
                assert(vIter1->id != vIter2->id);
#ifdef GEOLIB_DEBUG_ENABLED
                std::cout << " - adding point i1=(" << i1.x << ", " << i1.y << ") => Vertex[" << vIter1->id << "], Point(" << util::getX(vIter1->point) << ", " << util::getY(vIter1->point) << ")\n";
                std::cout << " - adding point i2=(" << i2.x << ", " << i2.y << ") => Vertex[" << vIter2->id << "], Point(" << util::getX(vIter2->point) << ", " << util::getY(vIter2->point) << ")\n";
                std::cout << " - adding edge: " << eIter->srcIter->id << "->" << vIter1->id << " (color=" << color << ")\n";
                std::cout << " - adding edge: " << vIter1->id << "->" << vIter2->id << " (color=" << color << ")\n";
                std::cout << " - adding edge: " << vIter2->id << "->" << eIter->dstIter->id << " (color=" << color << ")\n";
                std::cout << " - removing edge: " << eIter->srcIter->id << "->" << eIter->dstIter->id << " (color=" << color << ")\n";
#endif
                auto eIter1 = edgeAdd(Edge{
                    .color = color,
                    .src = eIter->srcIter->id,
                    .dst = vIter1->id,
                    .srcIter = eIter->srcIter,
                    .dstIter = vIter1});
                auto eIter2 = edgeAdd(Edge{
                    .color = color,
                    .src = vIter1->id,
                    .dst = vIter2->id,
                    .srcIter = vIter1,
                    .dstIter = vIter2});
                auto eIter3 = edgeAdd(Edge{
                    .color = color,
                    .src = vIter2->id,
                    .dst = eIter->dstIter->id,
                    .srcIter = vIter2,
                    .dstIter = eIter->dstIter});

                edges_.erase(eIter);

                if (color == color1) {
                    edges1.push_back(eIter1);
                    edges1.push_back(eIter2);
                    edges1.push_back(eIter3);
                    state1.push_back(0);
                    state1.push_back(0);
                    state1.push_back(0);
                    state1[eIndex] = SIZE_MAX;
                } else {
                    edges2.push_back(eIter1);
                    edges2.push_back(eIter2);
                    edges2.push_back(eIter3);
                    state2.push_back(0);
                    state2.push_back(0);
                    state2.push_back(0);
                    state2[eIndex] = SIZE_MAX;
                    index1 = 0;
                }
            };

        while (auto p = getNextPair()) {
            const auto eIter1 = edges1[p->first];
            const auto eIter2 = edges2[p->second];
            const auto& p1 = eIter1->srcIter->gpoint;
            const auto& p2 = eIter1->dstIter->gpoint;
            const auto& q1 = eIter2->srcIter->gpoint;
            const auto& q2 = eIter2->dstIter->gpoint;
            GridPoint i1, i2;

            const int count = gridSegmentIntersection(p1, p2, q1, q2, i1, i2);

            if (count == 0) {
                continue;
            }

#ifdef GEOLIB_DEBUG_ENABLED
            std::cout << " cross: " << eIter1->src << "->" << eIter1->dst << " and "
                      << eIter2->src << "->" << eIter2->dst << ": count=" << count << std::endl;
#endif

            if (count == 1) {
                if (i1 != p1 && i1 != p2) {
                    splitByOne(p->first, color1, i1);
                }
                if (i1 != q1 && i1 != q2) {
                    splitByOne(p->second, color2, i1);
                }
            }

            if (count == 2) {
                if ((p1 < i1 && i1 < i2 && i2 < p2) || (p1 > i1 && i1 > i2 && i2 > p2)) {
                    splitByTwo(p->first, color1, i1, i2);
                } else if ((p1 < i2 && i2 < i1 && i1 < p2) || (p1 > i2 && i2 > i1 && i1 > p2)) {
                    splitByTwo(p->first, color1, i2, i1);
                } else if ((i1 != p1 && i1 != p2) && (i2 == p1 || i2 == p2)) {
                    splitByOne(p->first, color1, i1);
                } else if ((i1 == p1 || i1 == p2) && (i2 != p1 && i2 != p2)) {
                    splitByOne(p->first, color1, i2);
                }

                if ((q1 < i1 && i1 < i2 && i2 < q2) || (q1 > i1 && i1 > i2 && i2 > q2)) {
                    splitByTwo(p->second, color2, i1, i2);
                } else if ((q1 < i2 && i2 < i1 && i1 < q2) || (q1 > i2 && i2 > i1 && i1 > q2)) {
                    splitByTwo(p->second, color2, i2, i1);
                } else if ((i1 != q1 && i1 != q2) && (i2 == q1 || i2 == q2)) {
                    splitByOne(p->second, color2, i1);
                } else if ((i1 == q1 || i1 == q2) && (i2 != q1 && i2 != q2)) {
                    splitByOne(p->second, color2, i2);
                }
            }
        }
    }

    inline void paintVertices() {
        for(const auto& e : edges_) {
            auto& src = const_cast<Vertex&>(*e.srcIter);
            auto& dst = const_cast<Vertex&>(*e.dstIter);
            if (!src.color) {
                src.color = e.color;
            } else if (src.color != e.color) {
                src.color = std::numeric_limits<TColor>::max();
            }
            if (!dst.color) {
                dst.color = e.color;
            } else if (dst.color != e.color) {
                dst.color = std::numeric_limits<TColor>::max();
            }
        }
    }

private:
    inline Edge makeFakeEdge(TColor color, TId src, TId dst) const {
        return Graph::Edge{
            .color = color,
            .src = src,
            .dst = dst,
            .srcIter = {},
            .dstIter = {}};
    }

    inline VertexIterator vertexAdd(const GridPoint& gpoint) {
        assert(std::llabs(gpoint.x) <= GRID_MAX_VALUE);
        assert(std::llabs(gpoint.y) <= GRID_MAX_VALUE);

        const auto iter = gpointMap_.find(gpoint);
        if (iter != gpointMap_.end()) {
            return iter->second;
        }

        Point point;
        util::getX(point) = static_cast<double>(gpoint.x) / gridFactorX_;
        util::getY(point) = static_cast<double>(gpoint.y) / gridFactorY_;

        const auto vIter = vertices_.insert(Vertex{
            .id = nextId_++,
            .point = point,
            .gpoint = gpoint}).first;

        gpointMap_[gpoint] = vIter;

        return vIter;
    }

    inline double getGridFactor(double max) {
        assert(std::isfinite(max) && max > 0);
        int exponent = 0;
        std::frexp(max, &exponent);
        return std::exp2(GRID_MAX_BITS - exponent); // Should be exact
    }

    struct Hasher {
        inline std::size_t operator()(const GridPoint& point) const {
            const std::size_t h1 = std::hash<int64_t>()(point.x);
            const std::size_t h2 = std::hash<int64_t>()(point.y);
            return h1 ^ (h2 + 0x9e3779b9 + (h1<<6) + (h1>>2));
        }
    };

    const TColor maxColors_;
    const double gridFactorX_;
    const double gridFactorY_;

    std::set<Vertex> vertices_;
    std::set<Edge> edges_;
    std::unordered_map<GridPoint, VertexIterator, Hasher> gpointMap_;
    TId nextId_;
};

template <typename Point>
std::ostream& operator<<(std::ostream& os, const Graph<Point>& graph)
{
    os << "Vertices:\n";
    for(auto iter = graph.vertexBegin(); iter != graph.vertexEnd(); ++iter) {
        os << "  Vertex[" << iter->id << "] = "
           << "Point(" << util::getX(iter->point) << ", " << util::getY(iter->point) << ") = "
           << "GridPoint(" << iter->gpoint.x << ", " << iter->gpoint.y << ")\n";
    }
    os << "Edges:\n";
    for(auto iter = graph.edgeBegin(); iter != graph.edgeEnd(); ++iter) {
        os << "  Edge[color=" << iter->color << ", src=" << iter->src << ", dst=" << iter->dst << "]\n";
    }
    return os;
}

} // namespace GEOLIB_NAMESPACE::detail
