#include <navigation_input.h>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#include "helpers.h"
#include "../../include/barriers_geometry_builder.h"
#include "../../src/navigation_error_codes.h"
#include "../../extlibs/nlohmann/json.hpp"

using namespace navigine::navigation_core;

static double const GRAPH_SAME_POINTS_GEO_DIST = 0.1;

namespace nlohmann {

NLOHMANN_JSON_SERIALIZE_ENUM(navigine::navigation_core::RadioMeasurementData::Type, {
  {navigine::navigation_core::RadioMeasurementData::Type::WIFI, 201},
  {navigine::navigation_core::RadioMeasurementData::Type::BEACON, 203},
  {navigine::navigation_core::RadioMeasurementData::Type::BLUETOOTH, 202},
  {navigine::navigation_core::RadioMeasurementData::Type::WIFI_RTT, 205}
})

NLOHMANN_JSON_SERIALIZE_ENUM(navigine::navigation_core::TransmitterType, {
  {navigine::navigation_core::TransmitterType::WIFI, "WIFI"},
  {navigine::navigation_core::TransmitterType::BEACON, "BEACON"},
  {navigine::navigation_core::TransmitterType::BLUETOOTH, "BLUETOOTH"},
  {navigine::navigation_core::TransmitterType::EDDYSTONE, "EDDYSTONE"},
  {navigine::navigation_core::TransmitterType::LOCATOR, "LOCATOR"},
  {navigine::navigation_core::TransmitterType::UNKNOWN, "UNKNOWN"}
})

NLOHMANN_JSON_SERIALIZE_ENUM(navigine::navigation_core::SensorMeasurementData::Type, {
  {navigine::navigation_core::SensorMeasurementData::Type::ACCELEROMETER, 101},
  {navigine::navigation_core::SensorMeasurementData::Type::GYROSCOPE, 102},
  {navigine::navigation_core::SensorMeasurementData::Type::MAGNETOMETER, 103},
  {navigine::navigation_core::SensorMeasurementData::Type::BAROMETER, 104},
  {navigine::navigation_core::SensorMeasurementData::Type::LOCATION, 106},
  {navigine::navigation_core::SensorMeasurementData::Type::ORIENTATION, 105}
})

NLOHMANN_JSON_SERIALIZE_ENUM(navigine::navigation_core::CommonSettings::UseAlgorithm, {
  {navigine::navigation_core::CommonSettings::UseAlgorithm::PF_PDR, "PF_PDR"},
  {navigine::navigation_core::CommonSettings::UseAlgorithm::PF_NOPDR, "PF_NOPDR"},
  {navigine::navigation_core::CommonSettings::UseAlgorithm::ZONES, "ZONES"},
  {navigine::navigation_core::CommonSettings::UseAlgorithm::GRAPH, "GRAPH"},
  {navigine::navigation_core::CommonSettings::UseAlgorithm::KNN, "KNN"}
})

NLOHMANN_JSON_SERIALIZE_ENUM(navigine::navigation_core::CommonSettings::MeasurementType, {
  {navigine::navigation_core::CommonSettings::MeasurementType::RSSI, "rssi"},
  {navigine::navigation_core::CommonSettings::MeasurementType::DISTANCE_V1, "distance_v1"},
  {navigine::navigation_core::CommonSettings::MeasurementType::DISTANCE_V2, "distance_v2"}
})

NLOHMANN_JSON_SERIALIZE_ENUM(navigine::navigation_core::CommonSettings::SignalsToUse, {
  {navigine::navigation_core::CommonSettings::SignalsToUse::BLE, "ble"},
  {navigine::navigation_core::CommonSettings::SignalsToUse::WIFI, "wifi"},
  {navigine::navigation_core::CommonSettings::SignalsToUse::BOTH, "both"}
})

static void from_json(const nlohmann::json& j, SensorMeasurementData& sensorMsr)
{
  j.at("type").get_to(sensorMsr.type);
  std::vector<double> values;
  j.at("values").get_to(values);
  if (values.size() == 3)
    sensorMsr.values = Vector3d(values.at(0), values.at(1), values.at(2));
}

static void from_json(const nlohmann::json& j, RadioMeasurementData& radioMsr)
{
  j.at("type").get_to(radioMsr.type);

  std::string id;
  j.at("bssid").get_to(id);
  radioMsr.id = TransmitterId(id);

  if (j.count("rssi"))
    j.at("rssi").get_to(radioMsr.rssi);

  if (j.count("distance"))
    j.at("distance").get_to(radioMsr.distance);
}

static void from_json(const nlohmann::json& j, NmeaMeasurementData& nmeaMsr)
{
  j.at("sentence_number").get_to(nmeaMsr.sentenceNumber);
  j.at("num_sats").get_to(nmeaMsr.satellitesNumber);
}

static void from_json(const nlohmann::json& j, Measurement& msr)
{
  j.at("timestamp").get_to(msr.ts);
  int msrType;
  j.at("type").get_to(msrType);
  if (msrType == 101 || msrType == 102 || msrType == 103 || msrType == 104 || msrType == 105 || msrType == 106)
    msr.data = j.get<SensorMeasurementData>();
  else if (msrType == 201 || msrType == 202 || msrType == 203 || msrType == 204 || msrType == 205)
    msr.data = j.get<RadioMeasurementData>();
  else if (msrType == 300)
    msr.data = j.get<NmeaMeasurementData>();
  else
    std::cerr << "Unknown measurement." << std::endl;
}

static void from_json(const nlohmann::json& j, CommonSettings& commonSettings)
{
  std::vector<std::string> keys;
  for (auto& el : j.items())
  {
    keys.push_back(el.key());
  }
  try 
  {
    for (const auto& key : keys)
    {
      if (key == "Use_algorithm")
        j.at(key).get_to(commonSettings.useAlgorithm);
      else if (key == "Correction_noise")
        j.at(key).get_to(commonSettings.lklCorrectionNoise);
      else if (key == "Use_enu_azimuth")
        j.at(key).get_to(commonSettings.useEnuAzimuth);
      else if (key == "Differention_mode")
        j.at(key).get_to(commonSettings.useDifferentionMode);
      else if (key == "Use_beacon_power")
        j.at(key).get_to(commonSettings.useTxPower);
      else if (key == "Use_radiomap")
        j.at(key).get_to(commonSettings.useRadiomap);
      else if (key == "Use_stops")
        j.at(key).get_to(commonSettings.useStops);
      else if (key == "Use_tracking")
        j.at(key).get_to(commonSettings.useTracking);
      else if (key == "Stop_detection_time")
        j.at(key).get_to(commonSettings.stopDetectionTime);
      else if (key == "Min_deviation_m")
        j.at(key).get_to(commonSettings.minDeviationM);
      else if (key == "Average_speed")
        j.at(key).get_to(commonSettings.averageMovSpeed);
      else if (key == "Measurement_type")
        j.at(key).get_to(commonSettings.measurementType);
      else if (key == "Use_best_ref_point_level")
        j.at(key).get_to(commonSettings.useBestRefPointLevel);
      else if (key == "Use_barometer")
        j.at(key).get_to(commonSettings.useBarometer);

      else if (key == "Use_unknown_beacons")
        j.at(key).get_to(commonSettings.useUnknownTransmitters);
      else if (key == "Cutoff_rss")
        j.at(key).get_to(commonSettings.sigCutOffRss);
      else if (key == "Use_closest_aps")
        j.at(key).get_to(commonSettings.useClosestAps);
      else if (key == "N_closest_aps")
        j.at(key).get_to(commonSettings.numClosestAps);
      else if (key == "Sig_averaging_time")
        j.at(key).get_to(commonSettings.sigAveragingTime);
      else if (key == "Sig_window_shift")
        j.at(key).get_to(commonSettings.sigWindowShift);
      else if (key == "Use_Signals")
        j.at(key).get_to(commonSettings.signalsToUse);

      else if (key == "N_particles_location_change")
        j.at(key).get_to(commonSettings.numParticles);
      else if (key == "Min_msr_num_for_mutation")
        j.at(key).get_to(commonSettings.minMsrNumForMutation);
      else if (key == "Resampling_threshold_num")
        j.at(key).get_to(commonSettings.resamplingThreshold);
      else if (key == "Min_num_of_particles")
        j.at(key).get_to(commonSettings.minNumParticlesAlive);
      else if (key == "Gyro_noise_degree")
        j.at(key).get_to(commonSettings.gyroNoiseDegree);
      else if (key == "Step_noise_deviation_meters")
        j.at(key).get_to(commonSettings.stepNoiseDeviationMeters);
      else if (key == "Use_projection_to_white_area")
        j.at(key).get_to(commonSettings.useProjectionToWhiteArea);

      else if (key == "N_particles_around_closest_AP")
        j.at(key).get_to(commonSettings.numParticlesAroundClosestAp);
      else if (key == "N_particles_to_mutate")
        j.at(key).get_to(commonSettings.numParticlesToMutate);
      else if (key == "Num_sampling_attempts")
        j.at(key).get_to(commonSettings.numSamplingAttempts);
      else if (key == "Num_initialization_attempts")
        j.at(key).get_to(commonSettings.numInitializationAttempts);
      else if (key == "Use_uniform_sampling")
        j.at(key).get_to(commonSettings.useUniformSampling);

      else if (key == "K")
        j.at(key).get_to(commonSettings.kNeighbors);
      else if (key == "Use_triangles")
        j.at(key).get_to(commonSettings.useTriangles);
      else if (key == "Use_diff_mode")
        j.at(key).get_to(commonSettings.useDiffMode);
      else if (key == "Min_num_of_measurements_for_position_calculation")
        j.at(key).get_to(commonSettings.minNumOfMeasurementsForPositionCalculation);

      else if (key == "Smoothing")
        j.at(key).get_to(commonSettings.smoothing);
      else if (key == "Dead_reckon_time")
        j.at(key).get_to(commonSettings.deadReckoningTime);
      else if (key == "Stop_update_time")
        j.at(key).get_to(commonSettings.stopUpdateTime);
      else if (key == "Graph_prog_distance")
        j.at(key).get_to(commonSettings.graphProjDist);
      else if (key == "Stopped_distance_threshold_m")
        j.at(key).get_to(commonSettings.useStopsDistanceThresholdM);

      else if (key == "Use_smoothing")
        j.at(key).get_to(commonSettings.useSmoothing);
      else if (key == "Use_graph_projection")
        j.at(key).get_to(commonSettings.useGraphProjection);
      else if (key == "Use_accuracy_for_smoothing")
        j.at(key).get_to(commonSettings.useAccuracyForSmoothing);
      else if (key == "Use_speed_constraint")
        j.at(key).get_to(commonSettings.useSpeedConstraint);
      else if (key == "Use_barriers")
        j.at(key).get_to(commonSettings.useBarriers);
      else if (key == "Use_time_smoothing")
        j.at(key).get_to(commonSettings.useTimeSmoothing);
      else if (key == "Use_ab_filter")
        j.at(key).get_to(commonSettings.useAbFilter);

      else if (key == "Use_gps")
        j.at(key).get_to(commonSettings.useGps);
      else if (key == "Use_instant_gps_position")
        j.at(key).get_to(commonSettings.useInstantGpsPosition);
      else if (key == "Prefer_indoor_solution")
        j.at(key).get_to(commonSettings.preferIndoorSolution);
      else if (key == "Fuse_gps")
        j.at(key).get_to(commonSettings.fuseGps);
      else if (key == "Use_gps_outside_map")
        j.at(key).get_to(commonSettings.useGpsOutsideMap);
      else if (key == "Use_gps_sigma_filter")
        j.at(key).get_to(commonSettings.useGpsSigmaFilter);
      else if (key == "Sigma_filter_window")
        j.at(key).get_to(commonSettings.sigmaFilterWindow);
      else if (key == "Sigma_filter_delta")
        j.at(key).get_to(commonSettings.sigmaFilterDelta);
      else if (key == "Min_num_sats")
        j.at(key).get_to(commonSettings.minNumSats);

      else if (key == "Prior_deviation")
        j.at(key).get_to(commonSettings.priorDeviation);
      else if (key == "Gps_deviation")
        j.at(key).get_to(commonSettings.minGpsDeviation);
      else if (key == "Maximum_to_accept_gps_measurement")
        j.at(key).get_to(commonSettings.maxGpsDeviation);
      else if (key == "Fuse_gps_border_m")
        j.at(key).get_to(commonSettings.fuseGpsBorderM);
      else if (key == "Position_old_for_fusing_sec")
        j.at(key).get_to(commonSettings.positionIsTooOldSec);
      else if (key == "Gps_valid_time_window_sec")
        j.at(key).get_to(commonSettings.gpsValidTimeWindowSec);

      else if (key == "Use_calculated_azimuth")
        j.at(key).get_to(commonSettings.useCalculatedAzimuth);
      else if (key == "Device_azimuth_lifetime_seconds")
        j.at(key).get_to(commonSettings.deviceAzimuthLifetimeSeconds);
      else if (key == "Step_multiplier")
        j.at(key).get_to(commonSettings.stepMultiplier);

      else if (key == "Has_accelerometer")
        j.at(key).get_to(commonSettings.hasAccelerometer);
      else if (key == "Has_gyroscope")
        j.at(key).get_to(commonSettings.hasGyroscope);

      else if (key == "No_signal_time_threshold")
        j.at(key).get_to(commonSettings.noSignalTimeThreshold);
      else if (key == "No_action_time_threshold")
        j.at(key).get_to(commonSettings.noActionTimeThreshold);

      else if (key == "WiFi_RTT_Offset")
        j.at(key).get_to(commonSettings.wiFiRttOffset);
      else
        std::cerr << "unknown setting: " << key << '\n';
    }
  } 
  catch (json::exception& e) 
  {
    std::cerr << "parsing json " << j << " following exception occurred" << '\n'
              << "message: " << e.what() << '\n';
  }
}
  
static void from_json(const nlohmann::json& j, LevelSettings& levelSettings)
{
  std::vector<std::string> keys;
  for (auto& el : j.items())
  {
    keys.push_back(el.key());
  }
  try
  {
    for (const auto& key : keys)
    {
      if (key == "A")
        j.at(key).get_to(levelSettings.normalModeA);
      else if (key == "B")
        j.at(key).get_to(levelSettings.normalModeB);
      else
        std::cerr << "unknown setting: " << key << '\n';
    }
  }
  catch (json::exception& e) 
  {
    std::cerr << "parsing json " << j << " following exception occurred" << '\n'
              << "message: " << e.what() << '\n';
  }
}
} // nlohmann

