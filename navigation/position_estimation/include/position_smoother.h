#pragma once

#include "navigation_structures.h"

namespace navigine {
namespace navigation_core {

class PositionSmoother
{
  public:
    PositionSmoother(double smoothing = 0.5);
    Position smoothPosition(Position pos);

  private:
    bool            mWasCalled = false;
    double          mSpeedX    = 0.0;
    double          mSpeedY    = 0.0;
    double          mAlpha     = 0.0;

    Position        mPosition;
    long long       mTsMs;
};

} } // namespace navigine::navigation_core
