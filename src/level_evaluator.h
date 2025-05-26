#pragma once

#include <navigine/navigation-core/level.h>
#include <navigine/navigation-core/measurement.h>
#include <navigine/navigation-core/navigation_settings.h>
#include <navigine/navigation-core/utils.h>
#include <navigine/navigation-core/value_estimator.h>

#include "measurement_preprocessor.h"

namespace navigine::navigation_core {

class LevelEvaluator
{
public:
    LevelEvaluator(
        const std::shared_ptr<const Level>& level,
        const PositionEstimatorSettings::Common& commonSettings,
        const PositionEstimatorSettings::LevelEvaluation& evalSettings);

    double getEvaluation(const Timestamp& ts);

    void addMeasurement(const Measurement& msr);

    void reset();

private:
    void addRadioMeasurement(const RadioMeasurement& radioMsr);
    void addLocationMeasurement(const LocationMeasurement& locMsr);
    void updateEvaluation();

private:
    const unsigned int numClosestTxs_;
    const double sigAveragingTime_;     // seconds
    const double updateInterval_;       // seconds
    const double rssiTxBonus_;
    const double weightRssi_;
    const double weightGps_;
    const double weightExternal_;
    const bool useGps_;

    std::shared_ptr<const Level> level_;
    std::unique_ptr<MeasurementsPreprocessor> msrBuffer_;
    std::unique_ptr<ValueEstimator> gpsEstimator_;
    std::unique_ptr<ValueEstimator> externalEstimator_;
    std::optional<Timestamp> currentTs_;
    std::optional<Timestamp> updateTs_;

    double evaluation_;
};

} // namespace navigine::navigation_core