std::vector<Measurement> GetNavMessages(const std::string& jsonFile)
{
  std::vector<Measurement> inputs;

  std::ifstream infile(jsonFile);
  if (!infile.good())
    std::cout << "GetNavMessages ERROR: CAN NOT READ FILE " << jsonFile << " !!!" << std::endl;

  std::string line;
  while (std::getline(infile, line))
  {
    auto j = nlohmann::json::parse(line);
    inputs.emplace_back(j.get<Measurement>());
  }
  return inputs;
}

double getGeoDist(const GeoPoint& p1, const GeoPoint& p2)
{
  static const double R = 6378000;
  double lat1Rad = M_PI * p1.latitude / 180.0;
  double lat2Rad = M_PI * p2.latitude / 180.0;
  double lng1Rad = M_PI * p1.longitude / 180.0;
  double lng2Rad = M_PI * p2.longitude / 180.0;
  double dlat = lat1Rad - lat2Rad;
  double dlng = lng1Rad - lng2Rad;
  double a = std::pow(std::sin(dlat / 2.0), 2)
             + std::cos(lat1Rad) * std::cos(lat2Rad) * std::pow(std::sin(dlng / 2.0), 2);
  double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
  double d = R * c;
  return d;
}

GeoPoint removeSame(std::vector<GeoPoint>& allPoints)
{
  assert(!allPoints.empty());
  GeoPoint p = allPoints.front();
  auto it = allPoints.begin();

  int counter = 0;
  while (it != allPoints.end())
  {
    if (getGeoDist(*it, p) < GRAPH_SAME_POINTS_GEO_DIST)
    {
      it = allPoints.erase(it);
      counter++;
    }
    else
      it++;
  }
  return p;
}

