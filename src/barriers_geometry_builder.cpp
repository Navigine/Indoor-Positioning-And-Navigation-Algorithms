/** barriers_geometry_builder.cpp
 *
 * Copyright (c) 2019 Navigine.
 *
 */
#include <iostream>
#include "barriers_geometry_builder.h"

namespace navigine {
namespace navigation_core {

//TODO test for empty barrier list
LevelGeometry getGeometry(const std::list<Polygon>& allowedArea)
{
  Multipolygon allowedFullArea;
  for (const auto& area: allowedArea)
  {
    Polygon correctedArea = area;
    boost::geometry::correct(correctedArea);
    allowedFullArea.push_back(correctedArea);
  }

  return {allowedFullArea, boost::geometry::return_envelope<Box>(allowedFullArea)};
}

} } // namespace navigine::navigation_core
