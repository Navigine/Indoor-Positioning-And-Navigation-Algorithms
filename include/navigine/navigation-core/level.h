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

#include "boost_geometry_adaptation.h"
#include "level_geometry.h"
#include "graph.h"
#include "radiomap.h"
#include "reference_point.h"
#include "transmitter.h"
#include "navigation_input.h"

namespace navigine
{
  namespace navigation_core
  {

    DECLARE_IDENTIFIER(LevelId)

    class Level
    {
    public:
      /**
      * @brief Construct a new Level object
      * 
      * @param id - {type}
      * @param bindingPoint - {type}
      */
      Level(const LevelId &id, const GeoPoint &bindingPoint)
          : mId(id), mBindingPoint(bindingPoint){}

      /**
      * @brief add transmitters location and parameters to the level map
      * 
      * @param transmitters - {type}
      */
      void addTransmitters(const XYZTransmitters &transmitters);
      /**
      * @brief Set the Reference Points object
      * 
      * @param referencePoints - {type}
      * @details used during integration phase when transmitters locations are not known
      */
      void setReferencePoints(const XYReferencePoints &referencePoints);
      /**
       * @brief Add the Graph object to the Level
       * 
       * @param graph - {type}
       * @details graph connects zones with allowed motion
       */
      void setGraph(const Graph<XYPoint> &graph);
      void setGeometry(const LevelGeometry &levelGeometry);

      const LevelId &id() const;
      const GeoPoint &bindingPoint() const;

      bool containsTransmitter(const TransmitterId &txId) const;
      const Transmitter<XYZPoint> &transmitter(const TransmitterId &txId) const;

      const Radiomap &radiomap() const;
      const Graph<XYPoint> &graph() const;
      const LevelGeometry &geometry() const;

    private:
      const LevelId mId;
      const GeoPoint mBindingPoint;

      std::unordered_map<TransmitterId, Transmitter<XYZPoint>, HasherTransmitterId> mTransmitters;
      Radiomap mRadiomap;
      LevelGeometry mGeometry;
      Graph<XYPoint> mGraph;
    };

    typedef std::vector<std::unique_ptr<Level>>::const_iterator LevelIterator;
    typedef std::vector<std::unique_ptr<Level>> Levels;

    RadioMeasurementsData getLevelRadioMeasurements(
        const Level &level,
        const RadioMeasurementsData &radioMsr);

  }
} // namespace navigine::navigation_core

#endif // NAVIGINE_LEVEL_H
