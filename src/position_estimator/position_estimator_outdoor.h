#pragma once

#include <navigine/navigation-core/geometry.h>
#include <navigine/navigation-core/level.h>
#include <navigine/navigation-core/measurement.h>
#include <navigine/navigation-core/navigation_settings.h>
#include <navigine/navigation-core/position.h>
#include <navigine/navigation-core/position_estimator.h>
#include <navigine/navigation-core/utils.h>

#include "../level_evaluator.h"
#include "../position_postprocessor/position_postprocessor.h"

#include "altitude_estimator.h"
#include "barrier_calculator.h"
// #include "kalman_filter/kalman_filter.h"
#include "location_filter.h"
#include "sigma_filter.h"

namespace navigine::navigation_core {

class PositionEstimatorOutdoor : public PositionEstimator
{
public:
    PositionEstimatorOutdoor(
        const std::shared_ptr<KalmanFilter>& kalmanFilter,
        const std::shared_ptr<const Level>& level,
        const PositionEstimatorSettings& settings);

    void addMeasurement(const Measurement& msr) override;

    std::optional<Position> getPosition(const Timestamp& ts) override;

    double getEvaluation() const override;

    std::shared_ptr<State> getState(const StateParams& params) const override;

private:
    void addSensorMeasurement(const Timestamp& ts, const SensorMeasurementData& data);
    void addLocationMeasurement(const Timestamp& ts, const LocationMeasurementData& data);
    void addNmeaMeasurement(const Timestamp& ts, const NmeaMeasurementData& data);
    void addOdometerMeasurement(const Timestamp& ts, const OdometerMeasurementData& data);

    unsigned int getNumSats(const Timestamp& ts) const;

    void updatePosition();
    void reset();

    std::optional<double> getDeadReckoningTime() const;

private:
    const double deadReckoningTimeThreshold_;
    const double motionSpeed_;

    std::shared_ptr<const Level> level_;
    std::unique_ptr<LocationFilter> locationFilter_;
    //std::shared_ptr<KalmanFilter> kalmanFilter_;
    std::unique_ptr<PositionPostprocessor> postprocessor_;
    std::unique_ptr<LevelEvaluator> evaluator_;
    std::unique_ptr<AltitudeEstimator> altitudeEstimator_;
    std::unique_ptr<BarrierCalculator> barrierCalculator_;

    std::optional<Timestamp> currentTs_;            // Current timestamp
    std::optional<NmeaMeasurement> nmeaMsr_;        // Last unfiltered NMEA measurement
    std::optional<LocationMeasurement> locMsr_;     // Last unfiltered location measurement
    std::optional<Position> position_;              // Current position
};

} // namespace navigine::navigation_core
