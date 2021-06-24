#include <navigine/navigation-core/navigation_input.h>
#include <navigine/navigation-core/level_collector.h>
#include <navigine/navigation-core/transmitter.h>
#include <navigine/navigation-core/level.h>
#include <navigine/navigation-core/point.h>

#include "likelihood.h"

namespace navigine {
namespace navigation_core {

bool isParticleInIntersectionArea(
  const Level& level,
  const RadioMeasurementsData& radioMsr,
  double x, double y)
{
    for (const auto& msr : radioMsr)
    {
        const Transmitter<XYZPoint> tx = level.transmitter(msr.id);
        const double circleEps = -1e-7;
        const double dx = tx.point.x - x;
        const double dy = tx.point.y - y;
        const double modelValue = std::sqrt(dx * dx + dy * dy);
        const double delta = msr.distance - modelValue;
        if (delta < circleEps)
            return false;
    }
    return true;
};
}
}