/** level.cpp
 *
 * Copyright (c) 2019 Navigine.
 *
 */

#include <level.h>

namespace navigine {
namespace navigation_core {

const GeoPoint& Level::bindingPoint() const
{
  return mBindingPoint;
}

const LevelId& Level::id() const
{
  return mId;
}

const LevelGeometry& Level::geometry() const
{
  return mGeometry;
}

bool Level::containsTransmitter(const TransmitterId& txId) const
{
  return mTransmitters.find(txId) != mTransmitters.end();
}

const Transmitter<XYZPoint>& Level::transmitter(const TransmitterId& txId) const
{
  return mTransmitters.at(txId);
}

void Level::addTransmitters(const XYZTransmitters& transmitters)
{
  for (const Transmitter<XYZPoint>& tx: transmitters)
    mTransmitters.insert({tx.id, tx});
}

void Level::setGeometry(const LevelGeometry& levelGeometry)
{
  mGeometry = levelGeometry;
}

RadioMeasurementsData getLevelRadioMeasurements(
  const Level& level,
  const RadioMeasurementsData& radioMsr)
{
  RadioMeasurementsData levelRadioMeasurements;
  for (const RadioMeasurementData& msr: radioMsr)
  {
    if (level.containsTransmitter(msr.id))
      levelRadioMeasurements.push_back(msr);
  }
  return levelRadioMeasurements;
}

} } // namespace navigine::navigation_core
