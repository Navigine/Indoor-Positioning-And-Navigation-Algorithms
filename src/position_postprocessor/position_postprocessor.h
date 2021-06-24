#ifndef POSITION_POSTPROCESSOR_H
#define POSITION_POSTPROCESSOR_H

#include <navigine/navigation-core/motion_info.h>
#include <navigine/navigation-core/navigation_settings.h>
#include <navigine/navigation-core/level.h>

#include "../position.h"
#include "navigation_time_smoother.h"
#include "position_smoother.h"

namespace navigine {
namespace navigation_core {

class PositionPostprocessor
{
  public:
    ~PositionPostprocessor();
    PositionPostprocessor(const NavigationSettings& navProps);

    Position getProcessedPosition(const Position& fusedPos,
      long long ts, const MotionInfo& motionInfo,
      const Level& level);

    Position fusePositions(long long curTs, const Position& indoorPos, const Position& outdoorPos, const NavigationStatus& navStatus);
    void fillIndoorOutdoorPositionStates(const Position& indoorPos, const Position& outdoorPos);
    const Position getIndoorPosition() { return mIndoorPos; }
    const Position getOutdoorPosition() { return mOutdoorPos; }

  private:
    bool isInsideMap(const Position& pos, const Level& level);

    const bool mUseSmoothing;
    const bool mUseStops;
    const bool mUseInstantGpsPosition;
    const bool mUseGps;
    const bool mFuseGps;
    const bool mUseGpsOutsideMap;
    const bool mPreferIndoorSolution;
    const bool mUseTimeSmoothing;
    const bool mUseGraphProjection;

    const long long mDeadReckonTimeMs;
    const long long mStopUpdateTimeMs;
    const long long mStopDetectionTimeMs;

    const double mGraphProjectionDistance;
    const double mPriorDev;
    const double mFuseGpsBorderM;
    const double mMotionSpeed;
    const double mPosIsTooOldForFusingSec;
    const double mStopsDistThresholdM;

    double mIndoorDev;
    double mOutdoorDev;

    NavigationTimeSmoother mNavigationTimeSmoother;
    PositionSmoother* mPositionSmoother;

    Position mIndoorPos;
    Position mOutdoorPos;
    Position mLastOutPosition;

    long long mLastExtractionTs = -1;
    long long mLastStepTs = -1;
};

} } // namespace navigine::navigation_core

#endif // POSITION_POSTPROCESSOR_H
