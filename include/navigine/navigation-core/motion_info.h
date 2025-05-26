#pragma once

#include <navigine/navigation-core/utils.h>

namespace navigine::navigation_core {

struct MotionInfo
{
    bool hasAccelerometer = false;
    bool hasGyroscope = false;
    bool hasMagnetometer = false;
    bool hasOdometer = false;
    Timestamp ts = 0;
    double deltaSec = 0.0;
    double distance = 0.0;
    double distanceDeviation = 0.0;
    double deltaHeading = 0.0;
    double deltaHeadingDeviation = 0.0;
    std::optional<double> heading = std::nullopt;
    std::optional<double> headingDeviation = std::nullopt;
    std::optional<double> speed = std::nullopt;
    std::optional<double> speedDeviation = std::nullopt;
    std::optional<double> deltaSpeed = std::nullopt;
    std::optional<double> deltaSpeedDeviation = std::nullopt;
    double stopTime = 0.0;
};

} // namespace navigine::navigation_core
