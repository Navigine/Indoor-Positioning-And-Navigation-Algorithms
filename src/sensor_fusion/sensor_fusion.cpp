#include <algorithm>
#include <numeric>
#include <navigine/navigation-core/navigation_settings.h>
#include "sensor_fusion.h"

namespace navigine {
namespace navigation_core {

SensorFusion::SensorFusion(const NavigationSettings& navProps)
    : mUseCalcAzimuth (navProps.commonSettings.useCalculatedAzimuth)
    , mStepMultiplier (navProps.commonSettings.stepMultiplier)
    , mComplFilter ( ComplementaryFilter(navProps) )
    , mPedometer ( Pedometer() )
{
}

MotionInfo SensorFusion::calculateDisplacement(const SensorMeasurement& msr, long long tsMs)
{
  mPedometer.update(msr);
  std::deque<Step> steps = mPedometer.calculateSteps();
  
  mComplFilter.update(msr);

  MotionInfo motionInfo;
  motionInfo.heading = -mComplFilter.getFusedAzimuth(); // the counter-clockwise rotation needed
  
  if (mUseCalcAzimuth)
  {
    motionInfo.azimuth = mComplFilter.getMagneticAzimuth();
    motionInfo.isAzimuthValid = true;
  }
  else if (!mComplFilter.isDeviceAzimuthOutdated())
  {
    motionInfo.azimuth = mComplFilter.getDeviceAzimuth();
    motionInfo.isAzimuthValid = true;
  }

  motionInfo.gyroHeading = mComplFilter.getGyroHeading();
  motionInfo.isStepDetected = !steps.empty();
  if (motionInfo.isStepDetected)
  {
    motionInfo.deltaAngle   = motionInfo.heading - mPrevHeading;
    mPrevHeading            = motionInfo.heading;
    motionInfo.distance     = std::accumulate(steps.begin(), steps.end(), 0.0,
                                [](double sum, const Step& s) { return sum + s.len; });
    motionInfo.distance    *= mStepMultiplier;
    motionInfo.lastMotionTs = tsMs;
    mStepCounter           += steps.size();
    motionInfo.stepCounter  = mStepCounter;
  }

  return motionInfo;
}

} } // namespace navigine::navigation_core
