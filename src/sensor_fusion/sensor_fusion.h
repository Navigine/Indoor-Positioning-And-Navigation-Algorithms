/** sensor_measurement.h
 *
 * Copyright (c) 2018 Navigine.
 *
 */

#ifndef _SENSOR_FUSION_H_
#define _SENSOR_FUSION_H_

#include <navigine/navigation-core/motion_info.h>
#include <navigine/navigation-core/navigation_input.h>
#include <navigine/navigation-core/navigation_settings.h>

#include <vector>

#include "complementary_filter.h"
#include "pedometer.h"

namespace navigine {
namespace navigation_core {

class SensorFusion
{
public:
  SensorFusion(const NavigationSettings& navProps);
  MotionInfo calculateDisplacement(const SensorMeasurement& msr, long long tsMs);

private:
  const bool mUseCalcAzimuth;
  const double mStepMultiplier;
  ComplementaryFilter mComplFilter;
  Pedometer mPedometer;
  int mStepCounter = 0;
  double mPrevHeading = 0.0;
};

} } // namespace navigine::navigation_core

#endif // _SENSOR_FUSION_H_
