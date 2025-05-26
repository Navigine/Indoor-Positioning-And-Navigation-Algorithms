#pragma once

#include <navigine/navigation-core/level.h>
#include <navigine/navigation-core/measurement.h>
#include <navigine/navigation-core/navigation_settings.h>
#include <navigine/navigation-core/transmitter.h>

namespace navigine::navigation_core {

class RadioMeasurementBuffer
{
public:
    RadioMeasurementBuffer() = default;

    void addMeasurement(const RadioMeasurement& msr);

    // Extract all measurements from the specified timestamp
    RadioMeasurementsData extractMeasurements(const Timestamp& fromTs) const;

    void reset();

private:
    void eraseExpiredMeasurements();
    std::deque<RadioMeasurement> measurements_;
};

class MeasurementsPreprocessor
{
public:
    MeasurementsPreprocessor(
        const std::shared_ptr<const Level>& level,
        const PositionEstimatorSettings::Common& settings,
        const double sigAveragingTime);

    void setAveragingTime(double sigAveragingTime);

    // Get the last valid signal timestamp
    std::optional<Timestamp> getLastSignalTs() const;

    void addMeasurement(const RadioMeasurement& msr);

    RadioMeasurementsData extractMeasurements(const Timestamp& ts);

    using RadioMeasurementFilter = std::function<bool(const RadioMeasurementData& msr)>;
    using RadioMeasurementCompare = std::function<bool(const RadioMeasurementData& msr1, const RadioMeasurementData& msr2)>;

    RadioMeasurementsData extractMeasurements(
        const Timestamp& fromTs,
        const RadioMeasurementFilter& filter,
        const RadioMeasurementCompare& compare,
        const unsigned int maxMsrCount = 0) const;

    void reset();

private:
    bool checkMeasurement(const RadioMeasurementData& radioMsr) const;
    bool checkRssi(double rssi, const std::optional<double>& rssiThreshold) const;

private:
    const std::shared_ptr<const Level> level_;
    const double correctionOffsetRssi_;
    const double correctionOffsetRtt_;
    const bool useBle_;
    const bool useWifi_;
    const bool useWifiRtt_;
    const double cutOffRssi_;
    const bool useTxRssiThreshold_;

    int sigAveragingTimeMs_; // Size of signal averaging window (in milliseconds)

    RadioMeasurementBuffer buffer_;
    std::optional<Timestamp> lastSignalTs_;
};

} // namespace navigine::navigation_core
