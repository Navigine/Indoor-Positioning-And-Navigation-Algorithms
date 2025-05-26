#include <navigine/navigation-core/math/math.h>

#include "position_estimator_zone.h"

namespace navigine::navigation_core {

PositionEstimatorZone::PositionEstimatorZone(
    const std::shared_ptr<const Level>& level,
    const PositionEstimatorSettings& settings)
    : useStopDetection_(settings.motionModel.useStopDetection.value_or(PositionEstimatorSettings::MotionModel::DEFAULT_USE_STOP_DETECTION))
    , sigAveragingTime_(settings.zoneSolver.sigAveragingTime.value_or(PositionEstimatorSettings::ZoneSolver::DEFAULT_SIG_AVERAGING_TIME))
    , updateInterval_(settings.zoneSolver.updateInterval.value_or(PositionEstimatorSettings::ZoneSolver::DEFAULT_UPDATE_INTERVAL))
    , deadReckoningTimeThreshold_(settings.common.deadReckoningTimeThreshold.value_or(
        PositionEstimatorSettings::Common::DEFAULT_DEAD_RECKONING_TIME_THRESHOLD))
    , level_(level)
    , msrPreprocessor_(std::make_unique<MeasurementsPreprocessor>(level_, settings.common, sigAveragingTime_))
    , sensorFusion_(std::make_unique<SensorFusion>(settings.motionModel))
    , postprocessor_(std::make_unique<PositionPostprocessor>(level_, settings.motionModel, settings.postprocessor))
    , evaluator_(std::make_unique<LevelEvaluator>(level_, settings.common, settings.levelEvaluation))
    , barrierCalculator_(std::make_unique<BarrierCalculator>(level_))
{}

std::optional<Position> PositionEstimatorZone::getPosition(const Timestamp& ts)
{
    if (currentTs_ && *currentTs_ > ts) {
        throw std::runtime_error("PositionEstimator: new time SHOULD NOT be less than the current time");
    }

    currentTs_ = ts;

    if (!updatePositionTs_) {
        updatePositionTs_ = ts;
    }

    if (*currentTs_ - *updatePositionTs_ >= updateInterval_ * 1000) {
        updatePosition();
        updatePositionTs_ = currentTs_;
    }

    return position_;
}

double PositionEstimatorZone::getEvaluation() const
{
    return currentTs_ ? evaluator_->getEvaluation(*currentTs_) : 0.0;
}

std::shared_ptr<PositionEstimator::State> PositionEstimatorZone::getState(const StateParams& params) const
{
    std::vector<RadioMeasurementData> radioMsrs;

    if (params.calculateMeasurements) {
        radioMsrs = radioMsrs_;
    }

    barrierCalculator_->updatePosition(position_);

    return std::make_shared<PositionEstimator::State>(State{
        .levelId = level_->id(),
        .deadReckoningTime = getDeadReckoningTime(),
        .radioMsrs = std::move(radioMsrs),
        .locMsrs = {},
        .particles = {},
        .message = std::string(),
        .evaluation = getEvaluation(),
        .barrierHit = barrierCalculator_->getBarrierHit(),
        .barrierIntersection = barrierCalculator_->getBarrierIntersection(),
    });
}

void PositionEstimatorZone::addMeasurement(const Measurement& msr)
{
    if (currentTs_ && *currentTs_ > msr.ts) {
        throw std::runtime_error("PositionEstimator: new measurement time SHOULD NOT be less than the current time");
    }

    currentTs_ = msr.ts;

    if (!updatePositionTs_) {
        updatePositionTs_ = msr.ts;
    }

    evaluator_->addMeasurement(msr);

    switch (msr.data.index()) {
        case RADIO_MEASUREMENT_DATA_INDEX:
            addRadioMeasurement(RadioMeasurement{
                .data = std::get<RadioMeasurementData>(msr.data),
                .ts = msr.ts});
            break;

        case SENSOR_MEASUREMENT_DATA_INDEX:
            addSensorMeasurement(SensorMeasurement{
                .data = std::get<SensorMeasurementData>(msr.data),
                .ts = msr.ts});
            // TODO: update position heading here
            break;

        default:
            break;
    }

    if (*currentTs_ - *updatePositionTs_ >= updateInterval_ * 1000) {
        updatePosition();
        updatePositionTs_ = currentTs_;
    }
}

void PositionEstimatorZone::addRadioMeasurement(const RadioMeasurement& radioMsr)
{
    msrPreprocessor_->addMeasurement(radioMsr);
}

void PositionEstimatorZone::addSensorMeasurement(const SensorMeasurement& sensorMsr)
{
    sensorFusion_->addMeasurement(sensorMsr);
}

void PositionEstimatorZone::updatePosition()
{
    NC_ASSERT(currentTs_);

    if (getDeadReckoningTime() >= deadReckoningTimeThreshold_) {
        reset();
        return;
    }

    const auto motionInfo = sensorFusion_->calculateMotion(*currentTs_);

    // Use stops: in case of detected stops increasing:
    //  * signal averaging time;
    //  * number of estimated best signals.
    // So that the resulting position is more stable
    const auto stopFactor = useStopDetection_ ?
        1.0 + motionInfo.stopTime / sigAveragingTime_ :
        1.0;

    const auto sigAveragingTimeMs = static_cast<int>(std::round(sigAveragingTime_ * stopFactor * 1000));

    radioMsrs_ = msrPreprocessor_->extractMeasurements(*currentTs_ - sigAveragingTimeMs,
        nullptr, // No signal filtering
        [](const auto& msr1, const auto& msr2) {
            return msr1.distance < msr2.distance;
        });

    if (radioMsrs_.empty()) {
        // No measurements for the solution
        return;
    }

    const auto nearestMsr = radioMsrs_.front();
    const auto nearestTx = level_->findTransmitter(nearestMsr.id);

    NC_ASSERT(nearestTx);

    position_ = Position{
        .point = LocalToGeo(nearestTx->point, level_->bindingPoint()),
        .accuracy = std::max(nearestMsr.distance, 1.0),
        .levelId = level_->id(),
        .timestamp = *currentTs_};

    position_ = postprocessor_->processPosition(*position_);
}

void PositionEstimatorZone::reset()
{
    msrPreprocessor_->reset();
    sensorFusion_->reset();
    //TODO: reset postprocessor
    evaluator_->reset();
    barrierCalculator_->reset();

    currentTs_ = std::nullopt;
    updatePositionTs_ = std::nullopt;
    position_ = std::nullopt;
    radioMsrs_.clear();
}

std::optional<double> PositionEstimatorZone::getDeadReckoningTime() const
{
    const auto lastSignalTs = msrPreprocessor_->getLastSignalTs();
    return lastSignalTs && currentTs_ ?
        std::make_optional(GetDeltaTimeSec(*currentTs_, *lastSignalTs)) :
        std::nullopt;
}

} // namespace navigine::navigation_core
