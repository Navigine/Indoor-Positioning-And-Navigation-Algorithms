/** position_estimator_zone.h
 *
 * Author: Aleksandr Galov <asgalov@gmail.com>
 * Copyright (c) 2017 Navigine. All rights reserved.
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
  PositionEstimatorZone(
    const std::shared_ptr<LevelCollector>& levelCollector,
    const NavigationSettings& navProps);

  Position calculatePosition(
    const Level& level,
    long long ts,
    const RadioMeasurementsData& radioMsr,
    NavigationStatus& retStatus) override;

private:
  std::map<LevelId, double> mLogModelParameterA;
  std::map<LevelId, double> mLogModelParameterB;
};

} } // namespace navigine::navigation_core

#endif // POSITION_ESTIMATOR_ZONE_H
