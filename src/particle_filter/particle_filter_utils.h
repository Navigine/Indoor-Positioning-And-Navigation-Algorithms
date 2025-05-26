#pragma once

#include <algorithm>

#include <navigine/navigation-core/geometry.h>
#include <navigine/navigation-core/particle.h>
#include <navigine/navigation-core/random.h>
#include <navigine/navigation-core/math/math.h>

#include "likelihood.h"

namespace navigine::navigation_core {

struct SolutionPF
{
    XYPoint point;
    double accuracy;
    std::optional<double> speed;
    std::optional<double> trackAngle;
    std::optional<double> heading;
    std::optional<double> headingAccuracy;
};

template <typename ParticleType>
std::optional<SolutionPF> getSolutionFromParticles(
    const std::vector<ParticleType>& particles,
    double minDeviation)
{
    double sumW = 0;    // sum of weights
    double sumX = 0;    // sum of x
    double sumY = 0;    // sum of y
    double sumAX = 0;   // sum of radius-vectors, x-component
    double sumAY = 0;   // sum of radius-vectors, y-component
    double sumVX = 0;   // sum of velocity-vectors, x-component
    double sumVY = 0;   // sum of velocity-vectors, y-component
    double sumWS = 0;

    for(const auto& p : particles) {
        const auto& point = p.point;
        const auto sinA = math::sin(p.angle);
        const auto cosA = math::cos(p.angle);
        sumW += p.weight;
        sumX += point.x * p.weight;
        sumY += point.y * p.weight;
        sumAX += sinA * p.weight;
        sumAY += cosA * p.weight;
        if (p.speed) {
            sumVX += sinA * *p.speed * p.weight;
            sumVY += cosA * *p.speed * p.weight;
            sumWS += p.weight;
        }
    }

    if (sumW < EPSILON) {
        return std::nullopt;
    }

    const auto speedAvailable = sumWS >= EPSILON;

    sumX /= sumW;
    sumY /= sumW;
    sumAX /= sumW;
    sumAY /= sumW;
    sumVX /= sumWS;
    sumVY /= sumWS;

    const auto point = XYPoint{sumX, sumY};
    const auto heading = math::atan2(sumAX, sumAY);

    const auto speed = math::sqrt(sumVX * sumVX + sumVY * sumVY);
    const auto trackAngle = math::atan2(sumVX, sumVY);

    double deviation = 0.0;
    double headingDeviation = 0.0;
    double trackAngleDeviation = 0.0;

    // Calculating deviation
    for(const auto& p : particles) {
        const auto error = GetDist(point, p.point);
        const auto headingError = NormalizedAngle(p.angle - heading);
        deviation += error * error * p.weight / sumW;
        headingDeviation += headingError * headingError * p.weight / sumW;
        if (p.speed) {
            const auto trackAngleError = NormalizedAngle(p.angle - trackAngle);
            trackAngleDeviation += trackAngleError * trackAngleError * p.weight / sumWS;
        }
    }

    deviation = math::sqrt(deviation);
    headingDeviation = math::sqrt(headingDeviation);
    trackAngleDeviation = math::sqrt(trackAngleDeviation);

    const auto headingAvailable = headingDeviation < M_PI / 2;
    const auto trackAngleAvailable = speedAvailable && trackAngleDeviation < M_PI / 2;

    return SolutionPF{
        .point = point,
        .accuracy = std::max(deviation, minDeviation),
        .speed = speedAvailable ?
            std::make_optional(speed) :
            std::nullopt,
        .trackAngle = trackAngleAvailable ?
            std::make_optional(trackAngle) :
            std::nullopt,
        .heading = headingAvailable ?
            std::make_optional(heading) :
            std::nullopt,
        .headingAccuracy = headingAvailable ?
            std::make_optional(headingDeviation) :
            std::nullopt,
    };
}

template <typename ParticleType>
bool isParticleDead(const ParticleType& particle)
{
    return particle.weight < EPSILON;
}

template <typename ParticleType>
ParticleType getClosestParticle(const std::vector<ParticleType>& particles, const XYPoint& center)
{
    auto closest = particles.at(0);
    auto minDistance = -1.0;

    for(const auto& p : particles) {
        if (isParticleDead(p)) {
            // Skipping dead particles
            continue;
        }

        auto dist = GetDist(p.point, center);

        if (minDistance < 0 || dist < minDistance) {
            closest = p;
            minDistance = dist;
        }
    }

    return closest;
}

template <typename ParticleType>
size_t calculateDeadParticles(const std::vector<ParticleType>& particles)
{
    size_t numDeadParticles = 0;
    for (const auto& p : particles) {
        if (isParticleDead(p)) {
            ++numDeadParticles;
        }
    }
    return numDeadParticles;
}

template <typename ParticleType>
size_t calculateAliveParticles(const std::vector<ParticleType>& particles)
{
    size_t numAliveParticles = 0;
    for (const auto& p : particles) {
        if (!isParticleDead(p)) {
            ++numAliveParticles;
        }
    }
    return numAliveParticles;
}

template <typename ParticleType>
void normalizeParticlesWeights(std::vector<ParticleType>& particles)
{
    double sum = 0.0;
    for(auto& p : particles) {
        if (isParticleDead(p)) {
            p.weight = 0.0;
        }
        sum += p.weight;
    }
    if (sum >= EPSILON) {
        for(auto& p : particles) {
            p.weight /= sum;
        }
    }
}

template <typename ParticleType>
void updateParticlesWeights(
    std::vector<ParticleType>& particles,
    const Likelihood* likelihood)
{
    // Switching to logarithmic scale
    for(size_t i = 0; i < particles.size(); ++i) {
        particles[i].weight = math::log(particles[i].weight);
    }

    for(size_t i = 0; i < particles.size(); ++i) {
        particles[i].weight += likelihood->getLogWeight(particles[i].point);
    }

    // Searching max particle weight
    const double maxWeight = std::max_element(particles.begin(), particles.end(),
        [](const auto& p1, const auto& p2)
        {
            return p1.weight < p2.weight;
        })->weight;

    if (std::isfinite(maxWeight)) {
        // Normalizing logarithmic weights (to avoid too small linear weights)
        // and switching back to the linear scale
        for(auto& p : particles) {
            p.weight = math::exp(p.weight - maxWeight);
        }
    } else {
        // If maxWeight equals -inf, than all weights are zero
        for(auto& p : particles) {
            p.weight = 0.0;
        }
    }

    // Normalizing particle weights
    normalizeParticlesWeights(particles);
}

// We calculate the norm as an inverse square sum of all particle weights
// and if this norm is less than 'Threshold' then we need to resample
template <typename ParticleType>
bool needResampling(const std::vector<ParticleType>& particles, double resamplingThreshold)
{
    double squareSum = 0.0;
    for(const auto& p : particles) {
        squareSum += p.weight * p.weight;
    }

    return squareSum > 1.0 / resamplingThreshold;
}

template <typename ParticleType>
void resampleParticles(std::vector<ParticleType>& particles, Rng* rng)
{
    if (particles.empty()) {
        return;
    }

    const auto size = particles.size();

    // Step 1: constructing probability distribution corresponding to particles weights
    std::vector<double> distribution(size, 0);
    distribution[0] = particles[0].weight;
    for(size_t i = 1; i < size; i++) {
        distribution[i] = distribution[i - 1] + particles[i].weight;
    }

    if (distribution[size - 1] < EPSILON) {
        // All particles are dead => do nothing
        return;
    }

    for(size_t i = 0; i < size; i++) {
        distribution[i] /= distribution[size - 1];
    }

    // Step 2: constructing random uniform sample in [0, 1]
    std::vector<double> sample(size);
    for(size_t i = 0; i < size; ++i) {
        sample[i] = rng->uniform(0, 1);
    }
    std::stable_sort(sample.begin(), sample.end());

    // Step 3: resampling particles according to the inverted distribution of particle weights
    auto newParticles = particles;
    for(size_t i = 0, j = 0; i < size; i++) {
        // Searching for index of distribution element, corresponding to this sample[i]
        while (distribution[j] < sample[i] && j + 1 < size) {
            j++;
        }
        newParticles[i] = particles[j];
        newParticles[i].weight = 1.0 / size;
    }

    particles = std::move(newParticles);
}

template <typename ParticleType>
std::vector<GeoParticle> convertParticles(
    const std::vector<ParticleType>& particles,
    const GeoPoint& bindingPoint)
{
    std::vector<GeoParticle> geoParticles;
    geoParticles.reserve(particles.size());
    for(const auto& p : particles) {
        geoParticles.push_back(GeoParticle{
            .point = LocalToGeo(p.point, bindingPoint),
            .angle = p.angle,
            .speed = p.speed,
            .weight = p.weight});
    }
    return geoParticles;
}

} // namespace navigine::navigation_core
