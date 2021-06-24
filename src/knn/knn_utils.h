/** knn_utils.h
 *
 * Copyright (c) 2018 Navigine.
 *
 */

#ifndef KNN_UTILS_H
#define KNN_UTILS_H

#include <navigine/navigation-core/navigation_output.h>
#include <navigine/navigation-core/point.h>
#include <navigine/navigation-core/navigation_input.h>
#include <navigine/navigation-core/level_collector.h>

#include <set>

#include "../triangulation.h"

namespace navigine {
namespace navigation_core {

struct RefPointsTriangle
{
  ReferencePointId rp1;
  ReferencePointId rp2;
  ReferencePointId rp3;
};

struct TriangleVerticesWeights
{
  double w1;
  double w2;
  double w3;
};

std::map<LevelId, std::vector<RefPointsTriangle>> triangulate(const std::shared_ptr<LevelCollector>& levelCollector);

std::map<LevelId,
         std::multimap<TransmitterId, std::pair<ReferencePointId, SignalStatistics>>>
    getLevelsRadiomaps(const std::shared_ptr<LevelCollector>& levelCollector);

std::map<LevelId, std::set<TransmitterId>> getLevelReferenceTransmittersMap(
  const std::shared_ptr<LevelCollector>& levelCollector);

std::map<ReferencePointId, double> calcRpWeightsKnn(
  const std::multimap<TransmitterId,
                      std::pair<ReferencePointId, SignalStatistics> >& radiomap,
  const RadioMeasurementsData& msr,
  bool useDiffMode = true);

XYPoint calcPositionInTriangle(const std::vector<RefPointsTriangle>& triangles,
                               const Radiomap& rps,
                               const std::map<ReferencePointId, double>& rpToWeight);

XYPoint calcKHeaviestRefPointsAverage(const Radiomap& radiomap,
                                     const std::map<ReferencePointId, double>& rpToWeight,
                                     size_t k);

RadioMeasurementsData getWhitelistedMeasurements(
  const std::set<TransmitterId>& wl,
  const RadioMeasurementsData msrs);

template<typename KeyType, typename ValType> 
 ValType func_helper(const std::map<KeyType, ValType>& map, KeyType key, ValType defVal) {
    if (map.find(key) == map.end())
      return defVal;
    else
      return map.find(key)->second;  
}

template<typename ...Keys, typename ValType, typename KeyType>
auto getValueForEachKey(const std::map<KeyType, ValType>& map, ValType defVal,
          Keys... keys) -> decltype(std::make_tuple(func_helper(map, keys, defVal)...))  {
    return std::make_tuple(func_helper(map, keys, defVal)...);
}

} } // namespace navigine::navigation_core

#endif // KNN_UTILS_H
