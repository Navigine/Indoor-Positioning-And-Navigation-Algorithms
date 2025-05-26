#pragma once

#include <navigine/navigation-core/geometry.h>
#include <navigine/navigation-core/level.h>
#include <navigine/navigation-core/measurement.h>
#include <navigine/navigation-core/navigation_settings.h>
#include <navigine/navigation-core/particle.h>

namespace navigine::navigation_core {

const constexpr double MIN_WEIGHT_SUM_DOUBLE_EPSILON = 1e-8;

class Likelihood
{
public:
    virtual ~Likelihood() = default;

    /// Perform all calculations necessary for likelihood and then use
    // this likelihood to get particle weights in method getWeight()
    virtual void setMeasurements(
        const RadioMeasurementsData& radioMsrs,
        const LocationMeasurementsData& locMsrs) = 0;

    virtual RadioMeasurementsData getRadioMeasurements() const = 0;
    virtual LocationMeasurementsData getLocationMeasurements() const = 0;

    /// Return position weight base on information calculated during Likelihood::compute() method
    virtual double getLogWeight(const XYPoint& point) const = 0;

    virtual bool hasSufficientMeasurementsForInit() const = 0;
    virtual bool hasSufficientMeasurementsForUpdate() const = 0;
    virtual bool hasSufficientMeasurementsForMutation() const = 0;

    /// Heuristic to improve particle filter robustness.
    /// The method returns points for sampling particles around them.
    /// Depending on implementation it can be transmitter positions with best signals,
    /// or reference points with best weighs
    virtual std::vector<XYPoint> calculateSamplingCenters() const = 0;
};

std::unique_ptr<Likelihood> createLikelihood(
    const Level* level,
    const PositionEstimatorSettings::ParticleFilter& pfSettings);

} // namespace navigine::navigation_core
