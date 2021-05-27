/** level_estimator_transmitters.cpp
*
* Author: Vasily Kosyanchuk <v.kosyanchuk@navigine.com>
* Copyright (c) 2019 Navigine. All rights reserved.
*
*/

#include "level_estimator_transmitters.h"
namespace navigine {
namespace navigation_core {

LevelEstimatorTransmitters::LevelEstimatorTransmitters(
  const std::shared_ptr<LevelCollector>& levelCollector,
  const NavigationSettings& navProps)
    : LevelEstimator(navProps)
    , mLevelIndex(levelCollector)
{
}

static double getAverageRssi(const RadioMeasurementsData& radioMsr)
{
  double averageRssi = 0.0;
  for(int i = 0; i < (int)radioMsr.size(); ++i)
    averageRssi += radioMsr[i].rssi;
  averageRssi /= std::max((int)radioMsr.size(), 1);
  return averageRssi;
}

LevelId LevelEstimatorTransmitters::detectCurrentLevel(const RadioMeasurementsData& radioMsr)
{
  LevelId bestLevelId("");
  double bestAverRssi = -1000.0;
  for (const Level& level : mLevelIndex->levels())
  {
    RadioMeasurementsData levelMsr = getLevelRadioMeasurements(level, radioMsr);
    if (!levelMsr.empty())
    {
      double averageRssi = getAverageRssi(levelMsr);
      // Using weighted average. Weight depends on number of visible Transmitters
      double weight = 1.0 + 0.1 * levelMsr.size() / radioMsr.size();
      averageRssi /= weight;

      if (averageRssi > bestAverRssi)
      {
        bestAverRssi = averageRssi;
        bestLevelId = level.id();
      }
    }
  }

  return bestLevelId;
}

} } // namespace navigine::navigation_core
