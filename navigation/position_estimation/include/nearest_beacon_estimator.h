#pragma once

#include "transmitter.h"
#include "navigation_structures.h"

namespace navigine {
namespace navigation_core {

class NearestBeaconEstimator
{
public:
  NearestBeaconEstimator(const std::vector<Transmitter>& m_transmitters);

  Position calculatePosition(const RadioMeasurements& radioMsr);

private:
  Point2D getTransmitterPosition(const std::string& txId);

  std::vector<Transmitter> m_transmitters;
};

} } // namespace navigine::navigation_core
