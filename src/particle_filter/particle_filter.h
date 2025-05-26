#pragma once

#include <navigine/navigation-core/level.h>
#include <navigine/navigation-core/measurement.h>
#include <navigine/navigation-core/motion_info.h>
#include <navigine/navigation-core/navigation_settings.h>
#include <navigine/navigation-core/position.h>
#include <navigine/navigation-core/random.h>

#include "sampler.h"
#include "likelihood.h"
#include "particle_filter_utils.h"

namespace navigine::navigation_core {

class ParticleFilter
{
public:
    ParticleFilter(
        const std::shared_ptr<const Level>& level,
        const std::shared_ptr<Rng>& rng,
        const PositionEstimatorSettings::ParticleFilter& pfSettings);

    RadioMeasurementsData getRadioMeasurements() const;
    LocationMeasurementsData getLocationMeasurements() const;

    std::vector<GeoParticle> getParticles() const;

    std::optional<SolutionPF> getSolution() const;

    // Returns true iff update-step was made
    bool performEstimation(
        const RadioMeasurementsData& radioMsrs,
        const LocationMeasurementsData& locMsrs,
        const MotionInfo& motionInfo);

    void reset();

private:
    void init();
    void update();
    void mutate();
    void resample();
    void move(const MotionInfo& motionInfo);

    bool hasGoodHeading() const;

private:
    const bool useHeadingCorrection_;

    const unsigned int numParticles_;
    const unsigned int numParticlesToMutate_;
    const unsigned int numParticlesAroundClosestTxs_;
    const unsigned int numParticlesAliveThreshold_;
    const double resamplingThreshold_;
    const double wallPassingProbability_;
    const double minDeviation_;

    std::shared_ptr<const Level> level_;
    std::shared_ptr<Rng> rng_;
    std::unique_ptr<Likelihood> likelihood_;
    std::unique_ptr<Sampler> sampler_;

    std::vector<XYParticle> particles_;

    bool initialized_;
};

} // namespace navigine::navigation_core
