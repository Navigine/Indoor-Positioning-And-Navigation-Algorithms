/** knn_utils.cpp
 *
 * Copyright (c) 2018 Navigine.
 *
 */

#include <iterator>
#include <fstream>
#include <tuple>

#include "knn_utils.h"

namespace navigine {
namespace navigation_core {

namespace {
static constexpr double MIN_WEIGHT_EPSILON = 1e-8;
static constexpr bool USE_AP_WEIGHTS = true;
static constexpr bool USE_TRIANGULATION = true;
static constexpr int MIN_NUM_VISIBLE_BS_IN_RP = 3;    // should be more than 0!!!
static constexpr double RSSI_ON_METER = -10.0;
static constexpr double MIN_RP_WEIGHT = 1e-6;         // we skip RPs with 0 weights because they are most improbable,
                                                      // but if all signals coincide to map, we should add small additive
static constexpr double GAMMA = 3.0;
static constexpr double G_WEIGHT_COEF = 1.8;          // Coef to find weight of AP (See Accuracy of RSS-Based Centroid Localization
                                                      // Algorithms in an Indoor Environment
} //namespace

std::map<LevelId, std::vector<RefPointsTriangle>> triangulate(const std::shared_ptr<LevelCollector>& levelCollector)
{
  std::map<LevelId, std::vector<RefPointsTriangle>> levelTriangles;
#ifdef DEBUG_OUTPUT_TRIANGULATION
  std::ofstream debugOs;
  std::string fileName = DEBUG_OUTPUT_TRIANGULATION;
  debugOs.open(fileName);
  debugOs << "level ax ay bx by cx cy " << std::endl;
  debugOs.close();
#endif

  for (const Level& level : levelCollector->levels())
  {
    const XYReferencePoints& referencePoints = level.radiomap().referencePoints();
    if (referencePoints.size() > 3)
    {
      std::vector<TriangleVertex> vertices;
      for(const ReferencePoint<XYPoint>& rp: referencePoints)
      {
        TriangleVertex v = TriangleVertex(rp.point.x, rp.point.y, rp.id);
        vertices.push_back(v);
      }

      auto widthHeightPair = getWidthHeightPair(level.geometry().boundingBox());
      double maxedge = std::max(widthHeightPair.first, widthHeightPair.second);
      std::vector<Triangle> triangles = TriangulateVertices(vertices, maxedge);
      if (triangles.empty())
      {
        // triangulation failed!
        return levelTriangles;
      }
#ifdef DEBUG_OUTPUT_TRIANGULATION
      std::ofstream debugOs;
      std::string fileName = DEBUG_OUTPUT_TRIANGULATION;
      debugOs.open(fileName, std::ios_base::app);
#endif
      std::vector<RefPointsTriangle> refPointsTriangles;
      for (auto it = triangles.begin(); it != triangles.end(); it++)
      {
#ifdef DEBUG_OUTPUT_TRIANGULATION
        Triangle& t = *it;
        debugOs << level.id << " "
                << t.getA().x << " " << t.getA().y << " "
                << t.getB().x << " " << t.getB().y << " "
                << t.getC().x << " " << t.getC().y << std::endl;
#endif
        RefPointsTriangle rpTriangle = {it->getA().id, it->getB().id, it->getC().id};
        refPointsTriangles.push_back(rpTriangle);
      }
#ifdef DEBUG_OUTPUT_TRIANGULATION
      debugOs.close();
#endif

      levelTriangles.insert(std::make_pair(level.id(), refPointsTriangles));
    }
  }
  return levelTriangles;
}

std::map<LevelId,
         std::multimap<TransmitterId,
                       std::pair<ReferencePointId, SignalStatistics>>>
    getLevelsRadiomaps(const std::shared_ptr<LevelCollector> &levelCollector)
{
  std::map<LevelId,
           std::multimap<TransmitterId,
                         std::pair<ReferencePointId, SignalStatistics>>>
      levelsRadiomaps;

  for (const Level& level : levelCollector->levels())
  {
    std::multimap<TransmitterId,
                  std::pair<ReferencePointId, SignalStatistics>> bssidRpToSigstatMap;

    const auto& levelId = level.id();
    for (const ReferencePoint<XYPoint>& rp : level.radiomap().referencePoints())
    {
      const std::map<TransmitterId, SignalStatistics>& rpFingerprint = rp.fingerprints;
      for (const auto& bssidSigStatPair: rpFingerprint)
      {
        bssidRpToSigstatMap.insert({bssidSigStatPair.first,
                                    std::make_pair(rp.id, bssidSigStatPair.second)});
      }
    }
    levelsRadiomaps[levelId] = bssidRpToSigstatMap;
  }
  return levelsRadiomaps;
}

std::map<LevelId, std::set<TransmitterId>> getLevelReferenceTransmittersMap(const std::shared_ptr<LevelCollector> &levelCollector)
{
  std::map<LevelId, std::set<TransmitterId>> levelTransmitters;
  for (const auto& level : levelCollector->levels())
  {
    const LevelId levelId = level.id();
    for (const ReferencePoint<XYPoint>& rp : level.radiomap().referencePoints())
    {
      const std::map<TransmitterId, SignalStatistics>& rpFingerprint = rp.fingerprints;
      std::set<TransmitterId> rpTransmittersIds;
      std::transform(rpFingerprint.begin(), rpFingerprint.end(),
                     std::inserter(rpTransmittersIds, rpTransmittersIds.end()),
                     [](const auto& p){ return p.first; });
      if (rpTransmittersIds.empty())
        continue;
      if (levelTransmitters.find(levelId) == levelTransmitters.end())
        levelTransmitters.insert({levelId, rpTransmittersIds});
      else
        levelTransmitters[levelId].insert(rpTransmittersIds.begin(), rpTransmittersIds.end());
    }
  }
  return levelTransmitters;
}

/**
 * @brief calculate weight of the RadioSignal
 *
 * The function weight the RadioSignal from AP based on this RSSI value.
 * The stronger the signal the bigger weight it will get.
 * The RSSI at some distance d is: RSSI = A + B * gamma * lg (d / d0)
 * RSSI1 - RSSI2 = B * gamma * lg (d1 / d2) and d1 can be found when d2 = 1 meter.
 * The rssi at 1 meter distance from the transmitter assumed to be defined in parameter.
 * Gamma is the pass loss exponent, or attenuation coefficient.
 * For G_WEIGHT_COEF see Accuracy of RSS-Based Centroid Localization Algorithms in an Indoor Environment
 *
 * @param msr RadioMeasurement either from the access point
 *
 * @return @warning unnormalized weight of the measurement.
 */

static double calculateTransmitterTrustworthiness(const RadioMeasurementData& msr)
{
  double B = 10 * GAMMA;
  double txPower = RSSI_ON_METER;
  double gCoef = G_WEIGHT_COEF;

  txPower = msr.power < 0.00001 ? RSSI_ON_METER : msr.power;
  double distance = pow(10, (txPower - msr.rssi) / B); //double apWeight = exp((aps[i].rssi - txPower) / B);
  double apWeight = 1 / pow(distance, gCoef);

  return apWeight;
}

static std::map<TransmitterId, double> calcTransmittersWeights(
        const RadioMeasurementsData& msr)
{
  // Calculate AP weights in the basis of signal strength
  // We should also take into account number of incoming measurements during weights calculation!!!!!!!!!!!!
  // We call this function after averageDuplicateMeasurements
  std::map<TransmitterId, double> trWeightMap;
  double trWeightSum = 0.0;
  for (const auto& m : msr)
  {
    const TransmitterId& trId = m.id;
    double trWeight = USE_AP_WEIGHTS ? calculateTransmitterTrustworthiness(m) : 1.0;
    trWeightMap[trId] = trWeight;
    trWeightSum += trWeight;
  }
  
  for (auto& tp : trWeightMap)
  {
    tp.second /= trWeightSum;
  }

  return trWeightMap;
}

/**
 * This function calculates weights of RPs on the basis of RSS measurements and
 * radio map of the building.
 * The radiomap is multimap of transmitter as keys
 * and pairs of reference points ids and corresponding signal statistics as values
*/
std::map<ReferencePointId, double> calcRpWeightsKnn(
    const std::multimap<TransmitterId,
                        std::pair<ReferencePointId, SignalStatistics>>& radiomap,
    const RadioMeasurementsData& msr,
    bool useDiffMode)
{
  struct SigStat { TransmitterId txId; double rssiMeasured; double rssiDatabase;};

  std::map<ReferencePointId, std::vector<SigStat>> rpsStatistics;
  for (const auto& curMsr : msr)
  {
    auto range = radiomap.equal_range(curMsr.id);

    for (auto it = range.first; it != range.second; it++)
    {
      const std::pair<ReferencePointId, SignalStatistics>& refPointsSsPair = it->second;
      const ReferencePointId& rpId = refPointsSsPair.first;
      if (rpsStatistics.find(rpId) == rpsStatistics.end())
        rpsStatistics[rpId] = std::vector<SigStat>();
      SigStat sigStat = {curMsr.id, curMsr.rssi, refPointsSsPair.second.mean};
      rpsStatistics[rpId].push_back(sigStat);
    }
  }

  std::map<ReferencePointId, double> rpWeights;
  std::map<ReferencePointId, double> rpNormalize;
  std::map<TransmitterId, double> trWeights = calcTransmittersWeights(msr);

  for (const auto& pair : rpsStatistics)
  {
    const auto& rpId = pair.first;
    const auto& vecSigStat = pair.second;
    if (vecSigStat.size() < MIN_NUM_VISIBLE_BS_IN_RP)
      continue;

    for (auto it1 = vecSigStat.begin(); it1 != vecSigStat.end(); it1 ++)
    {
      if (useDiffMode)
      {
        for (auto it2 = vecSigStat.begin(); it2 != it1; it2++)
        {
          double measDiff = it1->rssiMeasured - it2->rssiMeasured;
          double mapDiff = it1->rssiDatabase - it2->rssiDatabase;
            
          // We sum with rp Penalties, to make sum of apWeight = 1 after normalization)
          double apW = trWeights[it1->txId] * trWeights[it2->txId];
          rpNormalize[rpId] += apW;

          double weight = std::max(fabs(mapDiff - measDiff) * apW, MIN_RP_WEIGHT);
          rpWeights[rpId] += weight;
        }
      }
      else
      {
        double apW = trWeights[it1->txId];
        rpNormalize[rpId] += apW;
        double weight = std::max(fabs(it1->rssiDatabase + it1->rssiMeasured) * apW, MIN_RP_WEIGHT);
        rpWeights[rpId] += weight;
      }
    }
  }

  for (auto& pair : rpWeights)
  {
    const ReferencePointId& rpId = pair.first;
    double& rpWeight = pair.second;
    
    if (rpWeight > MIN_WEIGHT_EPSILON)
    {
      // normalize in order to make sum of transmitter weights == 1,
      // because we summed transmitter weights on all APs, not only visible
      if (USE_AP_WEIGHTS)
      {
        auto rpDivider = rpNormalize[rpId];
        if (rpDivider > MIN_WEIGHT_EPSILON)
          rpWeight /= rpDivider;
      }

      // do sqrt because this norm is better (it was shown empirically) 
      // this norm also takes into account
      // the number of visible APS, due to its asymptotic is 1 / sqrt(N)
      rpWeight = std::sqrt(rpWeight);

      // Divide by number of visible APs because it differs from point to point
      // rely more on RP where there are more visible tx from measurements vector
      // for KNN the more penalty the more probability
      auto visibleApNum = rpsStatistics[rpId].size();
      if (visibleApNum > 0)
        rpWeight /= visibleApNum;

      // Revert weights in KNN (to transform weights to probabilities)
      rpWeight = 1.0 / rpWeight;
    }
  }

  return rpWeights;
}

static double getTriangleWeight(const RefPointsTriangle& triangle,
                                const std::map<ReferencePointId, double>& rpWeights)
{
  double w1, w2, w3;
  std::tie(w1, w2, w3) = getValueForEachKey(rpWeights, 0.0, 
                                            triangle.rp1, 
                                            triangle.rp2, 
                                            triangle.rp3);
  return w1 + w2 + w3;
}

XYPoint calcPositionInTriangle(const std::vector<RefPointsTriangle>& triangles,
                               const Radiomap& radiomap,
                               const std::map<ReferencePointId, double>& rpWeights)
{
  auto maxWeightTriangle = std::max_element(triangles.begin(), triangles.end(),
                            [&rpWeights] 
                            (const RefPointsTriangle& lhs, const RefPointsTriangle& rhs) 
                            {
                              return getTriangleWeight(lhs, rpWeights) < getTriangleWeight(rhs, rpWeights);
                            });

  double w1, w2, w3;
  std::tie(w1, w2, w3) = getValueForEachKey(rpWeights, 0.0, 
                                            maxWeightTriangle->rp1, 
                                            maxWeightTriangle->rp2, 
                                            maxWeightTriangle->rp3);
  double norm = w1 + w2 + w3;

  if (norm < std::numeric_limits<double>::epsilon())
  {
    //TODO send error code
    return XYPoint(0.0, 0.0);
  }

  if (!radiomap.hasReferencePoint(maxWeightTriangle->rp1) ||
      !radiomap.hasReferencePoint(maxWeightTriangle->rp2) ||
      !radiomap.hasReferencePoint(maxWeightTriangle->rp3))
  {
    //TODO send error code
    return XYPoint(0.0, 0.0);
  }

  ReferencePoint<XYPoint> rp1 = radiomap.getReferencePoint(maxWeightTriangle->rp1);
  ReferencePoint<XYPoint> rp2 = radiomap.getReferencePoint(maxWeightTriangle->rp2);
  ReferencePoint<XYPoint> rp3 = radiomap.getReferencePoint(maxWeightTriangle->rp3);

  double x = (w1 * rp1.point.x + w2 * rp2.point.x + w3 * rp3.point.x) / norm;
  double y = (w1 * rp1.point.y + w2 * rp2.point.y + w3 * rp3.point.y) / norm;

  return XYPoint(x, y);
}

XYPoint calcKHeaviestRefPointsAverage(const Radiomap& radiomap,
                                     const std::map<ReferencePointId, double>& rpToWeight,
                                     size_t k)
{
  k = std::min(k, rpToWeight.size());
  std::vector<std::pair<ReferencePointId, double>> kHeaviestPoints;
  kHeaviestPoints.reserve(k);
  std::partial_sort_copy(
      rpToWeight.begin(), rpToWeight.end(),
      kHeaviestPoints.begin(), kHeaviestPoints.end(),
      [](std::pair<ReferencePointId, double> const& l,
         std::pair<ReferencePointId, double> const& r)
      {
        return l.second > r.second;
      });

  double x = 0.0;
  double y = 0.0;
  double norm = 0.0;
  for (auto it = kHeaviestPoints.cbegin(); it != kHeaviestPoints.cend(); it++)
  {
    // valid, due to heaviest point build structure
    ReferencePoint<XYPoint> rp = radiomap.getReferencePoint(it->first);
    double w = it->second;
    x += w * rp.point.x;
    y += w * rp.point.y;
    norm += w;
  }

  if (norm < std::numeric_limits<double>::epsilon())
  {
    //TODO check if norm can be zero and process error
    return XYPoint(0.0, 0.0);
  }

  x /= norm;
  y /= norm;

  return XYPoint(x, y);
}

RadioMeasurementsData getWhitelistedMeasurements(
    const std::set<TransmitterId>& wl,
    const RadioMeasurementsData msrs)
{
  RadioMeasurementsData filtered;

  std::copy_if(msrs.begin(), msrs.end(), std::back_inserter(filtered),
    [&wl](const RadioMeasurementData& msr) {return wl.find(msr.id) != wl.end(); });

  return filtered;
}

} } // namespace navigine::navigation_core
