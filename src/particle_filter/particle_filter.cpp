#include <algorithm>
#include <numeric>
#include <cmath>

#include <navigine/navigation-core/math/math.h>

#include "likelihood_model.h"
#include "particle_filter.h"
#include "sampler.h"

namespace navigine::navigation_core {

ParticleFilter::ParticleFilter(
    const std::shared_ptr<const Level>& level,
    const std::shared_ptr<Rng>& rng,
    const PositionEstimatorSettings::ParticleFilter& pfSettings)
    : useHeadingCorrection_(pfSettings.useHeadingCorrection.value_or(PositionEstimatorSettings::ParticleFilter::DEFAULT_USE_HEADING_CORRECTION))
    , numParticles_(pfSettings.numParticles.value_or(PositionEstimatorSettings::ParticleFilter::DEFAULT_NUM_PARTICLES))
    , numParticlesToMutate_(pfSettings.numParticlesToMutateRate.value_or(PositionEstimatorSettings::ParticleFilter::DEFAULT_NUM_PARTICLES_TO_MUTATE_RATE) * numParticles_)
    , numParticlesAroundClosestTxs_(pfSettings.numParticlesAroundClosestTxsRate.value_or(PositionEstimatorSettings::ParticleFilter::DEFAULT_NUM_PARTICLES_AROUND_CLOSEST_TXS_RATE) * numParticles_)
    , numParticlesAliveThreshold_(pfSettings.numParticlesAliveThresholdRate.value_or(PositionEstimatorSettings::ParticleFilter::DEFAULT_NUM_PARTICLES_ALIVE_THRESHOLD_RATE) * numParticles_)
    , resamplingThreshold_(pfSettings.resamplingThresholdRate.value_or(PositionEstimatorSettings::ParticleFilter::DEFAULT_RESAMPLING_THRESHOLD_RATE) * numParticles_)
    , wallPassingProbability_(pfSettings.wallPassingProbability.value_or(PositionEstimatorSettings::ParticleFilter::DEFAULT_WALL_PASSING_PROBABILITY))
    , minDeviation_(pfSettings.minDeviation.value_or(PositionEstimatorSettings::ParticleFilter::DEFAULT_MIN_DEVIATION))
    , level_(level)
    , rng_(rng)
    , likelihood_(createLikelihood(level_.get(), pfSettings))
    , sampler_(std::make_unique<Sampler>(level_.get(), likelihood_.get(), rng_.get()))
    , particles_(numParticles_)
    , initialized_(false)
{}

RadioMeasurementsData ParticleFilter::getRadioMeasurements() const
{
    return likelihood_->getRadioMeasurements();
}

LocationMeasurementsData ParticleFilter::getLocationMeasurements() const
{
    return likelihood_->getLocationMeasurements();
}

std::vector<GeoParticle> ParticleFilter::getParticles() const
{
    if (!initialized_) {
        return {};
    }

    return convertParticles(particles_, level_->bindingPoint());
}

std::optional<SolutionPF> ParticleFilter::getSolution() const
{
    if (!initialized_) {
        return std::nullopt;
    }

    return getSolutionFromParticles(particles_, minDeviation_);
}

bool ParticleFilter::performEstimation(
    const RadioMeasurementsData& radioMsrs,
    const LocationMeasurementsData& locMsrs,
    const MotionInfo& motionInfo)
{
    likelihood_->setMeasurements(radioMsrs, locMsrs);

    if (useHeadingCorrection_ && motionInfo.heading && motionInfo.headingDeviation && !hasGoodHeading()) {
        sampler_->setAngleRange(
            *motionInfo.heading - *motionInfo.headingDeviation,
            *motionInfo.heading + *motionInfo.headingDeviation);
    } else {
        sampler_->setAngleRange(-M_PI, M_PI);
    }

    if (motionInfo.speed && motionInfo.speedDeviation) {
        sampler_->setSpeedRange(
            *motionInfo.speed - *motionInfo.speedDeviation,
            *motionInfo.speed + *motionInfo.speedDeviation);
    } else {
        sampler_->setSpeedRange(std::nullopt, std::nullopt);
    }

    if (initialized_) {
        move(motionInfo);
    }

    if (!initialized_ || calculateAliveParticles(particles_) < numParticlesAliveThreshold_) {
        if (likelihood_->hasSufficientMeasurementsForInit()) {
            init();
            return true;
        }
        return false;
    }

    bool updated = false;

    if (likelihood_->hasSufficientMeasurementsForUpdate()) {
        if (likelihood_->hasSufficientMeasurementsForMutation()) {
            mutate();
        }

        update();
        updated = true;
    }

    resample();
    return updated;
}

void ParticleFilter::reset()
{
    if (!initialized_) {
        return;
    }

    initialized_ = false;

    for(auto& p : particles_) {
        p.weight = 0.0;
    }
}

void ParticleFilter::init()
{
    if (sampler_->sampleParticles(particles_)) {
        updateParticlesWeights(particles_, likelihood_.get());
        initialized_ = true;
    }
}

void ParticleFilter::update()
{
    if (!initialized_) {
        return;
    }

    // TODO: write custom divergency detector
    updateParticlesWeights(particles_, likelihood_.get());
}

void ParticleFilter::mutate()
{
    if (!initialized_) {
        return;
    }

    sampler_->mutateParticles(particles_, numParticlesToMutate_);
    sampler_->sampleParticlesAroundCenters(particles_, numParticlesAroundClosestTxs_);
}

void ParticleFilter::resample()
{
    if (!initialized_) {
        return;
    }

    if (needResampling(particles_, resamplingThreshold_)) {
        resampleParticles(particles_, rng_.get());
    }
}

void ParticleFilter::move(const MotionInfo& motionInfo)
{
    if (!initialized_) {
        return;
    }

    for(auto& p : particles_) {
        if (isParticleDead(p)) {
            continue;
        }

        const auto prevPoint = p.point;

        // Rotating particle
        if (std::fabs(motionInfo.deltaHeading) + std::fabs(motionInfo.deltaHeadingDeviation) > EPSILON) {
            const auto deltaAngle = rng_->uniform(
                motionInfo.deltaHeading - motionInfo.deltaHeadingDeviation,
                motionInfo.deltaHeading + motionInfo.deltaHeadingDeviation);
            p.angle = NormalizedAngle(p.angle + deltaAngle);
        }

        // Updating particle speed
        if (p.speed) {
            if (motionInfo.hasOdometer && motionInfo.speed && motionInfo.speedDeviation) {
                // Correct particle speed using odometer
                p.speed = std::max(rng_->normal(*motionInfo.speed, *motionInfo.speedDeviation), 0.0);
            } else if (motionInfo.deltaSpeed && motionInfo.deltaSpeedDeviation) {
                // Correct particle speed using stohastic speed delta
                const auto deltaSpeed = rng_->normal(*motionInfo.deltaSpeed, *motionInfo.deltaSpeedDeviation);
                p.speed = std::max(*p.speed + deltaSpeed, 0.0);
            }
        }

        // Moving particle (discrete part)
        if (std::fabs(motionInfo.distance) + std::fabs(motionInfo.distanceDeviation) > EPSILON) {
            const auto distance = std::max(rng_->normal(motionInfo.distance, motionInfo.distanceDeviation), 0.0);
            p.point.x += distance * math::sin(p.angle);
            p.point.y += distance * math::cos(p.angle);
        }

        // Moving particle (continuous part)
        if (p.speed && std::fabs(*p.speed) > EPSILON) {
            p.point.x += *p.speed * motionInfo.deltaSec * math::sin(p.angle);
            p.point.y += *p.speed * motionInfo.deltaSec * math::cos(p.angle);
        }

        if (!level_->containsSegment(XYSegment{prevPoint, p.point})) {
            if (!rng_->binary(wallPassingProbability_)) {
                p.weight = 0.0;
            }
        }
    }

    normalizeParticlesWeights(particles_);
}

bool ParticleFilter::hasGoodHeading() const
{
    const auto solution = getSolution();

    return solution &&
           solution->heading &&
           solution->headingAccuracy &&
           solution->headingAccuracy.value() < M_PI / 2;
}

} // namespace navigine::navigation_core
