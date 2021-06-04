/** xy_particle.h
 *
 * Copyright (c) 2017 Navigine.
 *
 */

#ifndef NAVIGINE_PARTICLE_H
#define NAVIGINE_PARTICLE_H

#include <point.h>

namespace navigine {
namespace navigation_core {

struct XYParticle : XYPoint
{
  XYParticle()
    : XYPoint ( 0.0, 0.0 )
    , angle   ( 0.0 )
  { }

  XYParticle(double _x, double _y, double _angle)
    : XYPoint( _x, _y )
    , angle  ( _angle )
  { }
  
  double angle;
};

} } // namespace navigine::navigation_core

#endif // NAVIGINE_PARTICLE
