/** measurement_preprocessor
 *
 * Author: Aleksandr Galov <asgalov@gmail.com>
 * Copyright (c) 2017 Navigine. All rights reserved.
 *
 */

#include <algorithm>
#include <numeric>

#include "measurement_preprocessor.h"

#if defined (DEBUG_OUTPUT_MSR_PREPROCESSOR)
#include <iostream>
#include <fstream>

namespace navigine {
namespace navigation_core {

static const std::string PREPROCESSED_MSR_DEBUG_OUTPUT_FILE = DEBUG_OUTPUT_MSR_PREPROCESSOR;
void clearDebugOutput();
void printInputSignals(NavigationMessage* navMsg, RadioMeasurementsData& entries);
void printExtractedMeasurements(RadioMeasurementsData& radioMeasurements);

} } // namespace navigine::navigation_core
#endif

namespace navigine {
namespace navigation_core {

namespace {

const constexpr bool DEFAULT_USE_WIFI = false;
const constexpr bool DEFAULT_USE_BLE = true;

bool checkUseWifi(const NavigationSettings& navProps) {
  const auto signalsToUse = navProps.commonSettings.signalsToUse;
  if (signalsToUse == CommonSettings::SignalsToUse::WIFI || signalsToUse == CommonSettings::SignalsToUse::BOTH) {
    return true;
  } else if (signalsToUse == CommonSettings::SignalsToUse::BLE) {
    return false;
  }
  return DEFAULT_USE_WIFI;
}

bool checkUseBle(const NavigationSettings& navProps) {
  const auto signalsToUse = navProps.commonSettings.signalsToUse;
  if (signalsToUse == CommonSettings::SignalsToUse::BLE || signalsToUse == CommonSettings::SignalsToUse::BOTH) {
    return true;
  } else if (signalsToUse == CommonSettings::SignalsToUse::WIFI) {
    return false;
  }
  return DEFAULT_USE_BLE;
}

RadioMeasurementBuffer createRadioMeasurementBuffer(
  const NavigationSettings& navProps) {
  long long sigAverageTime = (long long)(1000 * navProps.commonSettings.sigAveragingTime);
  long long sigWindowShift = (long long)(1000 * navProps.commonSettings.sigWindowShift);
  long long stopDetectTime = (long long)(1000 * navProps.commonSettings.stopDetectionTime);
  bool useStops = navProps.commonSettings.useStops;

  // TO DO: why RadioMeasurementBuffer is not initialized here
  if (sigWindowShift == 0 || sigWindowShift > sigAverageTime)
    sigWindowShift = sigAverageTime;
  
  long long keepRadioTimeMs = sigAverageTime - sigWindowShift;
  
  double rssiBias = 0;

  return RadioMeasurementBuffer(keepRadioTimeMs, sigWindowShift, stopDetectTime, rssiBias, useStops);
}

}

MeasurementsPreprocessor::MeasurementsPreprocessor(
    const std::shared_ptr<LevelCollector>& levelCollector,
    const NavigationSettings& navProps)
  : mLevelIndex(levelCollector)
  , mWiFiRttOffset(navProps.commonSettings.wiFiRttOffset)
  , mCutOffRssi(navProps.commonSettings.sigCutOffRss)
  , mUseClosestAps(navProps.commonSettings.useClosestAps)
  , mNumClosestAps(navProps.commonSettings.numClosestAps)
  , mUseUnknownTxs(navProps.commonSettings.useUnknownTransmitters)
  , mMeasurementType(navProps.commonSettings.measurementType)
  , mUseWifi(checkUseWifi(navProps))
  , mUseBle(checkUseBle(navProps))
  , mIsTracking(navProps.commonSettings.useTracking)
  , mRadiosBuffer(createRadioMeasurementBuffer(navProps))
  , mCurrentTs(0)
{ }

void MeasurementsPreprocessor::updateMeasurements(const Measurement& navMsg)
{
  mCurrentTs = navMsg.ts;
  boost::apply_visitor(*this, navMsg.data);
}

void MeasurementsPreprocessor::operator()(const RadioMeasurementData& measurement)
{
  RadioMeasurementData signal = measurement;
  boost::optional<RadioMeasurementData> validSignalData = getValidSignalEntry(signal);
  if (validSignalData.is_initialized())
  {
    RadioMeasurement validSignal;
    validSignal.data = validSignalData.get();
    validSignal.ts = mCurrentTs;
    mRadiosBuffer.addMeasurement(validSignal);
    mLastSignalTs = mCurrentTs;
  }
}

void MeasurementsPreprocessor::operator()(const SensorMeasurementData& measurement)
{
  mSensor.data = measurement;
  mSensor.ts = mCurrentTs;
}

void MeasurementsPreprocessor::operator()(const NmeaMeasurementData& measurement)
{
  mNmea.data = measurement;
  mNmea.ts = mCurrentTs;
}

RadioMeasurementsData MeasurementsPreprocessor::extractRadioMeasurements()
{
  RadioMeasurementsData radioMeasurements = mRadiosBuffer.extractMeasurements();
  if (mUseClosestAps)
  {
    std::sort(radioMeasurements.rbegin(), radioMeasurements.rend(),
           [](const RadioMeasurementData& lhs, const RadioMeasurementData& rhs)
           {
             return lhs.rssi < rhs.rssi;
           });

    if (radioMeasurements.size() > (unsigned int) mNumClosestAps)
      radioMeasurements.resize(mNumClosestAps);
  }

  for (auto& msr : radioMeasurements)
  {
    if (msr.type != RadioMeasurementData::Type::WIFI_RTT)
    {
      boost::optional<LevelId> levelId = mLevelIndex->findLevelByTransmitterId(msr.id);
      if (levelId.is_initialized())
      {
        const Level& level = mLevelIndex->level(levelId.get());
        Transmitter<XYZPoint> transmitter = level.transmitter(msr.id);
        const PathlossModel pathLossModel = transmitter.pathlossModel;
        msr.distance = std::exp((pathLossModel.A - msr.rssi) / pathLossModel.B);
      }
      continue;
    }

    msr.distance -= mWiFiRttOffset;
  }

  return radioMeasurements;
}

long long MeasurementsPreprocessor::getCurrentTs() const
{
  return mCurrentTs;
}

long long MeasurementsPreprocessor::getLastSignalTs() const
{
  return mLastSignalTs;
}

boost::optional<RadioMeasurementData> MeasurementsPreprocessor::getValidSignalEntry(
  const RadioMeasurementData& entry) const
{
  bool isTransmitterValid = mUseUnknownTxs
    || (mLevelIndex->findLevelByTransmitterId(entry.id).is_initialized()
      && isTransmitterSupported(entry.id));
  if (mMeasurementType != CommonSettings::MeasurementType::RSSI && entry.type != RadioMeasurementData::Type::WIFI_RTT)
    isTransmitterValid = false;
  if (mMeasurementType == CommonSettings::MeasurementType::RSSI && !isRssiValid(entry.rssi))
  {
    isTransmitterValid = false;
  }

  if (isTransmitterValid && isSignalTypeSupported(entry.type))
    return entry;
  else
    return boost::none;
}

bool MeasurementsPreprocessor::isSignalTypeSupported(RadioMeasurementData::Type signalType) const
{
  return (signalType == RadioMeasurementData::Type::WIFI      && mUseWifi) ||
         (signalType == RadioMeasurementData::Type::BEACON    && mUseBle)  ||
         (signalType == RadioMeasurementData::Type::BLUETOOTH && mUseBle) ||
         (signalType == RadioMeasurementData::Type::EDDYSTONE && mUseBle) ||
         (signalType == RadioMeasurementData::Type::WIFI_RTT  && mUseWifi);
}

bool MeasurementsPreprocessor::isRssiValid(double rssi) const
{
  return rssi < 0.0 && rssi > mCutOffRssi;
}

bool MeasurementsPreprocessor::isTransmitterSupported(const TransmitterId& transmitterId) const
{
  boost::optional<LevelId> levelId = mLevelIndex->findLevelByTransmitterId(transmitterId);
  if (!levelId.is_initialized())
    return false;

  if (!mLevelIndex->hasLevel(levelId.get()))
    return false;

  const Level& level = mLevelIndex->level(levelId.get());
  if (!level.containsTransmitter(transmitterId))
    return false;

  const Transmitter<XYZPoint>& transmitter = level.transmitter(transmitterId);
  return mIsTracking ? transmitter.type == TransmitterType::LOCATOR
                     : transmitter.type != TransmitterType::LOCATOR;
}

RadioMeasurementBuffer::RadioMeasurementBuffer(
    long long radioKeepPeriodMs,
    long long sigWindowShiftMs,
    long long stopDetectionTime,
    double rssiBias,
    bool useStops)
  : mRadioKeepPeriodMs(radioKeepPeriodMs)
  , mSignalWindowShiftMs(sigWindowShiftMs)
  , mStopDetectionTimeMs(stopDetectionTime)
  , mRssiBias(rssiBias)
  , mUseStops(useStops)
  , mLastExtractionTs(0)
  , mCurrentTs(0)
{ }

void RadioMeasurementBuffer::addMeasurement(const RadioMeasurement& msr)
{
  if (mCurrentTs == 0)
  {
    mCurrentTs = msr.ts;
    mLastExtractionTs = msr.ts;
  }

  RadioMeasurement radioMsr = msr;
  radioMsr.data.rssi = msr.data.rssi + mRssiBias;

  mMeasurements.push_back(radioMsr);
  mCurrentTs = msr.ts;
}

RadioMeasurementsData RadioMeasurementBuffer::extractMeasurements()
{
  RadioMeasurementsData measurements;

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
             return lhs.data.id < rhs.data.id;
           });

