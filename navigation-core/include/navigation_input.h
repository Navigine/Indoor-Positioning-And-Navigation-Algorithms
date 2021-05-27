/** navigation_input.h
 *
 * Author: Fedor Puchkov <fedormex@gmail.com>
 * Copyright (c) 2018 Navigine. All rights reserved.
 *
 */

#ifndef NAVIGINE_NAVIGATION_INPUT_H
#define NAVIGINE_NAVIGATION_INPUT_H
#define _CRTDBG_MAP_ALLOC

#include <string>
#include <vector>
#include "boost/variant.hpp"

#include "vector3d.h"
#include "transmitter.h"

namespace navigine {
namespace navigation_core {

template <typename MeasurementData>
struct MeasurementDataWithTimeStamp
{
  MeasurementData data;
  long long ts;
};

struct RadioMeasurementData
{
  enum class Type {
    WIFI,
    BEACON,
    BLUETOOTH,
    EDDYSTONE,
    WIFI_RTT
  };

  Type type;
  TransmitterId id;
  double rssi = 0;
  double power = 0;
  double distance = 0;
  double stddev = 0;
};

struct SensorMeasurementData
{
  enum class Type {
    ACCELEROMETER,
    MAGNETOMETER,
    GYROSCOPE,
    BAROMETER,
    LOCATION,
    ORIENTATION
  };

  Type       type;
  Vector3d   values;
};

struct NmeaMeasurementData
{
  int sentenceNumber;
  int satellitesNumber;
};

using RadioMeasurementsData = std::vector<RadioMeasurementData>;

using MeasurementData = boost::variant<RadioMeasurementData, SensorMeasurementData, NmeaMeasurementData>;

using Measurement = MeasurementDataWithTimeStamp<MeasurementData>;

} } // namespace navigine::navigation_core

#endif
