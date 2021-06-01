#include <navigation_input.h>

#include <cmath>
#include <numeric>
#include <vector>

#include "barometer.h"

namespace navigine {
namespace navigation_core {

#ifdef DEBUG_OUTPUT_BAROMETER
#include <iostream>
#include <fstream>
#endif

static const double BAROMETER_R                   = 8.31;      // [J / (mol * K)]
static const double BAROMETER_G                   = 9.8;       // [m / s^2]
static const double BAROMETER_T                   = 288.0;     // [K]
static const double BAROMETER_M                   = 0.0289;    // [kg / mol]
static const double BAROMETER_PASCAL_TO_METERS    = -BAROMETER_R * BAROMETER_T  / (BAROMETER_G * BAROMETER_M);
static const double BAROMETER_MBAR_TO_PASCAL      = 100.0;
static const double BAROMETER_MIN_FLOOR_HEIGHT_M  = 2.0;       // [m]

static const size_t SIZE_OF_CHUNK = 100;

static double calcMean(const std::deque<double>& measurements)
{
  if (measurements.empty())
  {
    //TODO process error
    return 0;
  }

  return std::accumulate(measurements.begin(), measurements.end(), 0.0) / measurements.size();
}

//TODO test if there is no barometer in the phone
//TODO try to use ring buffer with fixed size
//TODO take into account vertical speed in Pa/sec
// - the floor is changed only if the vertical displacement is more than distance between floors (i.e. 3m)
LevelId Barometer::checkLevelChange(
  const LevelId& levelId,
  const SensorMeasurement& sensorMsr)
{
  if (!mIsInitialized)
  {
    mIsInitialized = true;
    return levelId;
  }

  updateBuffers(sensorMsr);
  updateLevel(levelId);
  return mLastKnownLevel;
}

void Barometer::updateBuffers(const SensorMeasurement& sensorMsr)
{
#ifdef DEBUG_OUTPUT_BAROMETER
  std::ofstream debugOutputFile;
  debugOutputFile.open(DEBUG_OUTPUT_BAROMETER, std::ios_base::app);
  if (sensorMsr.data.type == SENSOR_ENTRY_BAROMETER)
    debugOutputFile << m.ts << " " << m.values.x << std::endl;
  debugOutputFile.close();
#endif

  if (sensorMsr.data.type == SensorMeasurementData::Type::BAROMETER)
  {
    if (mStoredMeasurements.size() > SIZE_OF_CHUNK)
      mStoredMeasurements.pop_front();

    if (mNewMeasurements.size() > SIZE_OF_CHUNK)
    {
      mStoredMeasurements.push_back(mNewMeasurements.front());
      mNewMeasurements.pop_front();
    }

    mNewMeasurements.push_back(sensorMsr.data.values.x);
  }
}

//TODO take into account pressure change speed - i.e. a person can move slowly upstairs
void Barometer::updateLevel(LevelId levelId)
{
  if ((mNewMeasurements.size() > SIZE_OF_CHUNK)
          && (mStoredMeasurements.size() >= SIZE_OF_CHUNK))
  {
    if (mLastKnownLevel != levelId)
    {
      double curMean = calcMean(mNewMeasurements);
      double lastMean = calcMean(mStoredMeasurements);

      if (curMean <= std::numeric_limits<double>::epsilon()
            || lastMean <= std::numeric_limits<double>::epsilon())
      {
        //TODO: process error: incorrect average pressure!
        return;
      }

      double p2Pa = BAROMETER_MBAR_TO_PASCAL * curMean;
      double p1Pa = BAROMETER_MBAR_TO_PASCAL * lastMean;

      double deltaHeightM = BAROMETER_PASCAL_TO_METERS * std::log(p2Pa / p1Pa);

      bool levelHasChanged = std::abs(deltaHeightM) > BAROMETER_MIN_FLOOR_HEIGHT_M;

      if (levelHasChanged)
          mLastKnownLevel = levelId;
    }
  }
  else
      mLastKnownLevel = levelId;
}

} } // namespace navigine::navigation_core
