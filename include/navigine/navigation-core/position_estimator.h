#pragma once

#include <navigine/navigation-core/level.h>
#include <navigine/navigation-core/measurement.h>
#include <navigine/navigation-core/navigation_settings.h>
#include <navigine/navigation-core/particle.h>
#include <navigine/navigation-core/position.h>
// #include <navigine/navigation-core/random.h>

namespace navigine::navigation_core {

class PositionEstimator
{
public:
    struct StateParams {
        bool calculateParticles = true;
        bool calculateMeasurements = true;
    };

    struct State {
        std::optional<LevelId> levelId;
        std::optional<double> deadReckoningTime;
        std::vector<RadioMeasurementData> radioMsrs;
        std::vector<LocationMeasurementData> locMsrs;
        std::vector<GeoParticle> particles;
        std::string message;
        double evaluation;
        bool barrierHit;
        bool barrierIntersection;
    };

    virtual ~PositionEstimator() = default;

    virtual void addMeasurement(const Measurement& m) = 0;

    virtual std::optional<Position> getPosition(const Timestamp& ts) = 0;

    virtual double getEvaluation() const = 0;

    virtual std::shared_ptr<State> getState(const StateParams& params) const = 0;
};

std::shared_ptr<PositionEstimator> createPositionEstimator(
    const std::shared_ptr<const Level>& level,
    const PositionEstimatorSettings& settings,
    const std::shared_ptr<Rng>& rng);

std::shared_ptr<PositionEstimator> createPositionEstimator(
    const std::shared_ptr<const Level>& level,
    const PositionEstimatorSettings& settings);

} // namespace navigine::navigation_core
