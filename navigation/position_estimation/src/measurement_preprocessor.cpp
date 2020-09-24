#include <numeric>
#include <algorithm>

#include "measurement_preprocessor.h"

namespace navigine {
namespace navigation_core {

MeasurementsPreprocessor::MeasurementsPreprocessor(double sigAverageTimeSec,
                                                   double sigWindowShiftSec,
                                                   bool useWifi,
                                                   bool useBle)
    : mCurrentTs         ( -1 )
    , mSigAverageTimeSec ( sigAverageTimeSec )
    , mSigWindowShiftSec ( sigWindowShiftSec )
    , mUseWifi           ( useWifi )
    , mUseBle            ( useBle )
{
  long long sigAverageTime = (long long)(1000 * mSigAverageTimeSec);
  long long sigWindowShift = (long long)(1000 * mSigWindowShiftSec);
  if (sigWindowShift == 0 || sigWindowShift > sigAverageTime)
    sigWindowShift = sigAverageTime;
  
  long long keepRadioTimeMs = sigAverageTime - sigWindowShift;
  mRadiosBuffer = RadioMeasurementBuffer(sigWindowShift, keepRadioTimeMs);
}

void MeasurementsPreprocessor::update(const RadioMeasurement& msr)
{
  if (isSignalTypeSupported(msr.type) && isRssiValid(msr.rssi))
  {
    std::vector<RadioMeasurement> msrVect;
    msrVect.push_back(msr);
    mRadiosBuffer.addMeasurements(msr.ts, msrVect);
  }

  if (msr.ts > mCurrentTs)
    mCurrentTs = msr.ts;
}

std::vector<RadioMeasurement> MeasurementsPreprocessor::extractRadioMeasurements()
{
  std::vector<RadioMeasurement> radioMeasurements = mRadiosBuffer.extractMeasurements();
  if (mUseClosestAps)
  {
    std::sort(radioMeasurements.rbegin(), radioMeasurements.rend(),
           [](const RadioMeasurement& lhs, const RadioMeasurement& rhs)
           {
             return lhs.rssi < rhs.rssi;
           });

    if (radioMeasurements.size() > (unsigned int) mNumClosestAps)
      radioMeasurements.resize(mNumClosestAps);
  }

  return radioMeasurements;
}

long long MeasurementsPreprocessor::getCurrentTs() const
{
  return mCurrentTs;
}

bool MeasurementsPreprocessor::isSignalTypeSupported(RadioMeasurement::Type signalType) const
{
  return (signalType == RadioMeasurement::Type::WIFI && mUseWifi) ||
         (signalType == RadioMeasurement::Type::BEACON && mUseBle);
}

bool MeasurementsPreprocessor::isRssiValid(double rssi) const
{
  return rssi > mCutOffRssi && rssi < 0.0;
}

RadioMeasurementBuffer::RadioMeasurementBuffer(
  long long sigWindowShiftMs, 
  long long radioKeepPeriodMs)
{
  mRadioKeepPeriodMs = radioKeepPeriodMs;
  mSignalWindowShiftMs = sigWindowShiftMs;
}

void RadioMeasurementBuffer::addMeasurements(long long messageTs, const std::vector<RadioMeasurement>& msrs)
{
  if (mCurrentTs == -1)
  {
    mCurrentTs = messageTs;
    mLastExtractionTs = messageTs;
  }

  for (const RadioMeasurement& msr : msrs)
  {
    RadioMeasurement radioMsr = msr;
    radioMsr.rssi = msr.rssi;

    // check if it is a fresh measurement
    if (radioMsr.ts > mLastExtractionTs)
      mMeasurements.push_back(radioMsr);

    mCurrentTs = std::max(radioMsr.ts, mCurrentTs);
  }
}

std::vector<RadioMeasurement> RadioMeasurementBuffer::extractMeasurements()
{
  std::vector<RadioMeasurement> measurements;
  if (mCurrentTs - mLastExtractionTs < mSignalWindowShiftMs)
  {
    // if not enough time passed
    return measurements;
  }

  mLastExtractionTs = mCurrentTs;

  // Averaging measurements from same Transmitters
  std::sort(mMeasurements.begin(), mMeasurements.end(),
           [](const RadioMeasurement& lhs, const RadioMeasurement& rhs)
           {
             return lhs.id < rhs.id;
           });

  auto sameIdFirst = mMeasurements.cbegin();
  while (sameIdFirst != mMeasurements.cend())
  {
    auto   sameIdLast = std::upper_bound(sameIdFirst, mMeasurements.cend(), *sameIdFirst,
                                        [](const RadioMeasurement& lhs, const RadioMeasurement& rhs)
                                        {
                                          return lhs.id < rhs.id;
                                        });

    double nSignals   = std::distance   (sameIdFirst, sameIdLast);
    double sumRssi    = std::accumulate (sameIdFirst, sameIdLast, 0.0,
                                        [](double sum, const RadioMeasurement& m)
                                        {
                                          return sum + m.rssi;
                                        });

    double averRssi = sumRssi / nSignals;

    double sumDistances = std::accumulate (sameIdFirst, sameIdLast, 0.0,
                                        [](double sum, const RadioMeasurement& m)
                                        {
                                          return sum + m.distance;
                                        });

    double averDistance = sumDistances / nSignals;

    RadioMeasurement radioMsr = *sameIdFirst;
    radioMsr.rssi = averRssi;
    radioMsr.distance = averDistance;
    measurements.push_back(radioMsr);
    sameIdFirst = sameIdLast;
  }

  // Erasing old measurements
  if (mRadioKeepPeriodMs == 0)
  {
    mMeasurements.clear();
  }
  else
  {
    long long keepFromTs = mCurrentTs - mRadioKeepPeriodMs;

    std::sort(mMeasurements.begin(), mMeasurements.end(),
           [](const RadioMeasurement& lhs, const RadioMeasurement& rhs)
           {
             return lhs.ts < rhs.ts;
           });

    auto eraseUpperBound = std::find_if(mMeasurements.begin(),
                                        mMeasurements.end(),
                                        [keepFromTs](const RadioMeasurement& rm)
                                        {
                                          return rm.ts >= keepFromTs;
                                        });

    mMeasurements.erase(mMeasurements.begin(), eraseUpperBound);
  }

  return measurements;
}

} } // namespace navigine::navigation_core
