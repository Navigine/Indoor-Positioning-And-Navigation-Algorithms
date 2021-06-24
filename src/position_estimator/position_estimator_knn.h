/** position_estimator_knn.h
 *
 * Copyright (c) 2018 Navigine.
 *
 */

#ifndef POSITION_ESTIMATOR_KNN_H
#define POSITION_ESTIMATOR_KNN_H

#include <navigine/navigation-core/navigation_output.h>

#include "position_estimator.h"
#include "../knn/knn_utils.h"

namespace navigine {
namespace navigation_core {

class PositionEstimatorKnn: public PositionEstimator
{
public:
  PositionEstimatorKnn(
    const std::shared_ptr<LevelCollector>& levelCollector,
    const NavigationSettings& navProps);

    Position calculatePosition(
      const Level& level,
      long long ts,
      const RadioMeasurementsData& radioMsr,
      const MotionInfo& motionInfo,
      NavigationStatus& retStatus) override;

#ifdef NAVIGATION_VISUALIZATION
  static std::map<ReferencePointId, double> debugRefPointWeights;
#endif

private:
    const bool mUseTriangles;
    const bool mUseDiffMode;
    const size_t mK;
    const size_t mMinMsrNumForPositionCalculation;
    
    const std::map<LevelId, std::vector<RefPointsTriangle>> mLevelTriangles;
    const std::map<LevelId, std::set<TransmitterId>> mLevelReferenceTransmittersMap;
    const std::map<LevelId,
             std::multimap<TransmitterId,
                           std::pair<ReferencePointId, SignalStatistics>>> mLevelsRadiomaps;
};

} } // namespace navigine::navigation_core

#endif // POSITION_ESTIMATOR_KNN_H
