#pragma once

#include <optional>

#include <navigine/navigation-core/geometry.h>
#include <navigine/navigation-core/level.h>
#include <navigine/navigation-core/utils.h>
#include <navigine/navigation-core/vector3d.h>

namespace navigine::navigation_core {

struct Position
{
    GeoPoint point;
    double accuracy = 0.0;                                  // Position accuracy (m)
    std::optional<double> altitude = std::nullopt;          // Absolute altitude above the sea level (m)
    std::optional<double> altitudeAccuracy = std::nullopt;  // Absolute altitude accuracy (m)
    std::optional<double> speed = std::nullopt;             // Device speed in horizontal plane (m/s)
    std::optional<double> speedVertical = std::nullopt;     // Device speed in vertical direction (m/s, positive in "up" direction)
    std::optional<double> trackAngle = std::nullopt;        // Device track angle (in radians, clockwise: north=0, south=pi, east=pi/2, west=-pi/2)
    std::optional<double> heading = std::nullopt;           // Radians (as in orientation.h)
    std::optional<double> headingAccuracy = std::nullopt;   // Radians
    std::optional<double> pitch = std::nullopt;             // Radians (as in orientation.h)
    std::optional<double> roll = std::nullopt;              // Radians (as in orientation.h)
    std::optional<LevelId> levelId = std::nullopt;
    Timestamp timestamp = 0;
};

} // namespace navigine::navigation_core
