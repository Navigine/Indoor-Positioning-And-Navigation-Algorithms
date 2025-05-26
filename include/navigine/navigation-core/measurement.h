#pragma once

#include <string>
#include <variant>
#include <vector>

#include <navigine/navigation-core/geometry.h>
#include <navigine/navigation-core/vectorXd.h>
#include <navigine/navigation-core/matrixXd.h>
#include <navigine/navigation-core/transmitter.h>
#include <navigine/navigation-core/utils.h>

namespace navigine::navigation_core {

///
/// Abstract measurement data
///

template <typename MeasurementData>
struct MeasurementDataWithTimeStamp
{
    MeasurementData data = {};
    Timestamp ts = 0;
};

///
/// Radio measurement
///

struct RadioMeasurementData
{
    enum class Type {
        WIFI,
        BEACON,
        BLUETOOTH,
        EDDYSTONE,
        WIFI_RTT,
        NONE,
    };

    Type type = Type::NONE;
    TransmitterId id;
    double rssi = 0.0;
    double distance = 0.0;
    double stddev = 0.0;
};

std::string RadioMeasurementTypeToString(RadioMeasurementData::Type type);
RadioMeasurementData::Type RadioMeasurementTypeFromString(const std::string& str);

inline bool isRssiMeasurement(RadioMeasurementData::Type type)
{
    return type == RadioMeasurementData::Type::WIFI ||
           type == RadioMeasurementData::Type::BEACON ||
           type == RadioMeasurementData::Type::BLUETOOTH ||
           type == RadioMeasurementData::Type::EDDYSTONE;
}

inline bool isRttMeasurement(RadioMeasurementData::Type type)
{
    return type == RadioMeasurementData::Type::WIFI_RTT;
}

using RadioMeasurement = MeasurementDataWithTimeStamp<RadioMeasurementData>;
using RadioMeasurements = std::vector<RadioMeasurement>;
using RadioMeasurementsData = std::vector<RadioMeasurementData>;

///
/// Sensor measurement
///

struct SensorMeasurementData
{
    enum class Type {
        ACCELEROMETER,
        MAGNETOMETER,
        GYROSCOPE,
        BAROMETER,
        ORIENTATION,
        NONE,
    };

    Type type = Type::NONE;
    Vector3d values = {};
};

std::string SensorMeasurementTypeToString(SensorMeasurementData::Type type);
SensorMeasurementData::Type SensorMeasurementTypeFromString(const std::string& str);

using SensorMeasurement = MeasurementDataWithTimeStamp<SensorMeasurementData>;
using SensorMeasurements = std::vector<SensorMeasurement>;
using SensorMeasurementsData = std::vector<SensorMeasurementData>;

///
/// Location measurement
///

struct LocationMeasurementData
{
    std::string provider;
    GeoPoint point;
    double accuracy = 0.0;
    std::optional<double> altitude = std::nullopt;
    std::optional<double> altitudeAccuracy = std::nullopt;
    std::optional<double> speed = std::nullopt;             // Speed in horizontal (east,north) direction [m/s]
    std::optional<double> speedVertical = std::nullopt;     // Speed in vertical (up) direction [m/s]
    std::optional<double> trackAngle = std::nullopt;        // Clockwise angle between North direction and device track direction [rad]
    std::optional<LevelId> levelId = std::nullopt;
};

using LocationMeasurement = MeasurementDataWithTimeStamp<LocationMeasurementData>;
using LocationMeasurements = std::vector<LocationMeasurement>;
using LocationMeasurementsData = std::vector<LocationMeasurementData>;

///
/// NMEA measurement
///

struct NmeaMeasurementData
{
    unsigned int sentenceNumber = 0;
    unsigned int satellitesNumber = 0;
};

using NmeaMeasurement = MeasurementDataWithTimeStamp<NmeaMeasurementData>;

///
/// Odometer measurement
///

struct OdometerMeasurementData
{
    double speed = 0.0;
    std::optional<double> accuracy = std::nullopt;
};

using OdometerMeasurement = MeasurementDataWithTimeStamp<OdometerMeasurementData>;

///
/// GNSS measurement
///

struct GnssMeasurementData
{
    VectorXd pseudoRanges = {};
    VectorXd pseudoRangeRates = {};
    MatrixXd satellitesPositions = {};
    MatrixXd satellitesVelocities = {};
};

using GnssMeasurement = MeasurementDataWithTimeStamp<GnssMeasurementData>;

///
/// Abstract measurement
///

using MeasurementData = std::variant<
    RadioMeasurementData,
    SensorMeasurementData,
    LocationMeasurementData,
    NmeaMeasurementData,
    OdometerMeasurementData,
    GnssMeasurementData>;

static constexpr int RADIO_MEASUREMENT_DATA_INDEX = 0;
static constexpr int SENSOR_MEASUREMENT_DATA_INDEX = 1;
static constexpr int LOCATION_MEASUREMENT_DATA_INDEX = 2;
static constexpr int NMEA_MEASUREMENT_DATA_INDEX = 3;
static constexpr int ODOMETER_MEASUREMENT_DATA_INDEX = 4;
static constexpr int GNSS_MEASUREMENT_DATA_INDEX = 5;

using Measurement = MeasurementDataWithTimeStamp<MeasurementData>;
using Measurements = std::vector<Measurement>;

} // namespace navigine::navigation_core
