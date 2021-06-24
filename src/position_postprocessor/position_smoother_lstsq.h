#ifndef NAVIGINE_POSITION_SMOOTHER_LSTSQ_H
#define NAVIGINE_POSITION_SMOOTHER_LSTSQ_H

#include "position_smoother.h"
#include "polynomial_fit.h"

namespace navigine {
namespace navigation_core {

class PositionSmootherLstsq: public PositionSmoother
{
  public:
    PositionSmootherLstsq(const NavigationSettings& navProps);
    Position smoothPosition(Position pos, const Level& level) override;
    void reset(Position pos) override;

  private:
    const double mTimeInterval;
    const double mWindowShift;
    const double mSmoothing;
    bool mWasCalled;
    bool mUseBarriers;
    PolynomialFit mPolyFitX;
    PolynomialFit mPolyFitY;
    long long mStartTime;
};

} } // namespace navigine::navigation_core

#endif // NAVIGINE_POSITION_SMOOTHER_LSTSQ_H
