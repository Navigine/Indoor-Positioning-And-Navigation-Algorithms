/** reference_point.h
 *
 * Copyright (c) 2017 Navigine.
 *
 */

#ifndef NAVIGINE_REFERENCE_POINT_H
#define NAVIGINE_REFERENCE_POINT_H

#include <ctime>
#include <string>
#include <map>
#include "transmitter.h"

namespace navigine {
namespace navigation_core {

struct SignalStatistics
{
  TransmitterType type;
  double mean;
  double variance;
  size_t nMeasurements;

  void addMeasurement(double meas);
};

DECLARE_IDENTIFIER(ReferencePointId)

template <typename Point>
struct ReferencePoint
{
  ReferencePoint(
    ReferencePointId _id,
    Point _point,
    const std::map<TransmitterId, SignalStatistics>& _fingerprints)
      : id(_id)
      , point(_point)
      , fingerprints(_fingerprints)
  {}

  ReferencePointId id;
  Point point;
  std::map<TransmitterId, SignalStatistics> fingerprints;
};

typedef std::vector<ReferencePoint<XYPoint>> XYReferencePoints;
typedef std::vector<ReferencePoint<GeoPoint>> GeoReferencePoints;

} } // namespace navigine::navigation_core

#endif // NAVIGINE_REFERENCE_POINT_H
