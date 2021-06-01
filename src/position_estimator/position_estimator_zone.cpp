/** position_estimator_zone.cpp
 *
 * Copyright (c) 2017 Navigine.
 *
 */

#include <navigation_settings.h>
#include "position_estimator_zone.h"

namespace navigine {
namespace navigation_core {

PositionEstimatorZone::PositionEstimatorZone(
  const std::shared_ptr<LevelCollector> &levelCollector,
  const NavigationSettings& navProps)
  : PositionEstimator(levelCollector)
{
  for (const Level& level: levelCollector->levels())
  {
    mLogModelParameterA[level.id()] = navProps.levelsSettings.at(level.id()).normalModeA;
    mLogModelParameterB[level.id()] = navProps.levelsSettings.at(level.id()).normalModeB;
  }
}

// TODO take into account transmitter power!
Position PositionEstimatorZone::calculatePosition(
  const Level& level,
  long long ts,
  const RadioMeasurementsData& radioMsr,
  NavigationStatus &retStatus)
{
  if (radioMsr.empty())
  {
    retStatus = NavigationStatus::NO_SOLUTION;
    return mPosition;
  }
  
  retStatus = NavigationStatus::OK;

  RadioMeasurementsData radioMeasurements = getLevelRadioMeasurements(level, radioMsr);
  if (radioMeasurements.empty())
  {
    retStatus = NavigationStatus::NO_SOLUTION;
    return Position();
  }

  auto nearestTx = std::max_element(radioMeasurements.begin(), radioMeasurements.end(),
        [](RadioMeasurementData msr1, RadioMeasurementData msr2) {return msr1.rssi < msr2.rssi; });

  TransmitterId nearestTxId = nearestTx->id;

  XYZPoint p = XYZPoint(0.0, 0.0, 0.0);
  if (level.containsTransmitter(nearestTxId))
  {
    p = level.transmitter(nearestTxId).point;
  }

  //TODO rename prop from A to "GP_Normal_Mode_A"
  double A = mLogModelParameterA[level.id()];
  double B = mLogModelParameterB[level.id()];

  double nearestTxRssi = nearestTx->rssi;

  double precision = sqrt(exp((A - nearestTxRssi) / B)) + 1.0;

  return Position(
    p.x, 
    p.y, 
    level.id(),
    precision, 
    ts, 
    false, 
    Provider::INDOOR,
    0.0);
}

} } // namespace navigine::navigation_core
