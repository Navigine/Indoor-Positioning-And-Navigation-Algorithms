#include "likelihood.h"
#include "likelihood_model.h"

namespace navigine::navigation_core {

std::unique_ptr<Likelihood> createLikelihood(
    const Level* level,
    const PositionEstimatorSettings::ParticleFilter& pfSettings)
{
    return std::make_unique<LikelihoodModel>(level, pfSettings);
}

} // namespace navigine::navigation_core
