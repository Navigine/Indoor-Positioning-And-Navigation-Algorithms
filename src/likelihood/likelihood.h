#ifndef LIKELIHOOD_H
#define LIKELIHOOD_H

#include <navigine/navigation-core/xy_particle.h>
#include <navigine/navigation-core/navigation_input.h>
#include <navigine/navigation-core/navigation_settings.h>
#include <navigine/navigation-core/level_collector.h>

#include "../position.h"

namespace navigine {
namespace navigation_core {

const constexpr double MIN_WEIGHT_SUM_DOUBLE_EPSILON = 1e-8;

enum class MeasurementLikelihoodModel {DISTANCE_V1, DISTANCE_V2, RSSI, RADIOMAP};


class Likelihood
{
    public:
        virtual ~Likelihood() {}
        virtual std::vector<double> calculateWeights(
            const Level& levelCollector,
            const RadioMeasurementsData& msr, 
            const std::vector<XYParticle>& particles) = 0;

        /**
         * optimization for mutation and sampling - first we perform all calculations necessary for likelihood
         * and then use this likelihood to get particle weights in method getWeight()
         */
        virtual void doPreliminaryCalculations(
            const Level& level,
            const RadioMeasurementsData& radioMsr) = 0;

        /**
         * return position weight base on information calculated during Likelihood::compute() method
         */
        virtual double getWeight(
            const Level& level,
            const RadioMeasurementsData& radioMsr,
            double x, double y) const = 0;

        virtual bool hasSufficientMeasurementsForUpdate(
            const Level& level,
            const RadioMeasurementsData& radioMsr) const = 0;

        virtual bool hasSufficientMeasurementsForMutation(
            const Level& level,
            const RadioMeasurementsData& radioMsr) const = 0;

        /**
         * Heuristic to improve particle filter robustness.
         * The method returns points for sampling particles around them.
         * Depending on implementation it can be transmitter positions with best signals,
         * or reference points with best weighs
         */
        virtual std::vector<XYPoint> calculateSamplingCenters(
            const Level& level,
            const RadioMeasurementsData& radioMsr) = 0;

        virtual MeasurementLikelihoodModel getLikelihoodModel(void) = 0;

        virtual RadioMeasurementsData getOnlyCorrectlyIntersectedMeasurements(
            const Level& level,
            const RadioMeasurementsData& radioMsr) const = 0;
};

bool isParticleInIntersectionArea(
  const Level& level,
  const RadioMeasurementsData& radioMsr,
  double x, double y);

std::shared_ptr<Likelihood> createLikelihoodLogModel(const std::shared_ptr<LevelCollector>& levelCollector, const NavigationSettings& navProps, MeasurementLikelihoodModel likelihoodModel);

} } // namespace navigine::navigation_core

#endif // LIKELIHOOD_H