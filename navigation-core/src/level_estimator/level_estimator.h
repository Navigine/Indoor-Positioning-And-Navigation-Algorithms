/** level_estimator.h
*
* Author: Vasily Kosyanchuk <v.kosyanchuk@navigine.com>
* Copyright (c) 2017 Navigine. All rights reserved.
*
*/

#ifndef LEVEL_ESTIMATOR_H
#define LEVEL_ESTIMATOR_H

#include <navigation_input.h>
#include <navigation_settings.h>
#include <level_collector.h>

#include "level_history.h"
#include "barometer.h"

#include "../measurement_types.h"

namespace navigine {
namespace navigation_core {

class LevelEstimator
{
  public:
    LevelEstimator(const NavigationSettings& navProps);
    virtual ~LevelEstimator() {}
    LevelId calculateLevel(
      const RadioMeasurementsData& radioMsr,
      const SensorMeasurement& sensorMsr);

  protected:
    virtual LevelId detectCurrentLevel(const RadioMeasurementsData& radioMsr) = 0;

  private:
    LevelHistory mHistory;
    Barometer mBarometer;
    const bool mUseBarometer;
};

} } // namespace navigine::navigation_core

#endif // LEVEL_ESTIMATOR_H