int getVertex(const GeoPoint& firstPoint, const std::unordered_map<int, GeoPoint>& vertexMap)
{
  for (const auto& it: vertexMap)
  {
    int v = it.first;
    GeoPoint p = it.second;
    if (getGeoDist(firstPoint, p) < GRAPH_SAME_POINTS_GEO_DIST)
      return v;
  }
  throw std::logic_error("Vertex was not found!");
  return -1;
}

std::pair<LevelId, Transmitter<GeoPoint3D>> parseTransmitter(nlohmann::json j)
{
  auto properties = j.at("properties");
  std::string levelId = properties.at("level");
  //TODO transmitters do not contain pathloss model in configuration file!

  double A = -80.0;
  double B = 3.0;
  if (properties.count("A"))
      A = properties.at("A");
  if (properties.count("B"))
      B = properties.at("B");

  PathlossModel pathlossModel{A, B, 0};
  std::string id = properties.at("uuid");

  TransmitterType type;
  auto transmitterTypeJson = properties.at("transmitter_type");
  transmitterTypeJson.get_to(type);
  std::vector<double> pointCoordinates;
  auto geometryJson = j.at("geometry");
  geometryJson.at("coordinates").get_to(pointCoordinates);
  //geojson has longitude:latitude format
  GeoPoint3D geopoint3D(pointCoordinates.at(1), pointCoordinates.at(0), 0.0);
  if (pointCoordinates.size() == 3)
    geopoint3D = GeoPoint3D(pointCoordinates.at(1), pointCoordinates.at(0), pointCoordinates.at(2));
  Transmitter<GeoPoint3D> transmitter(TransmitterId(id), geopoint3D, pathlossModel, type);
  return std::make_pair(LevelId(levelId), transmitter);
}

