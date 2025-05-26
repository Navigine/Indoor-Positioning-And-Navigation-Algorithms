#pragma once

#include <string>
#include <vector>
#include <cmath>

#include <navigine/navigation-core/geometry.h>
#include <navigine/navigation-core/utils.h>
#include <navigine/navigation-core/math/math.h>

namespace navigine::navigation_core {

using TransmitterId = std::string;

///
/// TransmitterType
///

enum class TransmitterType {
    WIFI,
    BEACON,
    BLUETOOTH,
    EDDYSTONE,
    LOCATOR,
    UNKNOWN,
};

TransmitterType transmitterTypeFromString(const std::string& type);
std::string transmitterTypeToString(TransmitterType type);

///
/// TransmitterRssiModel
///

struct TransmitterRssiModel
{
    double paramA = -82.0;
    double paramB = 3.0;
    std::optional<double> rssiThreshold;
    std::optional<double> rssiDeviation;
};

inline double RssiByDistance(double A, double B, double dist)
{
    static const auto MIN_DISTANCE = 0.01;
    return A - 2 * B * math::log(std::max(dist, MIN_DISTANCE));
}

inline double DistanceByRssi(double A, double B, double rssi)
{
    return math::exp((A - rssi) / B / 2);
}

///
/// Transmitter
///

template <typename Point>
struct Transmitter
{
    TransmitterId id;
    TransmitterType type = TransmitterType::UNKNOWN;
    TransmitterRssiModel rssiModel;
    Point point;
    std::optional<double> altitude;
};

using XYTransmitter = Transmitter<XYPoint>;
using XYTransmitters = std::vector<XYTransmitter>;

using GeoTransmitter = Transmitter<GeoPoint>;
using GeoTransmitters = std::vector<GeoTransmitter>;

} // namespace navigine::navigation_core
