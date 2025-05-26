#include <algorithm>
#include <functional>

#include "sampler.h"
#include "particle_filter_utils.h"

namespace navigine::navigation_core {

namespace {

const double DEVIATION_AROUND_CLOSEST_AP = 2.0;
const double SAMPLING_QUANTILE = 0.9;

} // namespace

Sampler::Sampler(
    const Level* level,
    const Likelihood* likelihood,
    Rng* rng)
    : level_(level)
    , likelihood_(likelihood)
    , rng_(rng)
{}

void Sampler::setAngleRange(std::optional<double> minAngle, std::optional<double> maxAngle)
{
    minAngle_ = minAngle;
    maxAngle_ = maxAngle;
}

void Sampler::setSpeedRange(std::optional<double> minSpeed, std::optional<double> maxSpeed)
{
    minSpeed_ = minSpeed;
    maxSpeed_ = maxSpeed;
}

bool Sampler::sampleParticles(std::vector<XYParticle>& particles)
{
    const auto sampleParticles = getSampleParticles(particles.size());

    if (sampleParticles.empty()) {
        return false;
    }

    for(size_t i = 0; i < particles.size(); i++) {
        particles[i] = XYParticle{
            .point = sampleParticles[i].point,
            .angle = sampleParticles[i].angle,
            .speed = sampleParticles[i].speed,
            .weight = 1.0 / particles.size()};
    }

    return true;
}

bool Sampler::mutateParticles(std::vector<XYParticle>& particles, unsigned int numParticlesToMutate)
{
    const auto sampleParticles = getSampleParticles(numParticlesToMutate);

    if (sampleParticles.empty()) {
        return false;
    }

    for(size_t i = 0; i < sampleParticles.size(); ++i) {
        const auto index = rng_->discrete(0, particles.size() - 1);
        particles[index] = XYParticle{
            .point = sampleParticles[i].point,
            .angle = sampleParticles[i].angle,
            .speed = sampleParticles[i].speed,
            .weight = 1.0 / particles.size()};
    }

    return true;
}

bool Sampler::sampleParticlesAroundCenters(std::vector<XYParticle>& particles, unsigned int numParticlesAroundCenters)
{
    const auto samplingCenters = likelihood_->calculateSamplingCenters();

    if (samplingCenters.empty()) {
        return false;
    }

    const auto nSamples = numParticlesAroundCenters / samplingCenters.size();

    for(const auto& center : samplingCenters) {
        for(unsigned int i = 0; i < nSamples; i++) {
            double x = rng_->normal(center.x, DEVIATION_AROUND_CLOSEST_AP);
            double y = rng_->normal(center.y, DEVIATION_AROUND_CLOSEST_AP);
            if (!level_->containsPoint(XYPoint{x, y})) {
                continue;
            }
            const auto angle = getRandomAngle();
            const auto speed = getRandomSpeed();
            const auto index = rng_->discrete(0, particles.size() - 1);
            particles[index] = XYParticle{
                .point = XYPoint{x, y},
                .angle = angle,
                .speed = speed,
                .weight = 1.0 / particles.size()};
        }
    }

    return true;
}

std::vector<XYParticle> Sampler::getSampleParticles(unsigned int sampleSize)
{
    const auto size = static_cast<unsigned int>(sampleSize / (1.0 - SAMPLING_QUANTILE));
    const auto samplePoints = level_->samplePoints(size, rng_);

    if (samplePoints.empty()) {
        return {};
    }

    std::vector<XYParticle> sampleParticles;
    for(size_t i = 0; i < samplePoints.size(); ++i) {
        const auto angle = getRandomAngle();
        const auto speed = getRandomSpeed();
        sampleParticles.push_back(XYParticle{
            .point = samplePoints[i],
            .angle = angle,
            .speed = speed,
            .weight = likelihood_->getLogWeight(samplePoints[i])});
    }

    std::stable_sort(sampleParticles.begin(), sampleParticles.end(),
        [](const auto& p1, const auto& p2) {
            return p1.weight > p2.weight;
        });

    sampleParticles.resize(sampleSize);

    return sampleParticles;
}

double Sampler::getRandomAngle()
{
    return minAngle_ && maxAngle_ ?
        NormalizedAngle(rng_->uniform(*minAngle_, *maxAngle_)) :
        NormalizedAngle(rng_->uniform(-M_PI, M_PI));
}

std::optional<double> Sampler::getRandomSpeed()
{
    if (!minSpeed_ || !maxSpeed_) {
        return std::nullopt;
    }

    return std::max(rng_->uniform(*minSpeed_, *maxSpeed_), 0.0);
}

} // namespace navigine::navigation_core
