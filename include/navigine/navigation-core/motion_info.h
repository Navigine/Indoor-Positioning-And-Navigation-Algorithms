/** motion_info.h
 *
 * Copyright (c) 2017 Navigine.
 *
 */

#ifndef NAVIGINE_MOTION_INFO_H
#define NAVIGINE_MOTION_INFO_H

#include <cstddef>

namespace navigine {
namespace navigation_core {

struct MotionInfo
{
  MotionInfo()
    : isStepDetected ( false )
    , dtSec          ( 0.0   )
    , deltaAngle     ( 0.0   )
    , distance       ( 0.0   )
    , stepCounter    ( 0     )
    , heading        ( 0.0   )
    , azimuth        ( 0.0   )
    , lastMotionTs   ( 0     )
    , isAzimuthValid (false)
    , gyroHeading    ( 0.0   )
  {}

  bool      isStepDetected;
  double    dtSec;
  double    deltaAngle;
  double    distance;
  size_t    stepCounter;
  double    heading;
  double    azimuth;
  long long lastMotionTs;
  bool      isAzimuthValid;
  double    gyroHeading;
};

} } // namespace navigine::navigation_core

#endif // NAVIGINE_MOTION_INFO_H
