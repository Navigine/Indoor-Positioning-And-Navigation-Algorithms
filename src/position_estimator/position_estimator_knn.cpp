/** position_estimator_knn.cpp
 *
 * Copyright (c) 2018 Navigine.
 *
 */

#include <algorithm>
#include <numeric>
#include <cmath>

#include <navigine/navigation-core/navigation_settings.h>
#include "position_estimator_knn.h"

namespace navigine {
namespace navigation_core {

#ifdef NAVIGATION_VISUALIZATION
   std::map<ReferencePointId, double>
   PositionEstimatorKnn::debugRefPointWeights = std::map<ReferencePointId, double>();
#endif

PositionEstimatorKnn::PositionEstimatorKnn(
  const std::shared_ptr<LevelCollector>& levelCollector,
  const NavigationSettings& navProps)
    : PositionEstimator(levelCollector)
    , mUseTriangles(navProps.commonSettings.useTriangles)
    , mUseDiffMode(navProps.commonSettings.useDiffMode)
    , mK(navProps.commonSettings.kNeighbors)
    , mMinMsrNumForPositionCalculation(navProps.commonSettings.minNumOfMeasurementsForPositionCalculation)
    , mLevelTriangles(triangulate(levelCollector))
    , mLevelReferenceTransmittersMap(getLevelReferenceTransmittersMap(levelCollector))
    , mLevelsRadiomaps(getLevelsRadiomaps(levelCollector))
{
}

Position PositionEstimatorKnn::calculatePosition(
  const Level& level,
  long long ts,
  const RadioMeasurementsData& radioMsr,
  const MotionInfo&,
  NavigationStatus &retStatus)
{
  if (radioMsr.empty())
  {
    retStatus = NavigationStatus::NO_SOLUTION;
    return mPosition;
  }

  retStatus = NavigationStatus::OK;

  const std::set<TransmitterId>& beaconsIds = mLevelReferenceTransmittersMap.at(level.id());
  auto validMsr = getWhitelistedMeasurements(beaconsIds, radioMsr);
  if (validMsr.size() < mMinMsrNumForPositionCalculation)
  {
    retStatus = NavigationStatus::NO_SOLUTION;
    return mPosition;
  }

  const Radiomap& refPointsMap = level.radiomap();
  const std::multimap<TransmitterId,
          std::pair<ReferencePointId, SignalStatistics>>& signalMap = mLevelsRadiomaps.at(level.id());
  std::map<ReferencePointId, double> rpToWeight = calcRpWeightsKnn(signalMap, validMsr, mUseDiffMode);
#ifdef NAVIGATION_VISUALIZATION
  std::map<ReferencePointId, double> weights;
  for (std::map<ReferencePointId, double>::iterator it = rpToWeight.begin();
       it != rpToWeight.end(); it++)
    weights.insert(*it);

  debugRefPointWeights = weights;
#endif

  XYPoint p = XYPoint(0.0, 0.0);
  if (mUseTriangles)
  {
    if (mLevelTriangles.find(level.id()) == mLevelTriangles.end())
    {
      retStatus = NavigationStatus::NAVIGATION_ERROR;
      return mPosition;
    }

    const std::vector<RefPointsTriangle>& triangles = mLevelTriangles.find(level.id())->second;

    if (triangles.empty())
    {
      retStatus = NavigationStatus::NO_RPS;
      return mPosition;
    }
    p = calcPositionInTriangle(triangles, refPointsMap, rpToWeight);
  }
  else
  {
    p = calcKHeaviestRefPointsAverage(refPointsMap, rpToWeight, mK);
  }

  bool isEmpty = false;
  double precision = 1.0; //TO FIX: calculate position accuracy
  mPosition = Position(p.x, p.y, level.id(), precision, ts, isEmpty, Provider::INDOOR, 0.0);
  
  return mPosition;
}

} } // namespace navigine::navigation_core
