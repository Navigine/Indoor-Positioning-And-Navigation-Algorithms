/** navigation_time_smoother.cpp
 *
 * Copyright (c) 2018 Navigine.
 *
 */

#include "navigation_time_smoother.h"

namespace navigine {
namespace navigation_core {

NavigationTimeSmoother::NavigationTimeSmoother()
    : mPrevPosition() // init empty position
    , mNextPosition() // init empty position
{
}

/**
 * Returns position smoothed in time (due to this navigation is updated every time wsMessage is received).
 * Keeps last two positions and calculates intermidiate position between them based on how much time passed since
 * last known position was calculated
 */
Position NavigationTimeSmoother::getTimeSmoothedPosition(const Position& lastKnownPosition, long long currentTs)
{
  if (currentTs < lastKnownPosition.ts)
    return lastKnownPosition;

  if (mPrevPosition.levelId != lastKnownPosition.levelId || mNextPosition.levelId != lastKnownPosition.levelId)
  {
    mPrevPosition = lastKnownPosition;
    mNextPosition = lastKnownPosition;
    return lastKnownPosition;
  }

  if (mPrevPosition.isEmpty || mNextPosition.isEmpty || lastKnownPosition.isEmpty)
  {
    mPrevPosition = mNextPosition;
    mNextPosition = lastKnownPosition;
    return lastKnownPosition;
  }

  if (lastKnownPosition.ts > mNextPosition.ts)
  {
    mPrevPosition = mNextPosition;
    mNextPosition = lastKnownPosition;
  }

  if (mNextPosition.ts == mPrevPosition.ts)
    return mNextPosition;

  double actualTsDiff = double(currentTs - mNextPosition.ts);
  double keptTsDiff = double(mNextPosition.ts - mPrevPosition.ts);

  if (actualTsDiff > keptTsDiff) // we don't use extrapolation
    return lastKnownPosition;

  double timeSmoothedX = (mNextPosition.x * actualTsDiff + mPrevPosition.x * (keptTsDiff - actualTsDiff)) / keptTsDiff;
  double timeSmoothedY = (mNextPosition.y * actualTsDiff + mPrevPosition.y * (keptTsDiff - actualTsDiff)) / keptTsDiff;

  Position timeSmoothedPos = mNextPosition;
  timeSmoothedPos.x = timeSmoothedX;
  timeSmoothedPos.y = timeSmoothedY;
  timeSmoothedPos.ts = currentTs;

  return timeSmoothedPos;
}

} } // namespace navigine
