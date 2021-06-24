#include "likelihood_radiomap.h"
#include "../knn/knn_utils.h"

namespace navigine {
namespace navigation_core {

#ifdef NAVIGATION_VISUALIZATION
XYPoint LikelihoodRadiomap::debugTrianglePoint = XYPoint(0, 0);

std::map<ReferencePointId, double>
LikelihoodRadiomap::debugRefPointWeights = std::map<ReferencePointId, double>();
#endif

LikelihoodRadiomap::LikelihoodRadiomap(
  const std::shared_ptr<LevelCollector>& levelCollector,
  const NavigationSettings& navProps)
   : mK(navProps.commonSettings.kNeighbors)
   , mMinMsrNumForMutation(navProps.commonSettings.minMsrNumForMutation)
   , mMinMsrNumForPositionCalculation(navProps.commonSettings.minNumOfMeasurementsForPositionCalculation)
   , mUseTriangles(navProps.commonSettings.useTriangles)
   , mUseDiffMode(navProps.commonSettings.useDiffMode)
   , mLikelihoodPoint(boost::none)
{
  if (mUseTriangles)
    mLevelTriangles = triangulate(levelCollector);

  mLevelReferenceTransmittersMap = getLevelReferenceTransmittersMap(levelCollector);
  mLevelsRadiomaps = getLevelsRadiomaps(levelCollector);
}

bool LikelihoodRadiomap::hasSufficientMeasurementsForUpdate(const Level& level,
    const RadioMeasurementsData& radioMsr) const
{
  const std::set<TransmitterId>& txIds = mLevelReferenceTransmittersMap.at(level.id());
  auto validMsr = getWhitelistedMeasurements(txIds, radioMsr);
  return validMsr.size() >= mMinMsrNumForPositionCalculation;
}

bool LikelihoodRadiomap::hasSufficientMeasurementsForMutation(
  const Level& level,
  const RadioMeasurementsData& radioMsr) const
{
  const std::set<TransmitterId>& txIds = mLevelReferenceTransmittersMap.at(level.id());
  auto validMsr = getWhitelistedMeasurements(txIds, radioMsr);
  return validMsr.size() >= mMinMsrNumForMutation;
}

double calcParticleWeight(
  XYParticle particle,
  XYPoint trianglePosition)
{
  double sqrDist = (particle.x - trianglePosition.x) * (particle.x - trianglePosition.x)
                 + (particle.y - trianglePosition.y) * (particle.y - trianglePosition.y);
  return 1.0 / (0.1 + sqrt(sqrDist));
}

std::vector<double> LikelihoodRadiomap::calculateWeights(const Level& level,
    const RadioMeasurementsData& radioMsr,
    const std::vector<XYParticle>& particles)
{
  const std::set<TransmitterId>& txIds = mLevelReferenceTransmittersMap.at(level.id());
  auto validMsr = getWhitelistedMeasurements(txIds, radioMsr);
  std::vector<double> weightsMult(particles.size(), 1.0 / particles.size());

  if (validMsr.size() < mMinMsrNumForPositionCalculation)
  {
    //TODO check if error is possible and process error
    return weightsMult;
  }

  const std::multimap<TransmitterId, std::pair<ReferencePointId, SignalStatistics>>&
          radiomap = mLevelsRadiomaps.at(level.id());
  std::map<ReferencePointId, double> rpToWeight = calcRpWeightsKnn(radiomap, validMsr, mUseDiffMode);

 boost::optional<XYPoint> p = calculateRadiomapPoint(level.id(), level.radiomap(), rpToWeight);

#ifdef NAVIGATION_VISUALIZATION
  std::map<ReferencePointId, double> weights;
  for (const auto &rpPair: rpToWeight)
    weights.insert(rpPair);

  debugRefPointWeights = weights;
  if (p.is_initialized())
    debugTrianglePoint = p.get();
#endif

  double sum = 0.0;
  for (size_t i = 0; i < particles.size(); i++)
  {
    double w = p.is_initialized() ? calcParticleWeight(particles[i], p.get()) : 0.0;
    weightsMult[i] = w;
    sum += w;
  }

  if (sum > MIN_WEIGHT_SUM_DOUBLE_EPSILON)
  {
    for (size_t i = 0; i < particles.size(); i++)
    {
      weightsMult[i] /= sum;
    }
  }

  return weightsMult;
}

boost::optional<XYPoint> LikelihoodRadiomap::calculateRadiomapPoint(
  const LevelId& levelId,
  const Radiomap& radiomap,
  const std::map<ReferencePointId, double>& rpToWeight)
{
  if (!mUseTriangles)
  {
    return calcKHeaviestRefPointsAverage(radiomap, rpToWeight, mK);
  }
  else
  {
    auto trianglesIt = mLevelTriangles.find(levelId);
    if (trianglesIt == mLevelTriangles.end())
    {
      return boost::none;
    }

    const std::vector<RefPointsTriangle>& triangles = trianglesIt->second;
    if (triangles.empty())
    {
      return boost::none;
    }

    return calcPositionInTriangle(triangles, radiomap, rpToWeight);
  }
}

std::vector<XYPoint> LikelihoodRadiomap::calculateSamplingCenters(const Level& level,
    const RadioMeasurementsData& radioMsr)
{
  this->doPreliminaryCalculations(level, radioMsr);

  std::vector<XYPoint> samplingCenters;
  if (mLikelihoodPoint.is_initialized())
    samplingCenters.push_back(mLikelihoodPoint.get());

  return samplingCenters;
}

void LikelihoodRadiomap::doPreliminaryCalculations(
    const Level& level,
    const RadioMeasurementsData& radioMsr)
{
  const std::set<TransmitterId>& txIds = mLevelReferenceTransmittersMap.at(level.id());
  auto validMsr = getWhitelistedMeasurements(txIds, radioMsr);

  if (validMsr.size() >= mMinMsrNumForPositionCalculation)
  {
    const Radiomap& refPointsMap = level.radiomap();
    const std::multimap<TransmitterId,
                        std::pair<ReferencePointId, SignalStatistics>>& signalsMap =
            mLevelsRadiomaps.at(level.id());
    std::map<ReferencePointId, double> rpToWeight =
            calcRpWeightsKnn(signalsMap, validMsr, mUseDiffMode);

    mLikelihoodPoint = calculateRadiomapPoint(level.id(), refPointsMap, rpToWeight);
  }
}

double LikelihoodRadiomap::getWeight(const Level&,
    const RadioMeasurementsData&,
    double x,
    double y) const
{
  return mLikelihoodPoint.is_initialized()
    ? calcParticleWeight(XYParticle{x, y, 0}, mLikelihoodPoint.get())
    : 0.0;
}

RadioMeasurementsData LikelihoodRadiomap::getOnlyCorrectlyIntersectedMeasurements(
  const Level& level,
  const RadioMeasurementsData& radioMsr) const
{
  return radioMsr;
}

} } // namespace navigine::navigation_core
