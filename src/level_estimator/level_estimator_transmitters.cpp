/** level_estimator_transmitters.cpp
*
** Copyright (c) 2019 Navigine.
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
    , mUseTxPower(navProps.commonSettings.useTxPower)
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

static double getAverageA(const Level& level, const RadioMeasurementsData& radioMsr)
{
  double averageA = 0.0;
  for(int i = 0; i < (int)radioMsr.size(); ++i)
  {
    const Transmitter<XYZPoint> transmitter = level.transmitter(radioMsr[i].id);
    const PathlossModel pathLossModel = transmitter.pathlossModel;
    averageA += pathLossModel.A;
  }
  averageA /= std::max((int)radioMsr.size(), 1);
  return averageA;
}

LevelId LevelEstimatorTransmitters::detectCurrentLevel(const RadioMeasurementsData& radioMsr)
{
  LevelId bestLevelId("");
  double bestAverCost = -1000.0;

  for (const Level& level : mLevelIndex->levels())
  {
    RadioMeasurementsData levelMsr = getLevelRadioMeasurements(level, radioMsr);
    if (!levelMsr.empty())
    {
      double averageRssi = getAverageRssi(levelMsr);
      double averageA;
      if (mUseTxPower)
        averageA = getAverageA(level, levelMsr);
      else
        averageA = 0;
      double averageCost = averageRssi + averageA;
      // Using weighted average. Weight depends on number of visible Transmitters
      double weight = 1.0 + 0.1 * levelMsr.size() / radioMsr.size();
      averageCost /= weight;

      if (averageCost > bestAverCost)
      {
        bestAverCost = averageCost;
        bestLevelId = level.id();
      }
    }
  }

  return bestLevelId;
}

} } // namespace navigine::navigation_core
