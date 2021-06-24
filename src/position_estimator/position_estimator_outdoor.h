/** position_estimator_outdoor.h
 *
 * Copyright (c) 2018 Navigine.
 *
 */

#ifndef NAVIGINE_POSITION_ESTIMATOR_OUTDOOR_H
#define NAVIGINE_POSITION_ESTIMATOR_OUTDOOR_H

#include <navigine/navigation-core/navigation_output.h>
#include <navigine/navigation-core/navigation_input.h>
#include <navigine/navigation-core/navigation_settings.h>
#include <navigine/navigation-core/level_collector.h>
#include <deque>

#include "../position.h"
#include "../measurement_types.h"

namespace navigine {
namespace navigation_core {

struct GpsPosition
{
  GpsPosition(double _latitude, double _longitude, double _deviation, long long _ts)
      : latitude  (_latitude)
      , longitude (_longitude)
      , deviation (_deviation)
      , ts (_ts)
  {
    isEmpty = false;
  }

  GpsPosition()
  {
    isEmpty = true;
  }

  double latitude = NAN;
  double longitude = NAN;
  double altitude = NAN;
  double deviation = NAN;
  long long ts = 0;
  bool isEmpty = true;
};

class SigmaFilter
{
  public:
    SigmaFilter(
      const unsigned int& sigmaWindow,
      const double& sigmaDelta)
      : mSigmaWindow(sigmaWindow)
      , mSigmaDelta(sigmaDelta)
    {}

    bool add(const XYPoint& point);
    void clear();

  private:
    const unsigned int mSigmaWindow;
    const double mSigmaDelta;
    std::deque<XYPoint> mGNSSbuffer;
};

class PositionEstimatorOutdoor
{
public:
  PositionEstimatorOutdoor(
    const std::shared_ptr<LevelCollector>& levelCollector,
    const NavigationSettings& navProps);

  Position calculatePosition(
    long long ts,
    const SensorMeasurement& sensorMsr,
    const NmeaMeasurement& nmeaMsr,
    const bool flagIndoorPos);

  private:
    GpsPosition extractGpsPosition(long long ts, const SensorMeasurement& sensorEntry);
    int extractNumberOfSatellites(const NmeaMeasurement& nmeaEntry);
    Position mLastGpsPosition;

    std::shared_ptr<LevelCollector> mLevelIndex;
    SigmaFilter mSigmaFilter;
    const double mGpsValidTimeWindowSec;
    const bool mUseGpsOutsideMap;
    const bool mUseSigmaFilter;
    const int mMinNumSats;
    const double mMinGpsDeviation;
    const double mMaxGpsDeviation;
};

} } // namespace navigine::navigation_core

#endif // POSITION_ESTIMATOR_OUTDOOR_H
