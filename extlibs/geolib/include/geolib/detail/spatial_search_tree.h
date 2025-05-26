#pragma once

#include "contains.h"
#include "num_points.h"
#include "precision.h"
#include "util.h"
#include "valid.h"
#include "wkt_write.h"

//#define GEOLIB_SST_DEBUG_ENABLED

#ifdef GEOLIB_SST_DEBUG_ENABLED
# include <iostream>
#endif

namespace GEOLIB_NAMESPACE::detail {

namespace {

const unsigned int MAX_TREE_DEPTH = 24;
const unsigned int GEOM_COMPLEXITY_THRESHOLD = 12;
const double REBALANCE_THRESHOLD = 2.0;

template <typename Point>
inline double paddingX()
{
    static const auto result = 1024 * precisionX<Point>();
    return result;
}

template <typename Point>
inline double paddingY()
{
    static const auto result = 1024 * precisionY<Point>();
    return result;
}

} // namespace

enum class Direction {
    NONE, X, Y,
};

template <typename Point>
struct NodeType {
    Box<Point> bbox;
    MultiPolygon<Point> geom;
    Direction splitDirection;
    std::optional<double> splitValue;
    std::unique_ptr<NodeType<Point>> minNode;
    std::unique_ptr<NodeType<Point>> maxNode;
};

template <typename Point>
std::unique_ptr<NodeType<Point>> buildSST(
    const MultiPolygon<Point>& geom,
    unsigned int depth)
{
    using Node = NodeType<Point>;

    std::string message;
    if (!detail::isValid(geom, message)) {
        throw Exception("Error building SpatialSearchTree: geometry is not valid: " + message);
    }

    const auto size = detail::numPoints(geom);

    if (!size) {
        // Empty geometry, making null node
        return nullptr;
    }

    const auto bbox = boundingBox(geom);

    if (depth >= MAX_TREE_DEPTH || size < GEOM_COMPLEXITY_THRESHOLD) {
        // Don't need to divide geometry
        return std::make_unique<Node>(Node{
            .bbox = bbox,
            .geom = geom,
            .splitDirection = Direction::NONE,
            .splitValue = std::nullopt,
            .minNode = nullptr,
            .maxNode = nullptr});
    }

    const auto xmin = util::getX(bbox.minCorner);
    const auto ymin = util::getY(bbox.minCorner);
    const auto xmax = util::getX(bbox.maxCorner);
    const auto ymax = util::getY(bbox.maxCorner);
    const auto xmid = (xmin + xmax) / 2;
    const auto ymid = (ymin + ymax) / 2;
    const auto width  = xmax - xmin;
    const auto height = ymax - ymin;

    Box<Point> boxMin;
    Box<Point> boxMax;

    if (width > height) {
        // Dividing by X
        util::getX(boxMin.minCorner) = xmin - paddingX<Point>();
        util::getY(boxMin.minCorner) = ymin - paddingY<Point>();
        util::getX(boxMin.maxCorner) = xmid + paddingX<Point>();
        util::getY(boxMin.maxCorner) = ymax + paddingY<Point>();

        util::getX(boxMax.minCorner) = xmid - paddingX<Point>();
        util::getY(boxMax.minCorner) = ymin - paddingY<Point>();
        util::getX(boxMax.maxCorner) = xmax + paddingX<Point>();
        util::getY(boxMax.maxCorner) = ymax + paddingY<Point>();

        return std::make_unique<Node>(Node{
            .bbox = bbox,
            .geom = geom,
            .splitDirection = Direction::X,
            .splitValue = xmid,
            .minNode = buildSST<Point>(intersection(geom, boxMin), depth + 1),
            .maxNode = buildSST<Point>(intersection(geom, boxMax), depth + 1)});
    } else {
        // Dividing by Y
        util::getX(boxMin.minCorner) = xmin - paddingX<Point>();
        util::getY(boxMin.minCorner) = ymin - paddingY<Point>();
        util::getX(boxMin.maxCorner) = xmax + paddingX<Point>();
        util::getY(boxMin.maxCorner) = ymid + paddingY<Point>();

        util::getX(boxMax.minCorner) = xmin - paddingX<Point>();
        util::getY(boxMax.minCorner) = ymid - paddingY<Point>();
        util::getX(boxMax.maxCorner) = xmax + paddingX<Point>();
        util::getY(boxMax.maxCorner) = ymax + paddingY<Point>();

        return std::make_unique<Node>(Node{
            .bbox = bbox,
            .geom = geom,
            .splitDirection = Direction::Y,
            .splitValue = ymid,
            .minNode = buildSST<Point>(intersection(geom, boxMin), depth + 1),
            .maxNode = buildSST<Point>(intersection(geom, boxMax), depth + 1)});
    }
}

template <typename Point>
void sstAddGeometry(
    std::unique_ptr<NodeType<Point>>& node,
    const MultiPolygon<Point>& geom,
    unsigned int depth)
{
    if (isEmpty(geom)) {
        return;
    }

    if (!node) {
        node = buildSST<Point>(geom, depth);
        return;
    }

    if (node->splitDirection == Direction::NONE) {
        node = buildSST<Point>(union_(node->geom, geom), depth);
        return;
    }

    // Fixing bounding box
    expand(node->bbox, geom);

    // Fixing geometry
    node->geom = union_(node->geom, geom);

    const auto xmin = util::getX(node->bbox.minCorner);
    const auto ymin = util::getY(node->bbox.minCorner);
    const auto xmax = util::getX(node->bbox.maxCorner);
    const auto ymax = util::getY(node->bbox.maxCorner);

    assert(node->splitValue);

    if (sstRebalanceNeeded(node)) {
#ifdef GEOLIB_SST_DEBUG_ENABLED
        std::cout << "Rebalancing tree of depth " << depth << ", size=" << detail::numPoints(node->geom) << "\n";
#endif
        node = buildSST<Point>(node->geom, depth);
        return;
    }

    Box<Point> boxMax;
    Box<Point> boxMin;

    switch (node->splitDirection) {
        case Direction::X:
            util::getX(boxMin.minCorner) = xmin - paddingX<Point>();
            util::getY(boxMin.minCorner) = ymin - paddingY<Point>();
            util::getX(boxMin.maxCorner) = *node->splitValue + paddingX<Point>();
            util::getY(boxMin.maxCorner) = ymax + paddingY<Point>();

            util::getX(boxMax.minCorner) = *node->splitValue - paddingX<Point>();
            util::getY(boxMax.minCorner) = ymin - paddingY<Point>();
            util::getX(boxMax.maxCorner) = xmax + paddingX<Point>();
            util::getY(boxMax.maxCorner) = ymax + paddingY<Point>();

            sstAddGeometry<Point>(node->minNode, intersection(geom, boxMin), depth + 1);
            sstAddGeometry<Point>(node->maxNode, intersection(geom, boxMax), depth + 1);
            break;

        case Direction::Y:
            util::getX(boxMin.minCorner) = xmin - paddingX<Point>();
            util::getY(boxMin.minCorner) = ymin - paddingY<Point>();
            util::getX(boxMin.maxCorner) = xmax + paddingX<Point>();
            util::getY(boxMin.maxCorner) = *node->splitValue + paddingY<Point>();

            util::getX(boxMax.minCorner) = xmin - paddingX<Point>();
            util::getY(boxMax.minCorner) = *node->splitValue - paddingY<Point>();
            util::getX(boxMax.maxCorner) = xmax + paddingX<Point>();
            util::getY(boxMax.maxCorner) = ymax + paddingY<Point>();

            sstAddGeometry<Point>(node->minNode, intersection(geom, boxMin), depth + 1);
            sstAddGeometry<Point>(node->maxNode, intersection(geom, boxMax), depth + 1);
            break;

        default:
            break;
    }
}

template <typename Point>
bool sstContainsPoint(const NodeType<Point>* root, const Point& point)
{
    if (!root) {
        return false;
    }

    for(auto node = root; node; ) {
        if (!detail::contains(node->bbox, point)) {
            break;
        }
        switch (node->splitDirection) {
            case Direction::NONE:
                return detail::contains(node->geom, point);

            case Direction::X:
                assert(node->splitValue);
                node = util::getX(point) < *node->splitValue ?
                    node->minNode.get() :
                    node->maxNode.get();
                break;

            case Direction::Y:
                assert(node->splitValue);
                node = util::getY(point) < *node->splitValue ?
                    node->minNode.get() :
                    node->maxNode.get();
                break;
        }
    }

    return false;
}

template <typename Point>
bool sstContainsSegment(const NodeType<Point>* root, const Segment<Point>& line)
{
    if (!root) {
        return false;
    }

    using Node = NodeType<Point>;
    using Segment = Segment<Point>;

    struct SearchState {
        const Node* node;
        Segment line;
    };

    std::vector<SearchState> stack;
    stack.emplace_back(SearchState{
        .node = root,
        .line = line});

    while (!stack.empty()) {
        auto& s = stack.back();
        if (!s.node || !detail::contains(s.node->bbox, s.line.first) || !detail::contains(s.node->bbox, s.line.second)) {
            return false;
        }
        switch (s.node->splitDirection) {
            case Direction::NONE:
                if (!detail::contains(s.node->geom, s.line)) {
                    return false;
                }
                stack.pop_back();
                break;

            case Direction::X: {
                assert(s.node->splitValue);
                const auto x1 = util::getX(s.line.first);
                const auto y1 = util::getY(s.line.first);
                const auto x2 = util::getX(s.line.second);
                const auto y2 = util::getY(s.line.second);
                const auto xmid = *s.node->splitValue;

                if (x1 < xmid + paddingX<Point>() && x2 < xmid + paddingX<Point>()) {
                    s.node = s.node->minNode.get();
                } else if (x1 > xmid - paddingX<Point>() && x2 > xmid - paddingX<Point>()) {
                    s.node = s.node->maxNode.get();
                } else {
                    assert(std::fabs(x1 - x2) >= 2 * paddingX<Point>());
                    const auto node = s.node;
                    const auto line = s.line;
                    const auto ymid = y1 + (xmid - x1) / (x2 - x1) * (y2 - y1);

                    // Removing current state, but adding two new states
                    stack.pop_back();

                    Point splitPoint;
                    util::getX(splitPoint) = xmid;
                    util::getY(splitPoint) = ymid;

                    if (x1 < x2) {
                        stack.emplace_back(SearchState{
                            .node = node->minNode.get(),
                            .line = Segment{line.first, splitPoint}});
                        stack.emplace_back(SearchState{
                            .node = node->maxNode.get(),
                            .line = Segment{splitPoint, line.second}});
                    } else {
                        stack.emplace_back(SearchState{
                            .node = node->minNode.get(),
                            .line = Segment{line.second, splitPoint}});
                        stack.emplace_back(SearchState{
                            .node = node->maxNode.get(),
                            .line = Segment{splitPoint, line.first}});
                    }
                }
                break;
            } // case Direction::X

            case Direction::Y: {
                assert(s.node->splitValue);
                const auto x1 = util::getX(s.line.first);
                const auto y1 = util::getY(s.line.first);
                const auto x2 = util::getX(s.line.second);
                const auto y2 = util::getY(s.line.second);
                const auto ymid = *s.node->splitValue;

                if (y1 < ymid + paddingY<Point>() && y2 < ymid + paddingY<Point>()) {
                    s.node = s.node->minNode.get();
                } else if (y1 > ymid - paddingY<Point>() && y2 > ymid - paddingY<Point>()) {
                    s.node = s.node->maxNode.get();
                } else {
                    assert(std::fabs(y1 - y2) >= 2 * paddingY<Point>());
                    const auto node = s.node;
                    const auto line = s.line;
                    const auto xmid = x1 + (ymid - y1) / (y2 - y1) * (x2 - x1);

                    // Removing current state, but adding two new states
                    stack.pop_back();

                    Point splitPoint;
                    util::getX(splitPoint) = xmid;
                    util::getY(splitPoint) = ymid;

                    if (y1 < y2) {
                        stack.emplace_back(SearchState{
                            .node = node->minNode.get(),
                            .line = Segment{line.first, splitPoint}});
                        stack.emplace_back(SearchState{
                            .node = node->maxNode.get(),
                            .line = Segment{splitPoint, line.second}});
                    } else {
                        stack.emplace_back(SearchState{
                            .node = node->minNode.get(),
                            .line = Segment{line.second, splitPoint}});
                        stack.emplace_back(SearchState{
                            .node = node->maxNode.get(),
                            .line = Segment{splitPoint, line.first}});
                    }
                }
                break;
            } // case Direction::Y
        } // switch
    } // while

    return true;
}

template <typename Point>
void sstPrint(const NodeType<Point>* node, std::ostream& os, unsigned int depth)
{
    std::string tab(depth, ' ');

    if (!node) {
        os << tab << "NULL" << std::endl;
        return;
    }

    switch (node->splitDirection) {
        case Direction::NONE:
            os << tab << detail::toWkt(node->geom) << ", size=" << detail::numPoints(node->geom) << ", depth=" << depth << std::endl;
            return;

        case Direction::X:
            os << tab << detail::toWkt(node->bbox) << ", split by X=" << *node->splitValue << ", depth=" << depth << std::endl;
            break;

        case Direction::Y:
            os << tab << detail::toWkt(node->bbox) << ", split by Y=" << *node->splitValue << ", depth=" << depth << std::endl;
            break;
    }

    sstPrint<Point>(node->minNode.get(), os, depth + 1);
    sstPrint<Point>(node->maxNode.get(), os, depth + 1);
}

template <typename Point>
bool sstRebalanceNeeded(const std::unique_ptr<NodeType<Point>>& node)
{
    if (!node || node->splitDirection == Direction::NONE) {
        return false;
    }

    const auto xmin = util::getX(node->bbox.minCorner);
    const auto ymin = util::getY(node->bbox.minCorner);
    const auto xmax = util::getX(node->bbox.maxCorner);
    const auto ymax = util::getY(node->bbox.maxCorner);

    const auto mid = *node->splitValue;

    double imbalanceCoef = node->splitDirection == Direction::X ?
        (mid - xmin) / (xmax - mid) :
        (mid - ymin) / (ymax - mid);

    if (imbalanceCoef < 1.0) {
        imbalanceCoef = 1.0 / imbalanceCoef;
    }

    return imbalanceCoef >= REBALANCE_THRESHOLD;
}

} // GEOLIB_NAMESPACE::detail
