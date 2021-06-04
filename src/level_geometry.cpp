#include <level_collector.h>

namespace navigine {
namespace navigation_core {

namespace { }

LevelGeometry::LevelGeometry(const Multipolygon& allowedArea, const Box& box)
  : mAllowedArea(allowedArea)
  , mBoundingBox(box)
{
  for (const auto& polygon : mAllowedArea)
    for (const auto& inner : polygon.inners())
      mInnerBarriers.push_back(inner);

  std::vector<BoxIdx> boxIndices;
  for (std::size_t i = 0; i < mInnerBarriers.size(); i++)
  {
    boost::geometry::correct(mInnerBarriers[i]);
    boxIndices.push_back({boost::geometry::return_envelope<Box>(mInnerBarriers[i]), i});
  }
  mTree = RTree(boxIndices);
}

bool LevelGeometry::isPathTouchesBarriers(const Linestring& path) const
{
  bool isInsideAllowedArea = false;
  for (const auto& allowedZone : mAllowedArea)
    if (boost::geometry::covered_by(path, allowedZone.outer()))
      isInsideAllowedArea = true;
  if (!isInsideAllowedArea)
    return true;

  std::vector<BoxIdx> intersectedGeometryBoxes;
  mTree.query(boost::geometry::index::intersects(path), std::back_inserter(intersectedGeometryBoxes));

  for (const auto& boxIdxPair : intersectedGeometryBoxes)
  {
    if (boost::geometry::intersects(path, mInnerBarriers[boxIdxPair.second]))
      return true;
  }

  return false;
}

const Multipolygon& LevelGeometry::allowedArea() const
{
  return mAllowedArea;
}

const Box& LevelGeometry::boundingBox() const
{
  return mBoundingBox;
}

} } // namespace navigine::navigation_core