/** level_estimator.cpp
*
** Copyright (c) 2017 Navigine.
*
*/

#include <navigine/navigation-core/navigation_settings.h>
#include "level_estimator.h"


namespace navigine {
namespace navigation_core {

LevelEstimator::LevelEstimator(const NavigationSettings& navProps)
  : mUseBarometer(navProps.commonSettings.useBarometer)
{
}

/**
 * function detects id of current level according to highest average RSSI for
 * placed transmitters. In a case of any error returns -1
 */
LevelId LevelEstimator::calculateLevel(
  const RadioMeasurementsData& radioMsr,
  const SensorMeasurement& sensorMsr)
{
  LevelId detectedLevelId = detectCurrentLevel(radioMsr);
  LevelId levelIdWithHistory = mHistory.detectLevelUsingHistory(detectedLevelId);
  LevelId levelId = mUseBarometer ? mBarometer.checkLevelChange(levelIdWithHistory, sensorMsr)
                                    : levelIdWithHistory;

  return levelId;
}

} } // namespace navigine::navigation_core
