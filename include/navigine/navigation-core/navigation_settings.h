#pragma once

#include <navigine/navigation-core/serialization/serialization.h>
#include <navigine/navigation-core/utils.h>

namespace navigine::navigation_core {

struct ProviderSelectorSettings
{
    static constexpr auto DEFAULT_CURRENT_INDEX_HANDICAP = 1.0;
    static constexpr auto DEFAULT_UPDATE_INTERVAL = 1.0;
    static constexpr auto DEFAULT_WINDOW_COUNT = 5U;

    std::optional<double> currentIndexHandicap;
    std::optional<double> updateInterval;       // Interval between subsequent provider updates (unit: sec)
    std::optional<unsigned int> windowCount;    // Number of windows taken into consideration
};

struct PositionEstimatorSettings
{
    struct Common
    {
        enum class UseAlgorithm {
            PF_PDR,
            PF_ADR,
            PF_PDR_KALMAN,
            PF_ADR_KALMAN,
            PF_GRAPH,
            LSM,
            LSM_PDR_KALMAN,
            LSM_ADR_KALMAN,
            ZONES,
            OUTDOOR,
            OUTDOOR_PDR_KALMAN,
            OUTDOOR_ADR_KALMAN,
            OUTDOOR_LC_KALMAN};

        static constexpr auto DEFAULT_CORRECTION_OFFSET_RSSI = 0.0;
        static constexpr auto DEFAULT_CORRECTION_OFFSET_RTT = 0.0;
        static constexpr auto DEFAULT_CUTOFF_RSSI = -100.0;
        static constexpr auto DEFAULT_DEAD_RECKONING_TIME_THRESHOLD = 35.0;
        static constexpr auto DEFAULT_USE_ALGORITHM = UseAlgorithm::PF_PDR;
        static constexpr auto DEFAULT_USE_GPS = false;
        static constexpr auto DEFAULT_USE_BLE = true;
        static constexpr auto DEFAULT_USE_WIFI = true;
        static constexpr auto DEFAULT_USE_WIFI_RTT = true;
        static constexpr auto DEFAULT_USE_TX_POWER = true;
        static constexpr auto DEFAULT_USE_TX_RSSI_THRESHOLD = true;

        std::optional<double> correctionOffsetRssi;     // Unit: dBm
        std::optional<double> correctionOffsetRtt;      // Unit: meters
        std::optional<double> cutOffRssi;
        std::optional<double> deadReckoningTimeThreshold;
        std::optional<UseAlgorithm> useAlgorithm;
        std::optional<bool> useGps;
        std::optional<bool> useBle;
        std::optional<bool> useWifi;
        std::optional<bool> useWifiRtt;
        std::optional<bool> useTxRssiThreshold;
    };

    struct MotionModel
    {
        static constexpr auto DEFAULT_USE_ACCELEROMETER = true;
        static constexpr auto DEFAULT_USE_GYROSCOPE = true;
        static constexpr auto DEFAULT_USE_MAGNETOMETER = true;
        static constexpr auto DEFAULT_USE_ODOMETER = true;
        static constexpr auto DEFAULT_USE_STOP_DETECTION = true;
        static constexpr auto DEFAULT_USE_SPEED_CONSTRAINT = true;
        static constexpr auto DEFAULT_GYROSCOPE_NOISE_DEVIATION = 4.0;
        static constexpr auto DEFAULT_MAGNETOMETER_NOISE_DEVIATION = 90.0;
        static constexpr auto DEFAULT_ODOMETER_NOISE_DEVIATION = 0.139; // 0.5 km/h
        static constexpr auto DEFAULT_STEP_NOISE_DEVIATION = 0.4;
        static constexpr auto DEFAULT_STOP_DETECTION_TIME = 3.0;
        static constexpr auto DEFAULT_SPEED_CONSTRAINT_HORIZONTAL = 2.0;
        static constexpr auto DEFAULT_SPEED_CONSTRAINT_VERTICAL = 0.0;
        static constexpr auto DEFAULT_MOTION_SPEED = 1.0;