std::pair<LevelId, std::list<Polygon>> parseBarriers(const nlohmann::json& j)
{
  auto propertiesJson = j.at("properties");
  auto levelId = propertiesJson.at("level");
  auto geometryJson = j.at("geometry");
  auto barriersPolygonsJson = geometryJson.at("coordinates");
  std::list<Polygon> levelPolygons;
  for (const auto& polygonJson: barriersPolygonsJson)
  {
    Polygon barrier;
    for (const auto& pointJson: polygonJson[0])
    {
      std::vector<double> pointCoordinates;
      pointJson.get_to(pointCoordinates);
      //geojson has longitude:latitude format
      boost::geometry::append(barrier, Point(pointCoordinates.at(1), pointCoordinates.at(0)));
    }
    levelPolygons.emplace_back(barrier);
  }

  return std::make_pair(LevelId(levelId), levelPolygons);
}

std::pair<LevelId, std::list<Polygon>> parseLevel(const nlohmann::json& j)
{
  auto propertiesJson = j.at("properties");
  auto levelId = propertiesJson.at("level");
  auto geometryJson = j.at("geometry");
  auto allowedAreaPolygonsJson = geometryJson.at("coordinates");
  std::list<Polygon> levelPolygons;
  for (const auto& polygonJson: allowedAreaPolygonsJson)
  {
    int count = 0;
    Polygon allowedArea;
    allowedArea.inners().resize(polygonJson.size() - 1);
    for (const auto& circleJson: polygonJson)
    {
      for (const auto& pointJson: circleJson)
      {
        std::vector<double> pointCoordinates;
        pointJson.get_to(pointCoordinates);
        //geojson has longitude:latitude format
        if (count == 0)
          boost::geometry::append(allowedArea.outer(), Point(pointCoordinates.at(1), pointCoordinates.at(0)));
        else
          boost::geometry::append(allowedArea.inners()[count - 1], Point(pointCoordinates.at(1), pointCoordinates.at(0)));
      }
      count++;
    }
    levelPolygons.emplace_back(allowedArea);
  }

  return std::make_pair(LevelId(levelId), levelPolygons);
}

