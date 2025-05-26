#pragma once

#include <navigine/navigation-core/level.h>
#include <navigine/navigation-core/position_estimator.h>
#include <navigine/navigation-core/utils.h>

#include "../level_evaluator.h"
#include "../measurement_preprocessor.h"
#include "../position_postprocessor/position_postprocessor.h"
#include "../sensor_fusion/sensor_fusion.h"

#include "barrier_calculator.h"

namespace navigine::navigation_core {

class PositionEstimatorZone : public PositionEstimator
{
public:
    PositionEstimatorZone(
        const std::shared_ptr<const Level>& level,
        const PositionEstimatorSettings& settings);

    void addMeasurement(const Measurement& msr) override;

    std::optional<Position> getPosition(const Timestamp& ts) override;

    double getEvaluation() const override;

    std::shared_ptr<State> getState(const StateParams& params) const override;

private:
    void addRadioMeasurement(const RadioMeasurement& radioMsr);
    void addSensorMeasurement(const SensorMeasurement& sensorMsr);

    void updatePosition();
    void reset();

    std::optional<double> getDeadReckoningTime() const;

private:
    const bool useStopDetection_;
    const double sigAveragingTime_;             // seconds
    const double updateInterval_;               // seconds
    const double deadReckoningTimeThreshold_;   // seconds

    std::shared_ptr<const Level> level_;
    std::unique_ptr<MeasurementsPreprocessor> msrPreprocessor_;
    std::unique_ptr<SensorFusion> sensorFusion_;
    std::unique_ptr<PositionPostprocessor> postprocessor_;
    std::unique_ptr<LevelEvaluator> evaluator_;
    std::unique_ptr<BarrierCalculator> barrierCalculator_;

    std::optional<Timestamp> currentTs_;
    std::optional<Timestamp> updatePositionTs_;
    std::optional<Position> position_;
    RadioMeasurementsData radioMsrs_;
};

} // namespace navigine::navigation_core
