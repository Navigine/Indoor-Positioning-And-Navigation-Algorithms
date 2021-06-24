/** geolevel.h
*
* Copyright (c) 2019 Navigine.
*
*/

#ifndef NAVIGINE_GEOLEVEL_H
#define NAVIGINE_GEOLEVEL_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#include "level.h"
#include "level_geometry.h"
#include "boost_geometry_adaptation.h"
#include "graph.h"
#include "reference_point.h"
#include "transmitter.h"

namespace navigine {
namespace navigation_core {

struct GeoLevel
{
  LevelId id;
  Geo3DTransmitters transmitters;
  GeoReferencePoints referencePoints;
  LevelGeometry geometry;
  Graph<GeoPoint> graph;
  double altitude;
};

typedef std::vector<std::shared_ptr<GeoLevel> > GeoLevels;

} } // namespace navigine::navigation_core

#endif // NAVIGINE_GEOLEVEL_H
