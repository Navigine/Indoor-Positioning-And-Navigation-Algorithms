#pragma once

#include <navigine/navigation-core/geometry.h>

namespace navigine::navigation_core {

template <typename Point>
struct Particle
{
    Point point;

    // Particle direction (aka heading), radians in clockwise direction s.t.:
    //  * angle = 0 directs towards North;
    //  * angle = pi directs towards South;
    //  * angle = pi/2 directs towards East;
    //  * angle = -pi/2 directs towars West.
    double angle = 0.0;

    // Particle speed (in the direction of its heading angle), m/s
    std::optional<double> speed = std::nullopt;

    // Particle weight
    double weight = 0.0;
};

using XYParticle = Particle<XYPoint>;
using GeoParticle = Particle<GeoPoint>;

} // namespace navigine::navigation_core
