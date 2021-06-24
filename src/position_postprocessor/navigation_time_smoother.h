/** navigation_time_smoother.h
 *
 * Copyright (c) 2018 Navigine.
 *
 */

#ifndef NAVIGINE_NAVIGATION_TIME_SMOOTHER_H
#define NAVIGINE_NAVIGATION_TIME_SMOOTHER_H

#include "../position.h"

namespace navigine {
namespace navigation_core {

class NavigationTimeSmoother
{
  public:
    NavigationTimeSmoother();
    Position getTimeSmoothedPosition(const Position& lastKnownPosition, long long currentTs);

  private:
    Position mPrevPosition;
    Position mNextPosition;
};

} } // namespace navigine::navigation_core

#endif // NAVIGINE_NAVIGATION_TIME_SMOOTHER_H
