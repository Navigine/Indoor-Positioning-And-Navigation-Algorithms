#include <navigine/navigation-core/navigation_settings.h>
#include "../geometry.h"
#include "position_postprocessor.h"
#include "position_smoother_ab.h"
#include "position_smoother_lstsq.h"

namespace navigine {
namespace navigation_core {

PositionPostprocessor::PositionPostprocessor(const NavigationSettings& navProps)
  : mUseSmoothing(navProps.commonSettings.useSmoothing)
  , mUseStops(navProps.commonSettings.useStops)
  , mUseInstantGpsPosition(navProps.commonSettings.useInstantGpsPosition)
  , mUseGps(navProps.commonSettings.useGps)
  , mFuseGps(navProps.commonSettings.fuseGps)
  , mUseGpsOutsideMap(navProps.commonSettings.useGpsOutsideMap)
  , mPreferIndoorSolution(navProps.commonSettings.preferIndoorSolution)
  , mUseTimeSmoothing(navProps.commonSettings.useTimeSmoothing)
  , mUseGraphProjection(navProps.commonSettings.useGraphProjection)
  , mDeadReckonTimeMs((long long)(1000 * navProps.commonSettings.deadReckoningTime))
  , mStopUpdateTimeMs((long long)(1000 * navProps.commonSettings.stopUpdateTime))
  , mStopDetectionTimeMs((long long)(1000 * navProps.commonSettings.stopDetectionTime))
  , mGraphProjectionDistance(navProps.commonSettings.graphProjDist)
  , mPriorDev(navProps.commonSettings.priorDeviation)
  , mFuseGpsBorderM(navProps.commonSettings.fuseGpsBorderM)
  , mMotionSpeed(navProps.commonSettings.averageMovSpeed)
  , mPosIsTooOldForFusingSec(navProps.commonSettings.positionIsTooOldSec)
  , mStopsDistThresholdM(navProps.commonSettings.useStopsDistanceThresholdM)
  , mIndoorDev(navProps.commonSettings.priorDeviation)
  , mOutdoorDev(navProps.commonSettings.priorDeviation)
  , mNavigationTimeSmoother()
  , mIndoorPos()
  , mOutdoorPos()
  , mLastOutPosition()
{
  mLastExtractionTs = 0;

  bool useAbFilter = navProps.commonSettings.useAbFilter;

  if (useAbFilter)
    mPositionSmoother = new PositionSmootherAB(navProps);
  else
    mPositionSmoother = new PositionSmootherLstsq(navProps);
}

PositionPostprocessor::~PositionPostprocessor()
{
  if (mPositionSmoother)
    delete mPositionSmoother;
}

void PositionPostprocessor::fillIndoorOutdoorPositionStates(
  const Position& indoorPos, 
  const Position& outdoorPos)
{
  if (!indoorPos.isEmpty)
  {
    double priorIndoorDev = mIndoorPos.isEmpty ? mPriorDev : std::min(mPriorDev, mIndoorDev + mMotionSpeed * (indoorPos.ts - mIndoorPos.ts) / 1000.0);
    mIndoorPos = indoorPos;
    mIndoorDev = (priorIndoorDev + indoorPos.deviationM) / 2.0;
  }

  if (!outdoorPos.isEmpty)
  {
    double priorOutdoorDev = mOutdoorPos.isEmpty ? mPriorDev : std::min(mPriorDev, mOutdoorDev + mMotionSpeed * (outdoorPos.ts - mOutdoorPos.ts) / 1000.0);
    mOutdoorDev = (priorOutdoorDev + outdoorPos.deviationM) / 2.0;
    mOutdoorPos = outdoorPos;
  }
}

Position PositionPostprocessor::fusePositions(
  long long curTs, 
  const Position& indoorPos, 
  const Position& outdoorPos,
  const NavigationStatus& navStatus)
{
  fillIndoorOutdoorPositionStates(indoorPos, outdoorPos);

  // return instant gps position for debugging:
  if (mUseInstantGpsPosition && !outdoorPos.isEmpty)
    return outdoorPos;

  if (mPreferIndoorSolution && !indoorPos.isEmpty and navStatus==0)
    return indoorPos;

  if (!mUseGps || mOutdoorPos.isEmpty)
    return mIndoorPos;

  if (mIndoorPos.isEmpty)
    return mOutdoorPos;

  double indoorPositionDelay  = curTs >= mIndoorPos.ts ? (curTs - mIndoorPos.ts) / 1000.0 : 0.0;
  double outdoorPositionDelay = curTs >= mOutdoorPos.ts ? (curTs - mOutdoorPos.ts) / 1000.0 : 0.0;

  if ((indoorPositionDelay) > mPosIsTooOldForFusingSec)
    return mOutdoorPos;

  if ((outdoorPositionDelay) > mPosIsTooOldForFusingSec)
    return mIndoorPos;

  double indoorDev = mMotionSpeed * indoorPositionDelay + mIndoorDev;
  double outdoorDev = mMotionSpeed * outdoorPositionDelay + mOutdoorDev;

  double positionsDist = GetDist(mIndoorPos.x, mIndoorPos.y, mOutdoorPos.x, mOutdoorPos.y);
  double accuracyDist = mIndoorPos.deviationM + mOutdoorPos.deviationM + mFuseGpsBorderM;

  if (mFuseGps && (accuracyDist > positionsDist))
  {
    Position fusedPos = mIndoorPos;
    fusedPos.ts = std::max(mIndoorPos.ts, mOutdoorPos.ts);
    fusedPos.x = (mIndoorPos.x * outdoorDev + mOutdoorPos.x * indoorDev) / (outdoorDev + indoorDev);
    fusedPos.y = (mIndoorPos.y * outdoorDev + mOutdoorPos.y * indoorDev) / (outdoorDev + indoorDev);
    fusedPos.deviationM = std::min(indoorDev, outdoorDev);
    fusedPos.provider = Provider::FUSED;
    return fusedPos;
  }
  else
    return indoorDev < outdoorDev ? mIndoorPos : mOutdoorPos;
}

LocationPoint getProjection(const Graph<XYPoint>& graph, const LocationPoint& P)
{
  LocationPoint P0 = P;
  double d0 = NAN;
  std::pair<int, int> e0;
  
  for (auto edgePair = graph.edgesBegin(); edgePair != graph.edgesEnd(); ++edgePair)
  {
    const Graph<XYPoint>::Vertex& u = graph.getVertex((*edgePair).first);
    const Graph<XYPoint>::Vertex& v = graph.getVertex((*edgePair).second);

    LocationPoint P1 = P;
    double k = GetProjection(u.point.x, u.point.y, v.point.x, v.point.y, P.x, P.y, &P1.x, &P1.y);

    if (k < 0)
      P1 = LocationPoint{P.level, u.point.x, u.point.y};
    else if (k > 1)
      P1 = LocationPoint{P.level, v.point.x, v.point.y};

    double d = GetDist(P, P1);
    if (std::isnan(d0) || d < d0)
    {
      P0 = P1;
      d0 = d;
      e0 = *edgePair;
    }
  }

  return P0;
}

bool PositionPostprocessor::isInsideMap(const Position& pos, const Level& level)
{
  if (mUseGpsOutsideMap)
    return true;
  return boost::geometry::covered_by(static_cast<XYPoint>(pos), level.geometry().boundingBox());
}

Position PositionPostprocessor::getProcessedPosition(const Position& fusedPosition,
  long long ts,
  const MotionInfo& motionInfo,
  const Level& level)
{
  if (motionInfo.lastMotionTs > 0)
    mLastStepTs = motionInfo.lastMotionTs;

  if (fusedPosition.isEmpty)
    return Position(); //TODO error code "fused position is empty!"

  if (!isInsideMap(fusedPosition, level))
    return Position(); // Position is outside of map!

  if (ts - fusedPosition.ts >= mDeadReckonTimeMs)
    return Position(); //TODO error code "last received position is too old!"

  Position result = fusedPosition;

  if (mUseSmoothing)
  {
    if (mLastOutPosition.levelId != result.levelId)
      mPositionSmoother->reset(result);
    result = mPositionSmoother->smoothPosition(result, level);
  }

  if (mUseTimeSmoothing)
    result = mNavigationTimeSmoother.getTimeSmoothedPosition(result, ts);

  //TODO check if pedometer is available before using steps!
  //TODO be very carefull when use stops and gps simultaneously
  bool lastStepWasTooLongAgo = (mLastStepTs > 0) && (ts - mLastStepTs >= mStopDetectionTimeMs);
  double dist = GetDist(mLastOutPosition.x, mLastOutPosition.y, result.x, result.y);
  bool newPositionIsNotTooFar = dist < mStopsDistThresholdM;

  mLastOutPosition.ts = ts;
  if (mUseStops && lastStepWasTooLongAgo)
  {
    if ((ts - mLastExtractionTs > mStopUpdateTimeMs) && newPositionIsNotTooFar)
    {
      mLastExtractionTs = ts;
      mLastOutPosition = result;
      mLastOutPosition.ts = ts;
    }
    else
    {
      mLastOutPosition.ts = ts;
    }
  }
  else
  {
    mLastOutPosition = result;
    mLastOutPosition.ts = ts;
    mLastOutPosition.deviationM = mIndoorPos.deviationM;
  }


  // Projecting coordinates to graph
  if (mUseGraphProjection)
  {
    const LocationPoint P1 = {mLastOutPosition.levelId, mLastOutPosition.x, mLastOutPosition.y};
    const LocationPoint P2 = getProjection(level.graph(), P1);
    const double distance = GetDist(P1, P2);

    if (!std::isnan(distance) && distance < mGraphProjectionDistance)
    {
      mLastOutPosition.x = P2.x;
      mLastOutPosition.y = P2.y;
    }
  }

  // Fitting coordinates inside box
  Point minCorner = level.geometry().boundingBox().min_corner();
  Point maxCorner = level.geometry().boundingBox().max_corner();
  mLastOutPosition.x = std::min(std::max(mLastOutPosition.x, minCorner.x()), maxCorner.x());
  mLastOutPosition.y = std::min(std::max(mLastOutPosition.y, minCorner.y()), maxCorner.y());

  // Rounding coordinates to 0.01
  mLastOutPosition.x = std::floor(mLastOutPosition.x * 100) / 100;
  mLastOutPosition.y = std::floor(mLastOutPosition.y * 100) / 100;

  return mLastOutPosition;
}

} } // namespace navigine::navigation_core
