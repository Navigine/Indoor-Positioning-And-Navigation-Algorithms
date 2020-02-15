#pragma once

#include <deque>
#include <algorithm>
#include <vector3d.h>

namespace navigine {
namespace navigation_core {

struct SensorMeasurement
{
  long long   ts      = -1;
  Vector3d    values  = Vector3d();
};

struct Step
{
  long long ts;
  double len;
  Step() : ts(0), len(0) {};
  Step(long long stepTs, double stepLength) : ts(stepTs), len(stepLength) {};
};

struct Magnitude
{
  long long ts {};
  double value {};
  Magnitude() : ts(0), value(0) {};
  Magnitude(long long msrTs, double magnitudeValue) : ts(msrTs), value(magnitudeValue) {};
};

/**
 * This class detects step of the human on the basis of accelerometer measurements
 * and empirical model of humans gate.
 * Step length is calculated using minimal and maximal accelerations
*/
class Pedometer
{
public:
  Pedometer() = default;
  
  void update(const std::vector<SensorMeasurement>& accMsrs);
  std::deque<Step> calculateSteps();

private:
  Magnitude calculateFilteredAccMagnitudes() const;
  double calculateStepLength(long long timeIntervalMs, std::deque<Magnitude>::const_iterator rightBorderIt);

  //we use deque in order to fast delete measurements of the array when we got new ones
  std::deque<SensorMeasurement> mAccelMeasurements;     // Array for saving acceleration measurements
  std::deque<Magnitude>         mFilteredAccMagnitudes; // Array for saving filtered acceleration magnitudes
  std::deque<long long>         mTimes;                 // Array for saving step durations
 
  std::size_t mMagnitudeSize  =  0;
  long long   mPossibleStepTs = -1;                     // The time when step possibly occurred (sec in unix)
  long long   mStepTime       = -1;                     // The time when previous step possibly occured (sec in unix)
  bool        mIsStep         = false;                  // We use this variable to mark possible step
};

} } // namespace navigine::navigation_core
