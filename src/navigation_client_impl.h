#pragma once

#include <navigine/navigation-core/navigation_client.h>
#include <navigine/navigation-core/navigation_input.h>
#include <navigine/navigation-core/navigation_output.h>
#include <navigine/navigation-core/navigation_state.h>
#include <navigine/navigation-core/navigation_settings.h>
#include <navigine/navigation-core/level_collector.h>

#include "measurements/measurement_preprocessor.h"
#include "position_estimator/position_estimator.h"
#include "position_estimator/position_estimator_outdoor.h"
#include "position_postprocessor/position_postprocessor.h"
#include "sensor_fusion/sensor_fusion.h"
#include "level_estimator/level_estimator.h"

namespace navigine {
namespace navigation_core {

class NavigationClientImpl : public NavigationClient
{
  public:
    NavigationClientImpl(
      const std::shared_ptr<navigine::navigation_core::LevelCollector>& levelCollector,
      const NavigationSettings& navProps);

    // Return navigation state of this client
    std::vector<NavigationState> getStates() const override;

    // Main navigation function. Calculates current position based on incoming measurements
    std::vector<NavigationOutput> navigate(const std::vector<Measurement>& navBatchInput) override;

  private:
    NavigationClientImpl(const NavigationClientImpl&) = delete;
    void operator=(const NavigationClientImpl&) = delete;

    std::unique_ptr<LevelEstimator> createLevelEstimator(
      const std::shared_ptr<LevelCollector>& levelCollector,
      const NavigationSettings& navProps);

    std::unique_ptr<PositionEstimator> createPostitionEstimator(
      const std::shared_ptr<LevelCollector>& levelCollector,
      const NavigationSettings& navProps);

  private:
    const bool mUseEnuAzimuth;
    const long long mNoSignalTimeThreshold;
    const long long mNoActionTimeThreshold;
    std::shared_ptr<LevelCollector> mLevelIndex;
    std::unique_ptr<LevelEstimator> mLevelEstimator;
    std::unique_ptr<PositionEstimator> mPositionEstimatorIndoor;
    std::unique_ptr<MeasurementsPreprocessor> mMsrPreprocessor;
    std::unique_ptr<PositionEstimatorOutdoor> mPositionEstimatorOutdoor;
    std::unique_ptr<PositionPostprocessor> mPositionPostprocessor;
    std::unique_ptr<SensorFusion> mSensorFusion;
    
    std::vector<NavigationState> mNavigationStates = {};
    Position mPosition = {};
    bool mFlagIndoorPos;
    boost::optional<double> mPrevFusedPosHeading;
    boost::optional<double> mPrevGyroHeading;
    const bool mUseAltitude;
};

} } // namespace navigine::navigation_core
