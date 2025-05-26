#pragma once

#include <navigine/navigation-core/level.h>
#include <navigine/navigation-core/measurement.h>
#include <navigine/navigation-core/navigation_settings.h>
#include <navigine/navigation-core/particle.h>
#include <navigine/navigation-core/random.h>

#include "likelihood.h"

namespace navigine::navigation_core {


class Sampler
{
public:
    Sampler(
        const Level* level,
        const Likelihood* likelihood,
        Rng* rng);

    void setAngleRange(std::optional<double> minAngle, std::optional<double> maxAngle);
    void setSpeedRange(std::optional<double> minSpeed, std::optional<double> maxSpeed);

    bool sampleParticles(std::vector<XYParticle>& particles);
    bool mutateParticles(std::vector<XYParticle>& particles, unsigned int numParticlesToMutate);
    bool sampleParticlesAroundCenters(std::vector<XYParticle>& particles, unsigned int numParticlesAroundCenters);

private:
    std::vector<XYParticle> getSampleParticles(unsigned int sampleSize);

    double getRandomAngle();
    std::optional<double> getRandomSpeed();

private:
    const Level* level_;
    const Likelihood* likelihood_;
    Rng* rng_;

    std::optional<double> minAngle_;
    std::optional<double> maxAngle_;
    std::optional<double> minSpeed_;
    std::optional<double> maxSpeed_;
};

} // namespace navigine::navigation_core
