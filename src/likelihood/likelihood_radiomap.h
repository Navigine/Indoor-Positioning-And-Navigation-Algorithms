#ifndef LIKELIHOOD_TRIANGULATION_H
#define LIKELIHOOD_TRIANGULATION_H

#include <boost/optional.hpp>
#include <navigine/navigation-core/navigation_settings.h>

#include "likelihood.h"
#include "../knn/knn_utils.h"

namespace navigine {
namespace navigation_core {

class LikelihoodRadiomap : public Likelihood
{
  public:
    LikelihoodRadiomap(
      const std::shared_ptr<LevelCollector>& levelCollector,
      const NavigationSettings& navProps);

    std::vector<double> calculateWeights(
      const Level& level,
      const RadioMeasurementsData& radioMsr,
      const std::vector<XYParticle>& particles
    ) override;

    void doPreliminaryCalculations(const Level& level,
      const RadioMeasurementsData& radioMsr
    ) override;

    double getWeight(
      const Level& level,
      const RadioMeasurementsData& radioMsr,
      double x, double y
    ) const override;

    std::vector<XYPoint> calculateSamplingCenters(
      const Level& level,
      const RadioMeasurementsData& radioMsr
    ) override;

    bool hasSufficientMeasurementsForUpdate(
      const Level& level,
      const RadioMeasurementsData& radioMsr
    ) const override;

    bool hasSufficientMeasurementsForMutation(const Level& level,
      const RadioMeasurementsData& radioMsr
    ) const override;

    MeasurementLikelihoodModel getLikelihoodModel(void) override{return mLikelihoodModel;}

    RadioMeasurementsData getOnlyCorrectlyIntersectedMeasurements(
      const Level& level,
      const RadioMeasurementsData& radioMsr
    ) const override;

  #ifdef NAVIGATION_VISUALIZATION
    static XYPoint debugTrianglePoint;
    static std::map<ReferencePointId, double> debugRefPointWeights;
  #endif

  private:
    boost::optional<XYPoint> calculateRadiomapPoint(
      const LevelId& levelId,
      const Radiomap& radiomap,
      const std::map<ReferencePointId, double>& rpToWeight);

    const MeasurementLikelihoodModel mLikelihoodModel = MeasurementLikelihoodModel::RADIOMAP;
    const int mK;
    const size_t mMinMsrNumForMutation;
    const size_t mMinMsrNumForPositionCalculation;
    const bool mUseTriangles;
    const bool mUseDiffMode;

    //TODO take into account probability of such point!
    boost::optional<XYPoint> mLikelihoodPoint; // point for particle mutation and particle sampling
    std::map<LevelId, std::vector<RefPointsTriangle>> mLevelTriangles;
    std::map<LevelId, std::set<TransmitterId>> mLevelReferenceTransmittersMap;
    std::map<LevelId,
            std::multimap<TransmitterId,
                          std::pair<ReferencePointId, SignalStatistics>>>
        mLevelsRadiomaps;
};

} } // namespace navigine::navigation_core

#endif // LIKELIHOOD_TRIANGULATION_H
