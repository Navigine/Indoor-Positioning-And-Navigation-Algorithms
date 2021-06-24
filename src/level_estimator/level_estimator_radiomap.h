/** level_estimator_radiomap.h
*
** Copyright (c) 2017 Navigine.
*
*/

#ifndef LEVEL_ESTIMATOR_RADIOMAP_H
#define LEVEL_ESTIMATOR_RADIOMAP_H

#include "level_estimator.h"

namespace navigine {
namespace navigation_core {

class LevelEstimatorRadiomap : public LevelEstimator
{
public:
  LevelEstimatorRadiomap(
    const std::shared_ptr<LevelCollector>& levelCollector,
    const NavigationSettings& navProps);

protected:
  LevelId detectCurrentLevel(const RadioMeasurementsData& radioMsr) override;

private:
  using fingerprint = std::pair<ReferencePointId, SignalStatistics>;

  double calcWeight(
      const RadioMeasurementsData& radioMsr,
      const std::multimap<TransmitterId, fingerprint> &fingerprints);

  double calcWeightBestRp(
      const RadioMeasurementsData& radioMsr,
      const std::multimap<TransmitterId, fingerprint> &radiomap);

  std::shared_ptr<LevelCollector> mLevelIndex;
  std::map<LevelId,
           std::multimap<TransmitterId, fingerprint>> mLevelsRadiomaps;

  const bool mUseBestRp;
};

} } // namespace navigine::navigation_core

#endif // LEVEL_ESTIMATOR_RADIOMAP_H
