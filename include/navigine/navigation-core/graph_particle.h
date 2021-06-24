/** graph_particle.h
 *
 * Copyright (c) 2017 Navigine.
 *
 */

#ifndef NAVIGINE_GRAPH_PARTICLE_H
#define NAVIGINE_GRAPH_PARTICLE_H

#include <navigine/navigation-core/xy_particle.h>

namespace navigine {
namespace navigation_core {

struct GraphParticle : XYParticle
{
  GraphParticle()
    : edgeId(0)
    , fromVertex(0)
    , toVertex(0)
    , percent(0)
  { }

  GraphParticle(int _edge, int _fromVertex, int _toVertex,
                double _percent, double _x, double _y, double _angle)
    : XYParticle(_x, _y, _angle)
    , edgeId(_edge)
    , fromVertex(_fromVertex)
    , toVertex(_toVertex)
    , percent(_percent)
  { }

  int edgeId;
  int fromVertex;
  int toVertex;
  double percent;
};

} } // namespace navigine::navigation_core

#endif // NAVIGINE_GRAPH_PARTICLE_H
