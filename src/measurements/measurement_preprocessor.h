#ifndef MEASUREMENT_PREPROCESSOR_H
#define MEASUREMENT_PREPROCESSOR_H

#include <navigine/navigation-core/navigation_input.h>
#include <navigine/navigation-core/navigation_settings.h>
#include <navigine/navigation-core/level_collector.h>

#include <functional>
#include <vector>
#include <map>
#include <string>
#include <boost/optional.hpp>

#include "../measurement_types.h"

namespace navigine {
namespace navigation_core {

class RadioMeasurementBuffer
{
public:
  RadioMeasurementBuffer(const RadioMeasurementBuffer&) = default;

  RadioMeasurementBuffer(long long radioKeepPeriodMs,
                         long long sigWindowShiftMs,
                         long long stopDetectionTime,
                         double rssiBias,
                         bool useStops);

  RadioMeasurementsData extractMeasurements(long long lastStepTime);

  void addMeasurement(const RadioMeasurement& msr);

private:
  const long long mRadioKeepPeriodMs;
  const long long mSignalWindowShiftMs;
  const long long mStopDetectionTimeMs;
  const double mRssiBias;
  const bool mUseStops;

  long long mLastExtractionTs;
  long long mCurrentTs;
  long long mLastStepTs;

  std::vector<RadioMeasurement> mMeasurements = {};
};

class MeasurementsPreprocessor: public boost::static_visitor<>
{
  public:
    MeasurementsPreprocessor(
      const std::shared_ptr<LevelCollector>& levelCollector,
      const NavigationSettings& navProps);

    
    void updateMeasurements(const Measurement& navMsg);
    long long getCurrentTs() const;
    long long getLastSignalTs() const;
    RadioMeasurementsData extractRadioMeasurements(long long lastStepTime);
    SensorMeasurement getValidSensor() const;
    NmeaMeasurement getCurrentNmea() const;

    void operator()(const RadioMeasurementData& measurement);
    void operator()(const SensorMeasurementData& measurement);
    void operator()(const NmeaMeasurementData& measurement);

  private:
    boost::optional<RadioMeasurementData> getValidSignalEntry(const RadioMeasurementData& entry) const;

    bool isSignalTypeSupported(RadioMeasurementData::Type signalType) const;
    bool isRssiValid(double rssi) const;
    std::pair<bool, LevelId> transmitterLevelId(const TransmitterId& transmitterId) const;
    bool isTransmitterSupported(const TransmitterId& transmitterId) const;

  private:
    const std::shared_ptr<LevelCollector> mLevelIndex;
    const double mWiFiRttOffset;
    const double mCutOffRssi;
    const bool mUseClosestAps;
    const int mNumClosestAps;
    const bool mUseUnknownTxs;
    const CommonSettings::MeasurementType mMeasurementType;
    const bool mUseWifi;
    const bool mUseBle;
    const bool mIsTracking;
    long long mLastSignalTs;

    RadioMeasurementBuffer mRadiosBuffer;
    long long mCurrentTs;

    SensorMeasurement mSensor;
    NmeaMeasurement mNmea;
};

} } // namespace navigine::navigation_core


#endif // MEASUREMENT_PREPROCESSOR_H