        std::optional<bool> useAccelerometer;
        std::optional<bool> useGyroscope;
        std::optional<bool> useMagnetometer;
        std::optional<bool> useOdometer;
        std::optional<bool> useStopDetection;
        std::optional<bool> useSpeedConstraint;
        std::optional<double> gyroscopeNoiseDeviation;      // Unit: deg/s
        std::optional<double> magnetometerNoiseDeviation;   // Unit: deg
        std::optional<double> odometerNoiseDeviation;       // Unit: m/s
        std::optional<double> stepNoiseDeviation;           // Unit: meters
        std::optional<double> stopDetectionTime;            // Unit: sec
        std::optional<double> speedConstraintHorizontal;    // Unit: m/s
        std::optional<double> speedConstraintVertical;      // Unit: m/s
        std::optional<double> motionSpeed;                  // Unit: m/s
    };

    struct ParticleFilter
    {
        static constexpr auto DEFAULT_USE_HEADING_CORRECTION = false;
        static constexpr auto DEFAULT_CORRECTION_NOISE_RSSI = 3.0;
        static constexpr auto DEFAULT_CORRECTION_NOISE_RTT = 3.0;
        static constexpr auto DEFAULT_CORRECTION_NOISE_GPS = 10.0;
        static constexpr auto DEFAULT_MIN_DEVIATION = 1.0;
        static constexpr auto DEFAULT_MAX_DISTANCE_THRESHOLD_RSSI = 40.0;
        static constexpr auto DEFAULT_MAX_DISTANCE_THRESHOLD_RTT = 100.0;
        static constexpr auto DEFAULT_MIN_MSR_NUM_FOR_UPDATE = 1U;
        static constexpr auto DEFAULT_MIN_MSR_NUM_FOR_MUTATION = 3U;
        static constexpr auto DEFAULT_NUM_CLOSEST_TXS = 5U;
        static constexpr auto DEFAULT_NUM_PARTICLES = 1000U;
        static constexpr auto DEFAULT_NUM_PARTICLES_TO_MUTATE_RATE = 0.05;
        static constexpr auto DEFAULT_NUM_PARTICLES_AROUND_CLOSEST_TXS_RATE = 0.01;
        static constexpr auto DEFAULT_NUM_PARTICLES_ALIVE_THRESHOLD_RATE = 0.1;
        static constexpr auto DEFAULT_RESAMPLING_THRESHOLD_RATE = 0.6;
        static constexpr auto DEFAULT_SIG_AVERAGING_TIME = 1.0;
        static constexpr auto DEFAULT_UPDATE_INTERVAL = 1.0;
        static constexpr auto DEFAULT_WALL_PASSING_PROBABILITY = 0.0;

        std::optional<bool> useHeadingCorrection;
        std::optional<double> correctionNoiseRssi;      // Unit: dimensionless
        std::optional<double> correctionNoiseRtt;       // Unit: dimensionless
        std::optional<double> correctionNoiseGps;       // Unit: dimensionless
        std::optional<double> maxDistanceThresholdRssi; // Unit: meters
        std::optional<double> maxDistanceThresholdRtt;  // Unit: meters
        std::optional<double> minDeviation;             // Unit: meters
        std::optional<unsigned int> minMsrNumForUpdate;
        std::optional<unsigned int> minMsrNumForMutation;
        std::optional<unsigned int> numClosestTxs;
        std::optional<unsigned int> numParticles;
        std::optional<double> numParticlesToMutateRate;
        std::optional<double> numParticlesAroundClosestTxsRate;
        std::optional<double> numParticlesAliveThresholdRate;
        std::optional<double> resamplingThresholdRate;
        std::optional<double> sigAveragingTime;         // Unit: sec
        std::optional<double> updateInterval;           // Unit: sec
        std::optional<double> wallPassingProbability;   // Unit: probability in interval [0,1]
    };

    struct LsmSolver
    {
        static constexpr auto DEFAULT_MAX_DEVIATION_FOR_UPDATE = 3.0;
        static constexpr auto DEFAULT_MIN_DEVIATION = 1.0;
        static constexpr auto DEFAULT_MIN_MSR_NUM_FOR_UPDATE = 3U;
        static constexpr auto DEFAULT_SIG_AVERAGING_TIME = 1.0;
        static constexpr auto DEFAULT_UPDATE_INTERVAL = 1.0;

        std::optional<double> maxDeviationForUpdate;        // Unit: meters
        std::optional<double> minDeviation;                 // Unit: meters
        std::optional<unsigned int> minMsrNumForUpdate;
        std::optional<double> sigAveragingTime;             // Unit: sec
        std::optional<double> updateInterval;               // Unit: sec
    };

    struct ZoneSolver
    {
        static constexpr auto DEFAULT_MIN_DEVIATION = 1.0;
        static constexpr auto DEFAULT_SIG_AVERAGING_TIME = 1.0;
        static constexpr auto DEFAULT_UPDATE_INTERVAL = 1.0;

