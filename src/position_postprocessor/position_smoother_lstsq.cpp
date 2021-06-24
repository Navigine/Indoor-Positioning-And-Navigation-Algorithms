#include <navigine/navigation-core/navigation_settings.h>
#include "position_smoother_lstsq.h"

namespace navigine {
namespace navigation_core {

PositionSmootherLstsq::PositionSmootherLstsq(const NavigationSettings& navProps)
    : mWasCalled(false)
    , mUseBarriers(navProps.commonSettings.useBarriers)
    , mTimeInterval(navProps.commonSettings.sigAveragingTime)
    , mWindowShift(navProps.commonSettings.sigWindowShift)
    , mSmoothing(std::min(1.0, std::max(0.0, navProps.commonSettings.smoothing)))
    , mPolyFitX(1, mTimeInterval, mWindowShift, mSmoothing)
    , mPolyFitY(1, mTimeInterval, mWindowShift, mSmoothing)
{
}

Position PositionSmootherLstsq::smoothPosition(Position pos, const Level& level)
{
  if (!mWasCalled)
  {
    mStartTime = pos.ts;
    mWasCalled = true;
    return pos;
  }

  double t = (pos.ts - mStartTime) / 1000.0;
  mPolyFitX.addSequencePoint(t, pos.x);
  mPolyFitY.addSequencePoint(t, pos.y);

  double xs = mPolyFitX.predict(t);
  double ys = mPolyFitY.predict(t);

  if (mUseBarriers && !boost::geometry::covered_by(Point(xs, ys), level.geometry().allowedArea()))
  {
    mPolyFitX.clear();
    mPolyFitY.clear();
    mStartTime = pos.ts;
    return pos;
  }

  if (std::isfinite(xs) && std::isfinite(ys))
  {
    pos.x = xs;
    pos.y = ys;
  }

  return pos;
}

void PositionSmootherLstsq::reset(Position pos)
{
  mPolyFitX.clear();
  mPolyFitY.clear();
  mStartTime = pos.ts;
}

} } // namespace navigine::navigation_core
