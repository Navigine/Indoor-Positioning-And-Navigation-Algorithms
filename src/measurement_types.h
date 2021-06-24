#pragma once

#include <navigine/navigation-core/navigation_input.h>

namespace navigine {
namespace navigation_core {

using SensorMeasurement = MeasurementDataWithTimeStamp<SensorMeasurementData>; // TODO: m.doroshenko rename me
using NmeaMeasurement = MeasurementDataWithTimeStamp<NmeaMeasurementData>; // TODO: m.doroshenko rename me
using RadioMeasurement = MeasurementDataWithTimeStamp<RadioMeasurementData>; // TODO: m.doroshenko rename me

} } // namespace navigine::navigation_core
