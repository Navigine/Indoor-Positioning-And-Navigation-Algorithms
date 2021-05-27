/** level_estimator_transmitters.h
*
* Author: Vasily Kosyanchuk <v.kosyanchuk@navigine.com>
* Copyright (c) 2019 Navigine. All rights reserved.
*
*/

#ifndef LEVEL_ESTIMATOR_TRANSMITTERS_H
#define LEVEL_ESTIMATOR_TRANSMITTERS_H

#include "level_estimator.h"

namespace navigine {
namespace navigation_core {

class LevelEstimatorTransmitters : public LevelEstimator
{
public:
  LevelEstimatorTransmitters(
    const std::shared_ptr<LevelCollector>& levelCollector,
    const NavigationSettings& navProps);

protected:
  LevelId detectCurrentLevel(const RadioMeasurementsData& radioMsr) override;

private:
  std::shared_ptr<LevelCollector> mLevelIndex;
};

} } // namespace navigine::navigation_core

#endif // LEVEL_ESTIMATOR_TRANSMITTERS_H
