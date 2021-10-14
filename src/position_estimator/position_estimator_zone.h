/**
 * @file position_estimator_zone.h
 * @author timur chikichev
 * @brief 
 * @version 1.0
 * @date 2021-10-14
 * 
 * @copyright Copyright (c) 2021 Navigine
 * 
 */
#ifndef POSITION_ESTIMATOR_ZONE_H
#define POSITION_ESTIMATOR_ZONE_H

#include "position_estimator.h"

namespace navigine {
namespace navigation_core {

class PositionEstimatorZone: public PositionEstimator
{
public:
  /**
   * @brief Construct a new Position Estimator Zone object
   * 
   * @param levelCollector - {type}
   * @param navProps - {type}
   */
  PositionEstimatorZone(
    const std::shared_ptr<LevelCollector>& levelCollector,
    const NavigationSettings& navProps);
  /**
   * @brief get position from state, observations and map data
   * 
   * @param level - {type}
   * @param ts - {type}
   * @param radioMsr - {type}
   * @param motionInfo - {type}
   * @param retStatus - {type}
   * @return Position 
   */
  Position calculatePosition(
    const Level& level,
    long long ts,
    const RadioMeasurementsData& radioMsr,
    const MotionInfo& motionInfo,
    NavigationStatus& retStatus) override;

private:
  std::map<LevelId, double> mLogModelParameterA;
  std::map<LevelId, double> mLogModelParameterB;
};

} } // namespace navigine::navigation_core

#endif // POSITION_ESTIMATOR_ZONE_H
