/** level.h
*
* Copyright (c) 2019 Navigine.
*
*/

#ifndef NAVIGINE_LEVEL_H
#define NAVIGINE_LEVEL_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#include "level_geometry.h"
#include "transmitter.h"
#include "navigation_input.h"

namespace navigine {
namespace navigation_core {

DECLARE_IDENTIFIER(LevelId)

class Level
{
public:
  Level(const LevelId& id, const GeoPoint& bindingPoint)
    : mId (id)
    , mBindingPoint (bindingPoint)
  {}

  void addTransmitters(const XYZTransmitters& transmitters);
  void setGeometry(const LevelGeometry &levelGeometry);

  const LevelId& id() const;
  const GeoPoint& bindingPoint() const;

  bool containsTransmitter(const TransmitterId& txId) const;
  const Transmitter<XYZPoint>& transmitter(const TransmitterId& txId) const;

  const LevelGeometry& geometry() const;

private:
  const LevelId mId;
  const GeoPoint mBindingPoint;

  std::unordered_map<TransmitterId, Transmitter<XYZPoint>, HasherTransmitterId> mTransmitters;
  LevelGeometry mGeometry;
};

typedef std::vector<std::unique_ptr<Level> >::const_iterator LevelIterator;
typedef std::vector<std::unique_ptr<Level> > Levels;

RadioMeasurementsData getLevelRadioMeasurements(
  const Level& level,
  const RadioMeasurementsData& radioMsr);

} } // namespace navigine::navigation_core

#endif // NAVIGINE_LEVEL_H
