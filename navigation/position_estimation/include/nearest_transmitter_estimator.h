#pragma once

#include <map>
#include "transmitter.h"
#include "navigation_structures.h"
#include "position_smoother.h"

namespace navigine {
namespace navigation_core {

class NearestTransmitterPositionEstimator
{
public:
  NearestTransmitterPositionEstimator(const std::vector<Transmitter>& transmitters);

  Position calculatePosition(const RadioMeasurements& inputMeasurements);

private:
  std::vector<RadioMeasurement> getRegisteredTransmittersMeasurements(
        const std::vector<RadioMeasurement>& radioMsr);
  Point2D getTransmitterPosition(const std::string& txId);

  PositionSmoother m_smoother;
  std::map<std::string, Transmitter> m_transmitters;
};

} } // namespace navigine::navigation_core
