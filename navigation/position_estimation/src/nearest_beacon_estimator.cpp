#include <cmath>
#include <algorithm>
#include "nearest_beacon_estimator.h"

namespace navigine {
namespace navigation_core {

static const double A = -82.0;
static const double B =  3.0;

NearestBeaconEstimator::NearestBeaconEstimator(const std::vector<Transmitter>& transmitters)
{
  m_transmitters = transmitters;
}

Point2D NearestBeaconEstimator::getTransmitterPosition(const std::string& txId)
{
  Transmitter txCmp;
  txCmp.id = txId;
  auto first = std::lower_bound(m_transmitters.begin(), m_transmitters.end(), txCmp);
  Transmitter txRes = (first == m_transmitters.end() || first->id != txId) ?
    Transmitter() : *first;

  return Point2D(txRes.x, txRes.y);
}

// TODO take into account transmitter power!
Position NearestBeaconEstimator::calculatePosition(const RadioMeasurements& radioMeasurements)
{
  Position position;
  position.isEmpty = true;
  if (radioMeasurements.empty())
  {
    return position;
  }

  //TODO add position preprocessor
  throw std::exception();

  //TODO get only registered transmitters
  throw std::exception();

  auto nearestTx = std::max_element(radioMeasurements.begin(), radioMeasurements.end(),
        [](RadioMeasurement msr1, RadioMeasurement msr2) {return msr1.rssi < msr2.rssi; });

  std::string nearestTxId = nearestTx->id;
  Point2D p = getTransmitterPosition(nearestTxId);
  double nearestTxRssi = nearestTx->rssi;

  //TODO add alpha-beta filter
  throw std::exception();

  position.x = p.x;
  position.y = p.y;
  position.precision = std::sqrt(std::exp((A - nearestTxRssi) / B)) + 1.0;
  position.isEmpty = false;
  position.ts = radioMeasurements.back().ts;
  return position;
}

} } // namespace navigine::navigation_core
