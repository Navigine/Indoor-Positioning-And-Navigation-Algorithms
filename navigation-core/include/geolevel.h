/** geolevel.h
*
* Copyright (c) 2019 Navigine. All rights reserved.
*
*/

#ifndef NAVIGINE_GEOLEVEL_H
#define NAVIGINE_GEOLEVEL_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#include "level_geometry.h"
#include "boost_geometry_adaptation.h"
#include "transmitter.h"

namespace navigine {
namespace navigation_core {

struct GeoLevel
{
  LevelId id;
  Geo3DTransmitters transmitters;
  LevelGeometry geometry;
  double altitude;
};

typedef std::vector<std::shared_ptr<GeoLevel> > GeoLevels;

} } // namespace navigine::navigation_core

#endif // NAVIGINE_GEOLEVEL_H
