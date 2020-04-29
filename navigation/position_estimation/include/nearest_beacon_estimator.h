#pragma once

#include <map>
#include "transmitter.h"
#include "navigation_structures.h"
#include "position_smoother.h"

namespace navigine {
namespace navigation_core {

class NearestBeaconEstimator
{
public:
  NearestBeaconEstimator(const std::vector<Transmitter>& transmitters);

  Position calculatePosition(const RadioMeasurements& radioMsr);

private:
  Point2D getTransmitterPosition(const std::string& txId);

  PositionSmoother m_smoother;
  std::map<std::string, Transmitter> m_transmitters;
};

} } // namespace navigine::navigation_core
