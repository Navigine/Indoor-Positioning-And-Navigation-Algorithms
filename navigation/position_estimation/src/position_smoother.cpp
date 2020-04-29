#include "position_smoother.h"

namespace navigine {
namespace navigation_core {

static const double MAX_ALPHA = 0.9;
static const double MIN_ALPHA = 0.1;
static const double TIME_THRESHOLD_SEC = 10;

PositionSmoother::PositionSmoother(double smoothing)
    : mWasCalled ( false )
    , mSpeedX    ( 0.0 )
    , mSpeedY    ( 0.0 )
{
  smoothing = std::min(1.0, std::max(0.0, smoothing));
  mAlpha = MIN_ALPHA * smoothing + MAX_ALPHA * (1.0 - smoothing);
}

Position PositionSmoother::smoothPosition(Position pos)
{
  if (pos.ts > mPosition.ts)
  {
    double t = (pos.ts - mPosition.ts) / 1000.0;
    double a = mAlpha;
    double b = a * a / (2.0 - a);

    double xp = mPosition.x + mSpeedX * t;
    double vxp = mSpeedX + (b / t) * (pos.x - xp);
    double xs = xp + a * (pos.x - xp);

    double yp = mPosition.y + mSpeedY * t;
    double vyp = mSpeedY + (b / t) * (pos.y - yp);
    double ys = yp + a * (pos.y - yp);

    bool timeIsTooLong = t > TIME_THRESHOLD_SEC;

    if (!mWasCalled || timeIsTooLong)
    {
      mPosition = pos;
      mSpeedX = 0.0;
      mSpeedY = 0.0;
      mWasCalled = true;
      return pos;
    }

    mSpeedX = vxp;
    mSpeedY = vyp;

    pos.x = xs;
    pos.y = ys;

    mPosition = pos;
  }

  return mPosition;
}

} } // namespace navigine::navigation_core