GeoLevels ParseGeojson(const std::string& jsonFile, int& errorCode)
{
  errorCode = 0;
  std::ifstream is(jsonFile);
  nlohmann::json j = nlohmann::json::parse(is);

  std::map<LevelId, std::list<Polygon>> levels;
  auto features = j.at("features");
  for (const auto& feature: features)
  {
    auto featureProperties = feature.at("properties");
    std::string featureType = featureProperties.at("type");
    if (featureType == "level")
    {
      std::pair<LevelId, std::list<Polygon>> levelPair = parseLevel(feature);
      levels.insert(levelPair);
    }
  }

  std::map<LevelId, std::list<Polygon>> barriers;
  std::map<LevelId, Geo3DTransmitters> transmitters;

  for (const auto& feature: features)
  {
    auto featureProperties = feature.at("properties");
    std::string featureType = featureProperties.at("type");

    if (featureType == "transmitter")
    {
      std::pair<LevelId, Transmitter<GeoPoint3D>> transmitterPair = parseTransmitter(feature);
      if (transmitters.find(transmitterPair.first) == transmitters.end())
        transmitters[transmitterPair.first] = std::vector<Transmitter<GeoPoint3D>>();
      transmitters[transmitterPair.first].push_back(transmitterPair.second);
    }
  }

  std::vector<std::shared_ptr<GeoLevel> > vLevels;
  for (const auto& entry : levels)
  {
    auto geolevel = std::make_shared<GeoLevel>();
    geolevel->id.value = entry.first.value;
    geolevel->altitude = 100.0; //TODO

    geolevel->geometry = getGeometry(entry.second);

    if (transmitters.find(entry.first) != transmitters.end())
      geolevel->transmitters = transmitters.at(entry.first);

    vLevels.push_back(std::move(geolevel));
  }

  return vLevels;
}

NavigationSettings CreateSettingsFromJson(
  const std::string& jsonFile,
  int& errorCode)
{
  NavigationSettings navigationSettings;

  errorCode = 0;
  std::ifstream is(jsonFile);
  if (!is.good())
  {
    std::cout << "CreateSettingsFromJson ERROR: CAN NOT READ FILE " << jsonFile << " !!!" << std::endl;
    errorCode = ERROR_OPEN_FILE;
    return navigationSettings;
  }

  nlohmann::json j = nlohmann::json::parse(is);
  auto levelsSettingsJson = j.at("level_settings");
  for (const auto& levelSettingsJson: levelsSettingsJson)
  {
    std::string levelId = levelSettingsJson.at("level");
    navigationSettings.levelsSettings[LevelId(levelId)] = levelSettingsJson.at("settings")[0].get<LevelSettings>();
  }
  navigationSettings.commonSettings = j.at("common_settings")[0].get<CommonSettings>();

  return navigationSettings;
}
