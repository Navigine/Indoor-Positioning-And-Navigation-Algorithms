/** location_point.h
 *
 * Copyright (c) 2018 Navigine.
 *
 */

#ifndef NAVIGINE_LOCATION_POINT_H
#define NAVIGINE_LOCATION_POINT_H

#include <vector>
#include "level.h"

namespace navigine {
namespace navigation_core {

struct LocationPoint
{
  LevelId level;
  double x;
  double y;
};

typedef std::vector<LocationPoint> PolyLine;

} } // namespace navigine::navigation_core

#endif