        std::optional<double> minDeviation;         // Unit: meters
        std::optional<double> sigAveragingTime;     // Unit: sec
        std::optional<double> updateInterval;       // Unit: sec
    };

    struct Outdoor
    {
        static constexpr auto DEFAULT_GPS_MIN_DEVIATION = 5.0;
        static constexpr auto DEFAULT_GPS_MAX_DEVIATION_THRESHOLD = 1000.0;
        static constexpr auto DEFAULT_GPS_ZONE_FILTER_BUFFER = 10.0;
        static constexpr auto DEFAULT_GPS_MIN_NUM_SATS_THRESHOLD = 20U;
        static constexpr auto DEFAULT_SIGMA_FILTER_WINDOW = 5U;
        static constexpr auto DEFAULT_SIGMA_FILTER_DELTA = 3.0;
        static constexpr auto DEFAULT_USE_GPS_SIGMA_FILTER = false;
        static constexpr auto DEFAULT_USE_GPS_ZONE_FILTER = false;

        std::optional<double> gpsMinDeviation;
        std::optional<double> gpsMaxDeviationThreshold;
        std::optional<double> gpsZoneFilterBuffer;
        std::optional<unsigned int> gpsMinNumSatsThreshold;
        std::optional<unsigned int> sigmaFilterWindow;
        std::optional<double> sigmaFilterDelta;
        std::optional<bool> useGpsSigmaFilter;
        std::optional<bool> useGpsZoneFilter;
    };

    struct KalmanFilter
    {
        static constexpr auto DEFAULT_ACCURACY_HORIZONTAL_FACTOR = 1.0;
        static constexpr auto DEFAULT_ACCURACY_VERTICAL_FACTOR = 1.7;
        static constexpr auto DEFAULT_ACCURACY_ODOMETER_FACTOR = 1.0;
        static constexpr auto DEFAULT_ACCURACY_VELOCITY_FACTOR = 1.0;
        static constexpr auto DEFAULT_ACCURACY_TRACK_ANGLE_FACTOR = 1.0;
        static constexpr auto DEFAULT_ACCELEROMETER_NOISE_PSD_DB = -50.0;
        static constexpr auto DEFAULT_ACCELEROMETER_BIAS_PSD_DB = -50.0;
        static constexpr auto DEFAULT_GYROSCOPE_NOISE_PSD_DB = -100.0;
        static constexpr auto DEFAULT_GYROSCOPE_BIAS_PSD_DB = -100.0;
        static constexpr auto DEFAULT_ALIGNMENT_INTERVAL = 1.0;
        static constexpr auto DEFAULT_CORRECTION_INTERVAL_GPS = 1.0;
        static constexpr auto DEFAULT_CORRECTION_INTERVAL_ODO = 1.0;
        static constexpr auto DEFAULT_UPDATE_INTERVAL = 0.01;
        static constexpr auto DEFAULT_USE_BIAS_ACCELEROMETER = true;
        static constexpr auto DEFAULT_USE_BIAS_GYROSCOPE = true;
        static constexpr auto DEFAULT_USE_CORRECTION_GPS = true;
        static constexpr auto DEFAULT_USE_CORRECTION_ODO = true;
        static constexpr auto DEFAULT_USE_CORRECTION_TRACK_ANGLE = true;

        std::optional<double> accuracyHorizontalFactor;     // Unit: dimensionless
        std::optional<double> accuracyVerticalFactor;       // Unit: dimensionless
        std::optional<double> accuracyOdometerFactor;       // Unit: dimensionless
        std::optional<double> accuracyVelocityFactor;       // Unit: dimensionless
        std::optional<double> accuracyTrackAngleFactor;     // Unit: dimensionless
        std::optional<double> accelerometerNoisePsdDb;      // Unit: Db (aka 10 * log10(psd in m^2/sec^3))
        std::optional<double> accelerometerBiasPsdDb;       // Unit: Db (aka 10 * log10(psd in m^2/sec^5))
        std::optional<double> gyroscopeNoisePsdDb;          // Unit: Db (aka 10 * log10(psd in rad^2/sec))
        std::optional<double> gyroscopeBiasPsdDb;           // Unit: Db (aka 10 * log10(psd in rad^2/sec^3))
        std::optional<double> alignmentInterval;            // Unit: sec
        std::optional<double> correctionIntervalGps;        // Unit: sec
        std::optional<double> correctionIntervalOdo;        // Unit: sec
        std::optional<double> updateInterval;               // Unit: sec
        std::optional<bool> useBiasAccelerometer;
        std::optional<bool> useBiasGyroscope;
        std::optional<bool> useCorrectionGps;
        std::optional<bool> useCorrectionOdo;
        std::optional<bool> useCorrectionTrackAngle;
    };

