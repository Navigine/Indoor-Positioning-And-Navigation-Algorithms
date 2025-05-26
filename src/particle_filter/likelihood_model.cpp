#include <numeric>

#include <navigine/navigation-core/math/math.h>

#include "likelihood_model.h"

namespace navigine::navigation_core {

namespace {

const auto DEFAULT_RSSI_DEVIATION = 5.0; // dBm
const auto DEFAULT_RTT_DEVIATION = 1.0;  // meters

const auto RTT_DISTRIBUTION_SKEW_COEFFICIENT = 5.0;
const auto RTT_MIN_DISTANCE = 1.0;

} // namespace

LikelihoodModel::LikelihoodModel(
    const Level* level,
    const PositionEstimatorSettings::ParticleFilter& pfSettings)
    : level_(level)
    , correctionNoiseRssi_(pfSettings.correctionNoiseRssi.value_or(PositionEstimatorSettings::ParticleFilter::DEFAULT_CORRECTION_NOISE_RSSI))
    , correctionNoiseRtt_(pfSettings.correctionNoiseRtt.value_or(PositionEstimatorSettings::ParticleFilter::DEFAULT_CORRECTION_NOISE_RTT))
    , correctionNoiseGps_(pfSettings.correctionNoiseGps.value_or(PositionEstimatorSettings::ParticleFilter::DEFAULT_CORRECTION_NOISE_GPS))
    , minMsrNumForUpdate_(pfSettings.minMsrNumForUpdate.value_or(PositionEstimatorSettings::ParticleFilter::DEFAULT_MIN_MSR_NUM_FOR_UPDATE))
    , minMsrNumForMutation_(pfSettings.minMsrNumForMutation.value_or(PositionEstimatorSettings::ParticleFilter::DEFAULT_MIN_MSR_NUM_FOR_MUTATION))
    , containsRtt_(false)
    , containsRssi_(false)
{}

void LikelihoodModel::setMeasurements(
    const RadioMeasurementsData& radioMsrs,
    const LocationMeasurementsData& locMsrs)
{
    radioMsrs_ = radioMsrs;
    locMsrs_ = locMsrs;

    containsRtt_ = false;
    containsRssi_ = false;
    for(const auto& msr: radioMsrs_) {
        containsRtt_ = containsRtt_ || isRttMeasurement(msr.type);
        containsRssi_ = containsRssi_ || isRssiMeasurement(msr.type);
    }
}

RadioMeasurementsData LikelihoodModel::getRadioMeasurements() const
{
    return radioMsrs_;
}

LocationMeasurementsData LikelihoodModel::getLocationMeasurements() const
{
    return locMsrs_;
}

double LikelihoodModel::getLogWeight(const XYPoint& point) const
{
    double weight = 0.0;

    for(const auto& msr: radioMsrs_) {
        const auto tx = level_->findTransmitter(msr.id);
        if (!tx) {
            continue;
        }

        if (isRttMeasurement(msr.type)) {
            // Evaluation of particles new weights multipliers for this Access Point
            const auto expectedDist = GetDist(tx->point, point);
            const auto measuredDist = std::max(msr.distance, RTT_MIN_DISTANCE);
            auto delta = measuredDist - expectedDist;
            if (delta < 0.0) {
                delta *= RTT_DISTRIBUTION_SKEW_COEFFICIENT;
            }
            const auto sigma = DEFAULT_RTT_DEVIATION * correctionNoiseRtt_;
            // Normal distribution formula with smoothing coeff. "measuredDist"
            weight -= delta * delta / (2.0 * sigma * sigma * measuredDist);
        }
        else if (isRssiMeasurement(msr.type)) {
            // Evaluation of  particles new weights multipliers for this Access Point
            const auto expectedRssi = RssiByDistance(
                tx->rssiModel.paramA,
                tx->rssiModel.paramB,
                GetDist(tx->point, point));
            const auto measuredRssi = msr.rssi;
            const auto delta = measuredRssi - expectedRssi;
            // Normal distribution formula with smoothing coeff. "correctionNoiseRssi"
            const auto sigma = tx->rssiModel.rssiDeviation.value_or(DEFAULT_RSSI_DEVIATION) * correctionNoiseRssi_;
            weight -= delta * delta / (2.0 * sigma * sigma);
        }
    }

    for(const auto& msr : locMsrs_) {
        const auto delta = GetDist(point, GeoToLocal(msr.point, level_->bindingPoint()));
        const auto sigma = msr.accuracy * correctionNoiseGps_;
        weight -= delta * delta / (2.0 * sigma * sigma);
    }

    return weight;
}

bool LikelihoodModel::hasSufficientMeasurementsForInit() const
{
    return !locMsrs_.empty() || !radioMsrs_.empty();
}

bool LikelihoodModel::hasSufficientMeasurementsForUpdate() const
{
    return !locMsrs_.empty() || radioMsrs_.size() >= minMsrNumForUpdate_;
}

bool LikelihoodModel::hasSufficientMeasurementsForMutation() const
{
    return !locMsrs_.empty() || radioMsrs_.size() >= minMsrNumForMutation_;
}

std::vector<XYPoint> LikelihoodModel::calculateSamplingCenters() const
{
    if (containsRtt_ || radioMsrs_.empty()) {
        return {};
    }

    const auto closestMsr = radioMsrs_.front();
    const auto closestTx = level_->findTransmitter(closestMsr.id);

    if (!closestTx) {
        return {};
    }

    return {closestTx->point};
}

} // namespace navigine::navigation_core
