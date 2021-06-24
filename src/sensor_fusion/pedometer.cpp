/** pedometer.cpp
*
* Copyright (c) 2018 Navigine.
*
*/

#include <navigine/navigation-core/navigation_input.h>

#include <algorithm>
#include <numeric>
#include <cmath>

#include "pedometer.h"
//#define DEBUG_OUTPUT_PEDOMETER 

namespace navigine {
namespace navigation_core {

#if defined (DEBUG_OUTPUT_PEDOMETER)
#include <iostream>
#include <fstream>
  static const std::string PREPROCESSED_PEDOMETER_MSR_DEBUG_OUTPUT_FILE = DEBUG_OUTPUT_PEDOMETER;

  void clearPedometerDebugOutput();
  void printDebugPedometer(const std::string &type, long long ts, double value);
#endif

static const long long  AVERAGING_TIME_INTERVAL_MS      = 2500; //2000   // Time interval on which we calculate averaged filtered magnitude in order to align it relative to zero value
static const long long  FILTER_TIME_INTERVAL_MS         = 200;     // Filtered acc measurements are calculated as average over this time interval

static const long long  UPDATE_TIME_INTERVAL_MS         = 700; // 650   // The time interval(in millisec) on which we update accelMax, Min, threshold
static const long long  MIN_TIME_BETWEEN_STEPS_MS       = 300;     // We cut off possible steps if time between them is less then this thresh

static const double     STEP_LENGTH_CONST               = 0.52; //0.48    // Constant using in algorithm which is responsible for step length calculation
static const double     MINIMAL_THRESHOLD_VALUE         = 0.05 * 9.80665; // Empirical threshold values from article p.888

static const int        MINIMAL_NUMBER_OF_STEPS         = 5;       // First N steps in which we accumulate data about average step time
static const int        MAXIMUM_NUMBER_OF_STEPS         = 50;      // Maximum number of steps to make an assumption about average step time
static const int        MAX_STEP_TIME                   = 2000;    // The maximum possible time for step duration

void Pedometer::update(const SensorMeasurement& msr)
{
  mMagnitudeSize = (std::max)(mFilteredAccMagnitudes.size(), size_t(1));

  if (msr.data.type == SensorMeasurementData::Type::ACCELEROMETER)
  {
    mAccelMeasurements.push_back(msr);
    Magnitude magn = calculateFilteredAccMagnitudes();
    if (magn.ts!=0)
      mFilteredAccMagnitudes.push_back(magn);

    while (mAccelMeasurements.back().ts - mAccelMeasurements.front().ts > 2 * AVERAGING_TIME_INTERVAL_MS &&
           mAccelMeasurements.size() > 0)
    {
      mAccelMeasurements.pop_front();
    }
  
#if defined (DEBUG_OUTPUT_PEDOMETER)
    if (mAccelMeasurements.size() == 1) 
    {
      //Remove output file at the start of the test
      clearPedometerDebugOutput(); 
    } 
    if (magn.ts != 0)
      printDebugPedometer("MAGNITUDE", magn.ts, magn.value);

    printDebugPedometer("INPUT", msr.ts, msr.getMagnitude());   
#endif
  }
}

/**
 * This method calculates and form array with filtered acceleration magnitude
 * sliding window average is used (low pass). This function also updates average
 * filtered magnitude
 */
Magnitude Pedometer::calculateFilteredAccMagnitudes() const
{
   long long lastMeasTs = mAccelMeasurements.back().ts;

  std::deque<SensorMeasurement>::const_iterator lBorderAverIt = mAccelMeasurements.begin();
  while (lastMeasTs - lBorderAverIt->ts >= AVERAGING_TIME_INTERVAL_MS)
    lBorderAverIt++;
  
  double averMagnitude = std::accumulate(lBorderAverIt, mAccelMeasurements.end(), 0.0,
                                         [](double sum, const SensorMeasurement msr)
                                         {return sum+msr.data.values.magnitude();});
  double nMeasAverage  = std::distance(lBorderAverIt, mAccelMeasurements.end());

  std::deque<SensorMeasurement>::const_iterator lBorderFilterIt = mAccelMeasurements.begin();
  while (lastMeasTs - lBorderFilterIt->ts >= FILTER_TIME_INTERVAL_MS)
    lBorderFilterIt++;

  double filterMagnitude = std::accumulate(lBorderFilterIt, mAccelMeasurements.end(), 0.0, 
                                           [](double sum, const SensorMeasurement msr)
                                           {return sum+msr.data.values.magnitude();});
  double nMeasFiltered   = std::distance(lBorderFilterIt, mAccelMeasurements.end());

  // There is no measurements at acceleration array!\n");
  if (nMeasAverage == 0 || nMeasFiltered == 0)
    return Magnitude(0,0);

  filterMagnitude /= nMeasFiltered;
  averMagnitude   += filterMagnitude; // For correct processing at the very beginning
  averMagnitude   /= nMeasAverage;
  filterMagnitude -= averMagnitude;
  
  return Magnitude(lastMeasTs, filterMagnitude);
}

std::deque<Step> Pedometer::calculateSteps()
{
  std::deque<Step> steps = {};
  if (mFilteredAccMagnitudes.size() <= 3) //There is no enough accelerometer measurements to detect steps
    return steps;

  long long averageStepTime = 0;
  auto      nSteps          = std::max(std::distance(mTimes.begin(), mTimes.end()), std::ptrdiff_t(1));

  if (nSteps >= MINIMAL_NUMBER_OF_STEPS) 
    averageStepTime = std::accumulate(mTimes.begin(), mTimes.end(), 0, 
                                       [](long long sum, long long s) 
                                        { return sum + s; });
  averageStepTime /= nSteps;
  double timeBetweenSteps = std::max(1.0 * MIN_TIME_BETWEEN_STEPS_MS, 0.6 * averageStepTime);

  for (size_t i = mMagnitudeSize; i < mFilteredAccMagnitudes.size(); i++)
  {
    Magnitude curAcc  = mFilteredAccMagnitudes[i];
    Magnitude prevAcc = mFilteredAccMagnitudes[i-1];
    
    //True if cross threshold and if new detection isn't too early.
    if (!mIsStep &&
        prevAcc.value < MINIMAL_THRESHOLD_VALUE &&
        curAcc.value > MINIMAL_THRESHOLD_VALUE &&
        curAcc.ts - mPossibleStepTs > MIN_TIME_BETWEEN_STEPS_MS &&
        timeBetweenSteps > 0) //  && curAcc.ts - mPossibleStepTs > timeBetweenSteps
    {
      mIsStep         = true;
      mPossibleStepTs = curAcc.ts;
    }

    if (mIsStep)
    {
      double stepLength = calculateStepLength(UPDATE_TIME_INTERVAL_MS, mFilteredAccMagnitudes.begin()+i);
      if (mPossibleStepTs - mStepTime < MAX_STEP_TIME)
          mTimes.push_back(mPossibleStepTs - mStepTime);

      steps.push_back(Step(mPossibleStepTs, stepLength));
#if defined (DEBUG_OUTPUT_PEDOMETER)
      printDebugPedometer("STEP", mPossibleStepTs, stepLength);
#endif
      mStepTime = mPossibleStepTs;
      mIsStep = false; // prepare to detect new step
    }
  }

  while(mFilteredAccMagnitudes.back().ts - mFilteredAccMagnitudes.front().ts > 2 * UPDATE_TIME_INTERVAL_MS)
    mFilteredAccMagnitudes.pop_front();

  while (mTimes.size() > MAXIMUM_NUMBER_OF_STEPS) 
    mTimes.pop_front();  

  return steps;
}

double Pedometer::calculateStepLength(long long timeIntervalMs, std::deque<Magnitude>::const_iterator rightBorderIt)
{
   // find left border
  std::deque<Magnitude>::const_iterator leftBorderIt = rightBorderIt;
  while (rightBorderIt->ts - leftBorderIt->ts <= timeIntervalMs &&
         leftBorderIt != mFilteredAccMagnitudes.begin() )
    leftBorderIt--;

  double maxMagn      = std::max_element(leftBorderIt, rightBorderIt,
                                         [](Magnitude m1, Magnitude m2) 
                                         {return m1.value < m2.value; })->value;
  double minMagn      = std::min_element(leftBorderIt, rightBorderIt, 
                                         [](Magnitude m1, Magnitude m2)
                                         {return m1.value < m2.value; })->value;
  double accAmplitude = maxMagn - minMagn;
  double stepLen      = STEP_LENGTH_CONST * sqrt(sqrt(accAmplitude));

  return stepLen;
}

#if defined (DEBUG_OUTPUT_PEDOMETER)
void clearPedometerDebugOutput()
{
  std::cout << "print debug output to " << PREPROCESSED_PEDOMETER_MSR_DEBUG_OUTPUT_FILE << std::endl;
  std::ofstream debugOutputFile;
  debugOutputFile.open(PREPROCESSED_PEDOMETER_MSR_DEBUG_OUTPUT_FILE);
  debugOutputFile << "type ts value" << std::endl;
  debugOutputFile.close();
}

void printDebugPedometer(const std::string &type, long long ts, double value)
{
  std::ofstream debugOutputFile;
  debugOutputFile.open(PREPROCESSED_PEDOMETER_MSR_DEBUG_OUTPUT_FILE, std::ofstream::out | std::ofstream::app);
  debugOutputFile << type << " " << ts << " " << value << std::endl;
  debugOutputFile.close();
}
#endif

} } // namespace navigine::navigation_core
