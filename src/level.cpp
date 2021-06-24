/** level.cpp
 *
 * Copyright (c) 2019 Navigine.
 *
 */

#include <navigine/navigation-core/level.h>

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

const Radiomap &Level::radiomap() const
{
  return mRadiomap;
}

const Graph<XYPoint>& Level::graph() const
{
  return mGraph;
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

//TODO after second call should reference points be added to existed or replaced?
void Level::setReferencePoints(const XYReferencePoints& referencePoints)
{
  mRadiomap = Radiomap(referencePoints);
}

void Level::setGraph(const Graph<XYPoint>& graph)
{
  mGraph = graph;
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
