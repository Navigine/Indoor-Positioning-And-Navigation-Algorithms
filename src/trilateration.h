#pragma once
#include <boost/optional.hpp>
#include <navigine/navigation-core/level.h>

namespace navigine {
namespace navigation_core {

class Trilateration
{
    public:
        boost::optional<double> calculateAltitude(const Level& level, const RadioMeasurementsData& radioMeasurements);
};

} } // namespace navigine::navigation_core
