#ifndef NAVIGINE_NAVIGATION_SETTINGS_H
#define NAVIGINE_NAVIGATION_SETTINGS_H

#include <map>
#include <vector>
#include <memory>
#include <sstream>

#include "level.h"

namespace navigine {
namespace navigation_core {

struct CommonSettings {
    enum class UseAlgorithm { PF_PDR, PF_NOPDR, ZONES, GRAPH, KNN }; //PF_PDR, PF_NOPDR, ZONES, GRAPH, KNN
    enum class MeasurementType { RSSI, DISTANCE_V1, DISTANCE_V2 }; // rssi, distance_21, distance_v2
    enum class SignalsToUse { BLE, WIFI, BOTH }; //ble, wifi, both

    UseAlgorithm useAlgorithm       = UseAlgorithm::PF_PDR; // "Use_algorithm"
    double      lklCorrectionNoise  = 10.0;          // "Correction_noise"
    bool        useEnuAzimuth       = false;         // "Use_enu_azimuth"
    bool        useDifferentionMode = false;         // "Differention_mode"
    bool        useTxPower          = false;         // "Use_beacon_power"
    bool        useRadiomap         = false;         // "Use_radiomap"
    bool        useStops            = false;         // "Use_stops"
    bool        useTracking         = false;         // "Use_tracking"
    double      stopDetectionTime   = 3.0;           // "Stop_detection_time"
    double      minDeviationM       = 1.0;           // "Min_deviation_m"
    double      averageMovSpeed     = 1.0;           // "Average_speed"
    MeasurementType measurementType = MeasurementType::RSSI; // "Measurement_type"

    bool useBestRefPointLevel = false;              // "Use_best_ref_point_level"
    bool useBarometer         = false;              // "Use_barometer"

    bool   useUnknownTransmitters = false;            // "Use_unknown_beacons"
    double sigCutOffRss           = -100;             // "Cutoff_rss"
    bool   useClosestAps          = false;            // "Use_closest_aps"
    unsigned int numClosestAps    = 1;                // "N_closest_aps"
    double sigAveragingTime       = 1.0;              // "Sig_averaging_time"
    double sigWindowShift         = 0.0;              // "Sig_window_shift"
    SignalsToUse signalsToUse     = SignalsToUse::BLE; // "Use_Signals"

    unsigned int numParticles             = 1000;    // "N_particles_location_change"
    unsigned int minMsrNumForMutation     = 3;       // "Min_msr_num_for_mutation"
    double resamplingThreshold            = 600.0;   // "Resampling_threshold_num", Level
    unsigned int minNumParticlesAlive     = 100;     // "Min_num_of_particles", Level
    double gyroNoiseDegree                = 4.0;     // "Gyro_noise_degree"
    double stepNoiseDeviationMeters       = 0.4;     // "Step_noise_deviation_meters"
    bool   useProjectionToWhiteArea       = false;   // "Use_projection_to_white_area"

    unsigned int numParticlesAroundClosestAp    = 10;      // "N_particles_around_closest_AP"
    unsigned int numParticlesToMutate           = 50;      // "N_particles_to_mutate"
    unsigned int numSamplingAttempts            = 60;      // "Num_sampling_attempts"
    unsigned int numInitializationAttempts      = 500;     // "Num_initialization_attempts"
    bool   useUniformSampling                   = false;   // "Use_uniform_sampling"

    unsigned int kNeighbors                        = 8;      // "K_neighbors"
    bool   useTriangles                               = true;   // "Use_triangles"
    bool   useDiffMode                                = true;   // "Use_diff_mode"
    unsigned int minNumOfMeasurementsForPositionCalculation = 4;      // "Min_num_of_measurements_for_position_calculation"

    double smoothing                           = 0.5;    // "Smoothing"
    double deadReckoningTime                   = 5.0;    // "Dead_reckon_time"
    double stopUpdateTime                      = 4.0;    // "Stop_update_time"
    double graphProjDist                       = 5.0;    // "Graph_prog_distance"
    double useStopsDistanceThresholdM          = 10.0;   // "Stopped_distance_threshold_m"
                                                                      // if distance between stopped position and current position
                                                                      // is more than the threshold - update stopped position

    bool   useSmoothing                        = true;    // "Use_smoothing"
    bool   useGraphProjection                  = false;   // "Use_graph_projection"
    bool   useAccuracyForSmoothing             = false;   // "Use_accuracy_for_smoothing"
    bool   useSpeedConstraint                  = true;    // "Use_speed_constraint"
    bool   useBarriers                         = false;   // "Use_barriers"
    bool   useTimeSmoothing                    = false;   // "Use_time_smoothing"
    bool   useAbFilter                         = true;    // "Use_ab_filter"

    bool   useAltitude                         = false;  // "Use_altitude"
    bool   useGps                              = false;  // "Use_gps"
    bool   useInstantGpsPosition               = false;  // "Use_instant_gps_position"
    bool   preferIndoorSolution                = false;  // "Prefer_indoor_solution"
    bool   fuseGps                             = false;  // "Fuse_gps"
    bool   useGpsOutsideMap                    = false;  // "Use_gps_outside_map"
    bool   useGpsSigmaFilter                   = false;  // "Use_gps_sigma_filter"
    unsigned int sigmaFilterWindow             = 1;      // "Sigma_filter_window"
    double sigmaFilterDelta                    = 0.0;    // "Sigma_filter_delta"
    unsigned int minNumSats                    = 20;     // "Min_num_sats"

    double priorDeviation                      = 50.0;   // "Prior_deviation"
    double minGpsDeviation                     = 5.0;    // "Gps_deviation"
    double maxGpsDeviation                     = std::numeric_limits<double>::infinity(); // Maximum_to_accept_gps_measurement"
    double fuseGpsBorderM                      = 10;     // "Fuse_gps_border"
    long long noSignalTimeThreshold            = 35;     // "No_signal_time_threshold"
    long long noActionTimeThreshold            = 20;     // "No_action_time_threshold"
    double positionIsTooOldSec                 = 20.0;   // "Position_old_for_fusing_sec"
    double gpsValidTimeWindowSec               = 1.0;    // "Gps_valid_time_window_sec"

    bool   useCalculatedAzimuth                = false; // "Use_calculated_azimuth"
    double deviceAzimuthLifetimeSeconds        = 10.0;  // "Device_azimuth_lifetime_seconds"
    double stepMultiplier                      = 1.0;   // "Step_multiplier"

    bool hasAccelerometer = true;                      // "Has_accelerometer"
    bool hasGyroscope = true;                          // "Has_gyroscope"

    double wiFiRttOffset = -1.5;                     // "WiFi_RTT_Offset"
};

struct LevelSettings {
    double normalModeA = -82.0;                      // "GP_Normal_Mode", "A", Level
    double normalModeB =  3.0;                       // "GP_Normal_Mode", "B", Level
};

struct NavigationSettings
{
    CommonSettings commonSettings;
    std::map<LevelId, LevelSettings> levelsSettings;
};

} } // namespace navigine::navigation_core

#endif // NAVIGINE_NAVIGATION_SETTINGS_H
