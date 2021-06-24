#ifndef NAVIGINE_POSITION_SMOOTHER_AB_H
#define NAVIGINE_POSITION_SMOOTHER_AB_H

#include "position_smoother.h"

namespace navigine {
namespace navigation_core {

class PositionSmootherAB: public PositionSmoother
{
  public:
    PositionSmootherAB(const NavigationSettings& navProps);
    Position smoothPosition(Position pos, const Level& level) override;
    void reset(Position pos) override;

  private:
    const bool mUseBarriers;
    const bool mUseSpeedConstraint;
    const bool mUseAccuracyForSmoothing;
    bool mWasCalled = false;
    double mSpeedX = 0.0;
    double mSpeedY = 0.0;
    double mAlpha = 0.0;

    Position mPosition;
    long long mTsMs;
};

} } // namespace navigine::navigation_core

#endif // NAVIGINE_POSITION_SMOOTHER_AB_H
