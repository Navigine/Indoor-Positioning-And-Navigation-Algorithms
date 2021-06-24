/** level_estimator_radiomap.cpp
*
** Copyright (c) 2017 Navigine.
*
*/

#include <cmath>
#include <navigine/navigation-core/navigation_settings.h>

#include "level_estimator_radiomap.h"
#include "../knn/knn_utils.h"

namespace navigine {
namespace navigation_core {

LevelEstimatorRadiomap::LevelEstimatorRadiomap(
  const std::shared_ptr<LevelCollector>& levelCollector,
  const NavigationSettings& navProps)
    : LevelEstimator(navProps)
    , mLevelIndex(levelCollector)
    , mUseBestRp(navProps.commonSettings.useBestRefPointLevel)
{
  mLevelsRadiomaps = getLevelsRadiomaps(levelCollector);
}

LevelId LevelEstimatorRadiomap::detectCurrentLevel(const RadioMeasurementsData& radioMsr)
{
  LevelId bestLevelId = LevelId("");
  double bestLevelWeight = 0.0;

  for (const Level& level : mLevelIndex->levels())
  {    
    const std::multimap<TransmitterId, fingerprint>& radiomap = mLevelsRadiomaps.at(level.id());

    double levelWeight = mUseBestRp ? calcWeightBestRp(radioMsr, radiomap)
                                    : calcWeight(radioMsr, radiomap);
    if (levelWeight > bestLevelWeight)
    {
      bestLevelWeight = levelWeight;
      bestLevelId = level.id();
    }
  }

  return bestLevelId;
}

/** level weight is calculated based of difference between measured and reference signals */
double LevelEstimatorRadiomap::calcWeight(
  const RadioMeasurementsData& radioMsr,
  const std::multimap<TransmitterId, fingerprint> &fingerprints)
{
  int nUniqueTransmitters = 0;
  double levelWeight = 0.0;

  for (const RadioMeasurementData& meas : radioMsr)
  {
    //TODO try to use lower weight for wifi signal since it is not reliable for level identification
    double signalTypeWeight = 1.0;//(meas.type == SIGNAL_ENTRY_WIFI) ? 0.01 : 1.0;
    double measWeight       = 0.0;
    int    numOfOccurrences = 0;
    auto   range            = fingerprints.equal_range(meas.id);

    for (auto it = range.first; it != range.second; ++it)
    {
      const fingerprint& refPointsSsPair = it->second;
      const SignalStatistics& sigStat = refPointsSsPair.second;
      // It is possible that positive rssi values are used in measure.xml
      // and negative rssi values are used in measurements - so both abs are necessary here:
      double difference = std::fabs(std::fabs(meas.rssi) - std::fabs(sigStat.mean));
      measWeight += signalTypeWeight * 1.0 / (difference + 0.1);
      numOfOccurrences ++;
    }

    if (numOfOccurrences > 0)
      levelWeight += measWeight / numOfOccurrences;

    if (std::distance(range.first, range.second) > 0)
      nUniqueTransmitters++;
  }

  levelWeight *= static_cast<double>(nUniqueTransmitters);
  return levelWeight;
}

/** level weight is given equal to weight of best reference point */
double LevelEstimatorRadiomap::calcWeightBestRp(
    const RadioMeasurementsData& radioMsr,
    const std::multimap<TransmitterId, fingerprint>& radiomap)
{
  std::map<ReferencePointId, double> rpToWeight = calcRpWeightsKnn(radiomap, radioMsr, true);

  double bestWeight = 0.0;
  if (!rpToWeight.empty())
  {
    ReferencePointId bestRp = rpToWeight.begin()->first;
    for (auto const& rpWeightPair : rpToWeight)
    {
      if (rpWeightPair.second > bestWeight)
      {
        bestRp = rpWeightPair.first;
        bestWeight = rpWeightPair.second;
      }
    }
  }

  return bestWeight;
}

} } // namespace navigine::navigation_core
