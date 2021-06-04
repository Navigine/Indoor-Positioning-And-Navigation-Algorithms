#pragma once

#include <navigation_client.h>
#include <navigation_input.h>
#include <navigation_output.h>
#include <navigation_settings.h>
#include <level_collector.h>

#include "measurements/measurement_preprocessor.h"
#include "position_estimator/position_estimator.h"
#include "level_estimator/level_estimator.h"

namespace navigine {
namespace navigation_core {

class NavigationClientImpl : public NavigationClient
{
  public:
    NavigationClientImpl(
      const std::shared_ptr<navigine::navigation_core::LevelCollector>& levelCollector,
      const NavigationSettings& navProps);

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
    std::shared_ptr<LevelCollector> mLevelIndex;
    std::unique_ptr<LevelEstimator> mLevelEstimator;
    std::unique_ptr<PositionEstimator> mPositionEstimatorIndoor;
    std::unique_ptr<MeasurementsPreprocessor> mMsrPreprocessor;

    Position mPosition = {};
    bool mFlagIndoorPos;
};

} } // namespace navigine::navigation_core
