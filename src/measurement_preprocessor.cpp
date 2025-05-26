#include <algorithm>
#include <iterator>
#include <numeric>

#include <navigine/navigation-core/value_statistics.h>

#include "measurement_preprocessor.h"

namespace navigine::navigation_core {

namespace {

const int MAX_BUFFER_DURATION_MS = 30000;
const int MAX_BUFFER_SIZE = 100;

} // namespace

///
/// RadioMeasurementBuffer
///

void RadioMeasurementBuffer::addMeasurement(const RadioMeasurement& radioMsr)
{
    measurements_.push_back(radioMsr);
    eraseExpiredMeasurements();
}

RadioMeasurementsData RadioMeasurementBuffer::extractMeasurements(const Timestamp& fromTs) const
{
    static const auto compare =
        [](const RadioMeasurement& radioMsr, Timestamp ts) {
            return radioMsr.ts < ts;
        };

    // Averaging measurements from same transmitters
    std::map<std::pair<TransmitterId, RadioMeasurementData::Type>, ValueStatistics> txMap;

    for(auto iter = std::lower_bound(measurements_.begin(), measurements_.end(), fromTs, compare); iter != measurements_.end(); ++iter) {
        const auto key = std::make_pair(iter->data.id, iter->data.type);
        auto& value = txMap[key];
        if (iter->data.type == RadioMeasurementData::Type::WIFI_RTT) {
            value.addValue(std::max(iter->data.distance, 0.1));
        } else {
            value.addValue(iter->data.rssi);
        }
    }

    RadioMeasurementsData measurements;

    for(const auto& [key, value] : txMap) {
        const auto averageRssi = key.second == RadioMeasurementData::Type::WIFI_RTT ? 0.0 : *value.average();
        const auto averageDist = key.second == RadioMeasurementData::Type::WIFI_RTT ? *value.average() : 0.0;
        measurements.push_back(RadioMeasurementData{
            .type = key.second,
            .id = key.first,
            .rssi = averageRssi,
            .distance = averageDist});
    }

    return measurements;
}

void RadioMeasurementBuffer::reset()
{
    measurements_.clear();
}

void RadioMeasurementBuffer::eraseExpiredMeasurements()
{
    if (measurements_.size() <= MAX_BUFFER_SIZE) {
        return;
    }

    const auto maxTs = measurements_.back().ts;
    const auto minTs = maxTs - MAX_BUFFER_DURATION_MS;

    // Erasing all measurements with timestamps less than minTs
    while (measurements_.size() > MAX_BUFFER_SIZE && measurements_.front().ts < minTs) {
        measurements_.pop_front();
    }
}

///
/// MeasurementsPreprocessor
///

MeasurementsPreprocessor::MeasurementsPreprocessor(
    const std::shared_ptr<const Level>& level,
    const PositionEstimatorSettings::Common& settings,
    double sigAveragingTime)
    : level_(level)
    , correctionOffsetRssi_(settings.correctionOffsetRssi.value_or(PositionEstimatorSettings::Common::DEFAULT_CORRECTION_OFFSET_RSSI))
    , correctionOffsetRtt_(settings.correctionOffsetRtt.value_or(PositionEstimatorSettings::Common::DEFAULT_CORRECTION_OFFSET_RTT))
    , useBle_(settings.useBle.value_or(PositionEstimatorSettings::Common::DEFAULT_USE_BLE))
    , useWifi_(settings.useWifi.value_or(PositionEstimatorSettings::Common::DEFAULT_USE_WIFI))
    , useWifiRtt_(settings.useWifiRtt.value_or(PositionEstimatorSettings::Common::DEFAULT_USE_WIFI_RTT))
    , cutOffRssi_(settings.cutOffRssi.value_or(PositionEstimatorSettings::Common::DEFAULT_CUTOFF_RSSI))
    , useTxRssiThreshold_(settings.useTxRssiThreshold.value_or(PositionEstimatorSettings::Common::DEFAULT_USE_TX_RSSI_THRESHOLD))
{
    setAveragingTime(sigAveragingTime);
}

void MeasurementsPreprocessor::setAveragingTime(double sigAveragingTime)
{
    sigAveragingTimeMs_ = std::max(static_cast<int>(sigAveragingTime * 1000), 1);
}

std::optional<Timestamp> MeasurementsPreprocessor::getLastSignalTs() const
{
    return lastSignalTs_;
}

void MeasurementsPreprocessor::addMeasurement(const RadioMeasurement& radioMsr)
{
    if (!checkMeasurement(radioMsr.data)) {
        return;
    }

    lastSignalTs_ = radioMsr.ts;

    buffer_.addMeasurement(radioMsr);
}

RadioMeasurementsData MeasurementsPreprocessor::extractMeasurements(const Timestamp& ts)
{
    auto radioMsrs = buffer_.extractMeasurements(ts - sigAveragingTimeMs_);

    for(auto& radioMsr : radioMsrs) {
        auto tx = level_->findTransmitter(radioMsr.id);
        if (isRssiMeasurement(radioMsr.type)) {
            // Calculating model distance for RSSI measurements
            radioMsr.rssi += correctionOffsetRssi_;
            radioMsr.distance = DistanceByRssi(
                tx->rssiModel.paramA,
                tx->rssiModel.paramB,
                radioMsr.rssi);
        } else if (isRttMeasurement(radioMsr.type)) {
            // Calculating virtual RSSI for distance measurements
            radioMsr.distance += correctionOffsetRtt_;
            radioMsr.rssi = RssiByDistance(
                tx->rssiModel.paramA,
                tx->rssiModel.paramB,
                radioMsr.distance);
        }
    }

    return radioMsrs;
}

RadioMeasurementsData MeasurementsPreprocessor::extractMeasurements(
    const Timestamp& fromTs,
    const RadioMeasurementFilter& filter,
    const RadioMeasurementCompare& compare,
    const unsigned int maxMsrCount) const
{
    RadioMeasurementsData radioMsrs;

    for(auto radioMsr : buffer_.extractMeasurements(fromTs)) {
        auto tx = level_->findTransmitter(radioMsr.id);
        if (isRssiMeasurement(radioMsr.type)) {
            // Calculating model distance for RSSI measurements
            radioMsr.rssi += correctionOffsetRssi_;
            radioMsr.distance = DistanceByRssi(
                tx->rssiModel.paramA,
                tx->rssiModel.paramB,
                radioMsr.rssi);
        } else if (isRttMeasurement(radioMsr.type)) {
            // Calculating virtual RSSI for distance measurements
            radioMsr.distance += correctionOffsetRtt_;
            radioMsr.rssi = RssiByDistance(
                tx->rssiModel.paramA,
                tx->rssiModel.paramB,
                radioMsr.distance);
        }

        if (!filter || filter(radioMsr)) {
            radioMsrs.push_back(std::move(radioMsr));
        }
    }

    if (compare) {
        std::stable_sort(radioMsrs.begin(), radioMsrs.end(), compare);
    }

    if (maxMsrCount && radioMsrs.size() > maxMsrCount) {
        radioMsrs.resize(maxMsrCount);
    }

    return radioMsrs;
}

void MeasurementsPreprocessor::reset()
{
    buffer_.reset();
    lastSignalTs_ = std::nullopt;
}

bool MeasurementsPreprocessor::checkMeasurement(const RadioMeasurementData& radioMsr) const
{
    const auto tx = level_->findTransmitter(radioMsr.id);
    if (!tx) {
        return false;
    }

    switch (radioMsr.type) {
        case RadioMeasurementData::Type::WIFI:
            return useWifi_ && checkRssi(radioMsr.rssi, tx->rssiModel.rssiThreshold);

        case RadioMeasurementData::Type::BEACON:
        case RadioMeasurementData::Type::BLUETOOTH:
        case RadioMeasurementData::Type::EDDYSTONE:
            return useBle_ && checkRssi(radioMsr.rssi, tx->rssiModel.rssiThreshold);

        case RadioMeasurementData::Type::WIFI_RTT:
            return useWifiRtt_;

        default:
            return false;
    }
}

bool MeasurementsPreprocessor::checkRssi(double rssi, const std::optional<double>& rssiThreshold) const
{
    if (rssi >= 0 || rssi <= cutOffRssi_) {
        return false;
    }

    if (rssiThreshold && useTxRssiThreshold_ && rssi < *rssiThreshold) {
        return false;
    }

    return true;
}

} // namespace navigine::navigation_core
