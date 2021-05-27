/** level.h
*
* Author: Vasiliy Kosyanchuk <v.kosyanchuk@navigine.com>
* Copyright (c) 2019 Navigine. All rights reserved.
*
*/

#ifndef NAVIGINE_LEVEL_GEOMETRY_H
#define NAVIGINE_LEVEL_GEOMETRY_H

#include "boost_geometry_adaptation.h"
#include <vector>

namespace navigine {
namespace navigation_core {

class LevelGeometry
{
public:
  LevelGeometry() {};
  LevelGeometry(const Multipolygon& allowedArea, const Box& box);

  bool isPathTouchesBarriers(const Linestring& path) const;

  const Multipolygon& allowedArea() const;
  const Box& boundingBox() const;


private:
  Multipolygon mAllowedArea;
  Box mBoundingBox;
  RTree mTree;
  std::vector<Ring> mInnerBarriers;
};

} } // namespace navigine::navigation_core

#endif // NAVIGINE_LEVEL_GEOMETRY_H
