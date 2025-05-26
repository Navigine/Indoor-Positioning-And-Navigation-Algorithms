#pragma once

#include "likelihood.h"

namespace navigine::navigation_core {

class LikelihoodModel : public Likelihood
{
public:
    LikelihoodModel(
        const Level* level,
        const PositionEstimatorSettings::ParticleFilter& pfSettings);

    void setMeasurements(
        const RadioMeasurementsData& radioMsrs,
        const LocationMeasurementsData& locMsrs) override;

    RadioMeasurementsData getRadioMeasurements() const override;
    LocationMeasurementsData getLocationMeasurements() const override;

    double getLogWeight(const XYPoint& point) const override;

    bool hasSufficientMeasurementsForInit() const override;
    bool hasSufficientMeasurementsForUpdate() const override;
    bool hasSufficientMeasurementsForMutation() const override;

    std::vector<XYPoint> calculateSamplingCenters() const override;

private:
    const Level* level_;
    const double correctionNoiseRssi_;
    const double correctionNoiseRtt_;
    const double correctionNoiseGps_;
    const unsigned int minMsrNumForUpdate_;
    const unsigned int minMsrNumForMutation_;

    RadioMeasurementsData radioMsrs_;
    LocationMeasurementsData locMsrs_;
    bool containsRtt_;
    bool containsRssi_;
};

} // namespace navigine::navigation_core
