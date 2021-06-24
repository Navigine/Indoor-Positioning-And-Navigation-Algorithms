#include <navigine/navigation-core/navigation_settings.h>
#include "position_smoother_ab.h"

namespace navigine {
namespace navigation_core {

static const double MAX_ALPHA = 0.9;
static const double MIN_ALPHA = 0.1;
static const double TIME_THRESHOLD_SEC = 10;
static const double MAX_OBJECT_VELOCITY_MPS = 4;

PositionSmootherAB::PositionSmootherAB(const NavigationSettings& navProps)
  : mUseBarriers(navProps.commonSettings.useBarriers)
  , mUseSpeedConstraint(navProps.commonSettings.useSpeedConstraint)
  , mUseAccuracyForSmoothing(navProps.commonSettings.useAccuracyForSmoothing)
  , mWasCalled(false)
  , mSpeedX(0.0)
  , mSpeedY(0.0)
{
  double smoothing = navProps.commonSettings.smoothing;
  smoothing = std::min(1.0, std::max(0.0, smoothing));
  mAlpha = MIN_ALPHA * smoothing + MAX_ALPHA * (1.0 - smoothing);
}

Position PositionSmootherAB::smoothPosition(Position pos, const Level& level)
{
  // assert(pos.ts > mPosition.ts);
  if (pos.ts > mPosition.ts)
  {
    double t = (pos.ts - mPosition.ts) / 1000.0;
    double a = mAlpha;

    if (mUseAccuracyForSmoothing)
    {
      double newAlpha = mPosition.deviationM / (pos.deviationM + mPosition.deviationM + 0.5);
      a = std::min(std::max(MIN_ALPHA, newAlpha), MAX_ALPHA);
    }

    /* temporary commented - it boost the filter to obtain the minimal speed */
    // if (std::sqrt(mSpeedX * mSpeedX + mSpeedY * mSpeedY) < 0.5)
    //   a = MAX_ALPHA / 2.0;
    double b = a * a / (2.0 - a);

    double xp = mPosition.x + mSpeedX * t;
    double vxp = mSpeedX + (b / t) * (pos.x - xp);
    double xs = xp + a * (pos.x - xp);

    double yp = mPosition.y + mSpeedY * t;
    double vyp = mSpeedY + (b / t) * (pos.y - yp);
    double ys = yp + a * (pos.y - yp);
  
    double velocity = std::sqrt(vxp * vxp + vyp * vyp);

    bool timeIsTooLong = t > TIME_THRESHOLD_SEC;
    bool velocityIsTooFast = (mUseSpeedConstraint && velocity > MAX_OBJECT_VELOCITY_MPS);
    bool isInsideBarrier = (mUseBarriers && !boost::geometry::covered_by(Point(xs, ys), level.geometry().allowedArea()));

    if (!mWasCalled || timeIsTooLong || velocityIsTooFast || isInsideBarrier)
    {
      mPosition = pos;
      mSpeedX = 0.0;
      mSpeedY = 0.0;
      mWasCalled = true;
      return pos;
    }

    mSpeedX = vxp;
    mSpeedY = vyp;

    if (std::isfinite(xs) && std::isfinite(ys))
    {
      pos.x = xs;
      pos.y = ys;
    }

    mPosition = pos;
    if (mUseAccuracyForSmoothing)
    {
      //TODO more real accuracy update
      double predictedAccuracy = mPosition.deviationM + t * std::sqrt(mSpeedX * mSpeedX + mSpeedY * mSpeedY);
      mPosition.deviationM = (1.0 - a) * predictedAccuracy + a * pos.deviationM;
    }
  }
  return mPosition;
}

void PositionSmootherAB::reset(Position pos)
{
  mPosition = pos;
  mSpeedX = 0.0;
  mSpeedY = 0.0;
}

} } // namespace navigine::navigation_core