  auto sameIdFirst = mMeasurements.cbegin();
  while (sameIdFirst != mMeasurements.cend())
  {
    auto sameIdLast = std::upper_bound(sameIdFirst, mMeasurements.cend(), *sameIdFirst,
                                       [](const RadioMeasurement& lhs, const RadioMeasurement& rhs)
                                       {
                                         return lhs.data.id < rhs.data.id;
                                       });

    double nSignals = std::distance(sameIdFirst, sameIdLast);
    double sumRssi = std::accumulate(sameIdFirst, sameIdLast, 0.0,
                                     [](double sum, const RadioMeasurement& m)
                                     {
                                       return sum + m.data.rssi;
                                     });

    double averRssi = sumRssi / nSignals;

    double sumDistances = std::accumulate(sameIdFirst, sameIdLast, 0.0,
                                          [](double sum, const RadioMeasurement& m)
                                          {
                                            return sum + m.data.distance;
                                          });

    double averDistance = sumDistances / nSignals;

    RadioMeasurement radioMsr = *sameIdFirst;
    radioMsr.data.rssi = averRssi;
    radioMsr.data.distance = averDistance;
    measurements.push_back(radioMsr.data);
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

SensorMeasurement MeasurementsPreprocessor::getValidSensor() const
{
  return mSensor;
}

NmeaMeasurement MeasurementsPreprocessor::getCurrentNmea() const
{
  return mNmea;
}

#if defined (DEBUG_OUTPUT_MSR_PREPROCESSOR)
void clearDebugOutput()
{
  cout << "print debug output to " << PREPROCESSED_MSR_DEBUG_OUTPUT_FILE << endl;
  std::ofstream debugOutputFile;
  debugOutputFile.open(PREPROCESSED_MSR_DEBUG_OUTPUT_FILE);
  debugOutputFile << "type id ts rssi" << endl;
  debugOutputFile.close();
}

void printInputSignals(NavigationMessage* navMsg, RadioMeasurementsData& entries)
{
  ofstream debugOutputFile;
  debugOutputFile.open(PREPROCESSED_MSR_DEBUG_OUTPUT_FILE, ofstream::out | ofstream::app);
  for (RadioMeasurement& entry: entries)
  {
    long long wsTsSec = navMsg->tmUnixTime;
    long long wsTsMs = wsTsSec * 1000 - entry.offset;
    debugOutputFile << "INPUT "<< entry.bssid << " " << wsTsMs << " " << entry.data.rssi << endl;
  }
  debugOutputFile.close();
}

void printExtractedMeasurements(RadioMeasurementsData& radioMeasurements)
{
  ofstream debugOutputFile;
  debugOutputFile.open(PREPROCESSED_MSR_DEBUG_OUTPUT_FILE, ofstream::out | ofstream::app);
  for (RadioMeasurement msr: radioMeasurements)
  {
    debugOutputFile << "OUTPUT "<< msr.data.id << " " << msr.ts << " " << msr.data.rssi << endl;
  }
  debugOutputFile.close();
}

#endif

} } // namespace navigine::navigation_core
