/** barometer.h
 *
 * Copyright (c) 2018 Navigine.
 *
 */

#ifndef BAROMETER_H
#define BAROMETER_H

#include <navigation_input.h>

#include <deque>
#include "level.h"

#include "../measurement_types.h"

namespace navigine {
namespace navigation_core {

class Barometer
{
  public:
    Barometer() : mIsInitialized ( false ), mLastKnownLevel ( LevelId("")) {}
    LevelId checkLevelChange(
      const LevelId &levelId,
      const SensorMeasurement& sensorMsr);

  private:
    void updateBuffers(const SensorMeasurement& sensorMsr);
    void updateLevel(LevelId levelId);

    std::deque<double> mNewMeasurements = {};
    std::deque<double> mStoredMeasurements = {};

    bool mIsInitialized;
    LevelId mLastKnownLevel;
};

} } // namespace navigine::navigation_core

#endif // BAROMETER_H
