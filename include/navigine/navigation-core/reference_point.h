#pragma once

#include <map>

#include <navigine/navigation-core/transmitter.h>
#include <navigine/navigation-core/utils.h>

namespace navigine::navigation_core {

///
/// SignalStatistics
///

struct SignalStatistics
{
    TransmitterType type = TransmitterType::UNKNOWN;
    double mean = 0.0;
    double variance = 0.0;
    unsigned int nMeasurements = 0;
};

///
/// ReferencePoint
///

using ReferencePointId = std::string;

template <typename Point>
struct ReferencePoint
{
    ReferencePointId id;
    Point point;
    std::map<TransmitterId, SignalStatistics> fingerprints;
};

using XYReferencePoint = ReferencePoint<XYPoint>;
using XYReferencePoints = std::vector<XYReferencePoint>;

using GeoReferencePoint = ReferencePoint<GeoPoint>;
using GeoReferencePoints = std::vector<GeoReferencePoint>;

} // namespace navigine::navigation_core
