#pragma once

#include <memory>

#include "models.h"
#include "detail/spatial_search_tree.h"

namespace GEOLIB_NAMESPACE {

template <typename Point>
class SpatialSearchTree
{
public:
    // NB: throws exception if geometry is not valid
    SpatialSearchTree(const MultiPolygon<Point>& geom)
        : root_(detail::buildSST<Point>(geom, 0))
    {}

    void addGeometry(const MultiPolygon<Point>& geom) {
        detail::sstAddGeometry(root_, geom, 0);
    }

    bool containsPoint(const Point& point) const {
        return detail::sstContainsPoint<Point>(root_.get(), point);
    }

    bool containsSegment(const Segment<Point>& segment) const {
        return detail::sstContainsSegment<Point>(root_.get(), segment);
    }

    void print(std::ostream& os) const {
        return detail::sstPrint(root_.get(), os, 0);
    }

private:
    std::unique_ptr<detail::NodeType<Point>> root_;
};

} // namespace GEOLIB_NAMESPACE
