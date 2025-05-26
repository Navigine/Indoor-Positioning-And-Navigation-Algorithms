#pragma once

#include <navigine/navigation_core/measurement.h>
#include <navigine/navigation_core/motion_info.h>
#include <navigine/navigation_core/graph_particle.h>
#include <navigine/navigation_core/navigation_settings.h>
#include <navigine/navigation_core/position.h>
#include <navigine/navigation_core/random.h>

#include "likelihood.h"
#include "navigation_graph.h"
#include "particle_filter_utils.h"

namespace navigine::navigation_core {

class GraphParticleFilter
{
public:
    GraphParticleFilter(
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

    std::vector<GraphParticle> getSampleParticles(size_t sampleSize) const;

private:
    const NavigationGraph graph_;
    const unsigned int numParticles_;
    const unsigned int numParticlesToMutate_;
    const unsigned int numParticlesAroundClosestTxs_;
    const unsigned int numParticlesAliveThreshold_;
    const double resamplingThreshold_;
    const double minDeviation_;

    std::shared_ptr<const Level> level_;
    std::shared_ptr<Rng> rng_;
    std::unique_ptr<Likelihood> likelihood_;

    std::vector<GraphParticle> particles_;

    bool initialized_;
};

} // namespace navigine::navigation_core
