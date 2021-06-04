/** level_collector.h
*
* Copyright (c) 2019 Navigine.
*
*/

#ifndef NAVIGINE_LEVEL_COLLECTOR_H
#define NAVIGINE_LEVEL_COLLECTOR_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <boost/optional.hpp>

#include "level.h"
#include "geolevel.h"
#include "transmitter.h"

namespace navigine {
namespace navigation_core {

class LevelCollector
{
public:
  LevelCollector() {}
  bool hasLevel(const LevelId& levelId) const;
  const Level& level(const LevelId& levelId) const;
  void addGeoLevel(const GeoLevel& geoLevel);
  const std::vector<Level>& levels() const;
  boost::optional<LevelId> findLevelByTransmitterId(const TransmitterId& transmitterId) const;

private:
  std::vector<Level> mLevels;
  std::unordered_map<LevelId, std::size_t, HasherLevelId> mLevelsIndices;
};

std::shared_ptr<LevelCollector> createLevelCollector();

} } // namespace navigine::navigation_core

#endif // NAVIGINE_LEVEL_COLLECTOR_H