    struct Postprocessor
    {
        static constexpr auto DEFAULT_MAX_PROJECTION_DISTANCE = 5.0;
        static constexpr auto DEFAULT_MAX_PROJECTION_SPEED = 0.0;
        static constexpr auto DEFAULT_SMOOTHING_COEFFICIENT = 0.5;
        static constexpr auto DEFAULT_USE_ACCURACY_FOR_SMOOTHING = false;
        static constexpr auto DEFAULT_USE_GRAPH_PROJECTION = false;
        static constexpr auto DEFAULT_USE_PROJECTION_TO_WHITE_AREA = false;
        static constexpr auto DEFAULT_USE_SMOOTHING = true;

        std::optional<double> maxProjectionDistance;
        std::optional<double> maxProjectionSpeed;
        std::optional<double> smoothingCoefficient;
        std::optional<bool> useAccuracyForSmoothing;
        std::optional<bool> useGraphProjection;
        std::optional<bool> useProjectionToWhiteArea;
        std::optional<bool> useSmoothing;
    };

    struct LevelEvaluation
    {
        static constexpr auto DEFAULT_NUM_CLOSEST_TXS = 3U;
        static constexpr auto DEFAULT_RSSI_TX_BONUS = 5.0;
        static constexpr auto DEFAULT_SIG_AVERAGING_TIME = 10.0;
        static constexpr auto DEFAULT_UPDATE_INTERVAL = 1.0;
        static constexpr auto DEFAULT_WEIGHT_RSSI = 0.20;
        static constexpr auto DEFAULT_WEIGHT_GPS = 0.05;
        static constexpr auto DEFAULT_WEIGHT_EXTERNAL = 0.75;

        std::optional<unsigned int> numClosestTxs;      // Maximum number of transmitters taken into consideration within the evaluation window
        std::optional<double> rssiTxBonus;              // Bonus for every transmitter within evaluation window (dBm)
        std::optional<double> sigAveragingTime;         // Unit: sec
        std::optional<double> updateInterval;           // Unit: sec
        std::optional<double> weightRssi;               // Unit: [0,1]
        std::optional<double> weightGps;                // Unit: [0,1]
        std::optional<double> weightExternal;           // Unit: [0,1]
    };

    Common common;
    ParticleFilter particleFilter;
    LsmSolver lsmSolver;
    ZoneSolver zoneSolver;
    Outdoor outdoor;
    MotionModel motionModel;
    KalmanFilter kalmanFilter;
    Postprocessor postprocessor;
    LevelEvaluation levelEvaluation;
};

struct NavigationSettings
{
    ProviderSelectorSettings providerSelector;
    PositionEstimatorSettings defaultPositionEstimator;
    std::map<LevelId, PositionEstimatorSettings> positionEstimators;
};

// Navigation settings raw data representation.
struct NavigationSettingsData
{
    serialization::KVStorage common;
    std::map<LevelId, serialization::KVStorage> levels;
};

// Parsing navigation settings from raw data.
NavigationSettings parseNavigationSettingsFromRawData(
    const NavigationSettingsData& settingsData,
    std::vector<std::string>& errors);

// Parsing navigation settings from JSON data.
NavigationSettings parseNavigationSettingsFromJsonData(
    const std::string& data,
    std::vector<std::string>& errors);

// Parsing navigation settings from JSON file.
NavigationSettings parseNavigationSettingsFromJsonFile(
    const std::string& filename,
    std::vector<std::string>& errors);

// Dump navigation settings to raw data.
NavigationSettingsData dumpNavigationSettingsToRawData(
    const NavigationSettings& settings);

std::string dumpNavigationSettingsRawDataToJson(
    const NavigationSettingsData& settings,
    const int indent = -1);

// Dump navigation settings to JSON data.
std::string dumpNavigationSettingsToJsonData(
    const NavigationSettings& settings,
    const int indent = -1);

PositionEstimatorSettings getPositionEstimatorSettings(
    const NavigationSettings& settings,
    const LevelId& levelId);

} // namespace navigine::navigation_core
