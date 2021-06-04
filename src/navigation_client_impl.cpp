#include <iostream>
#include <navigation_settings.h>
#include "navigation_client_impl.h"
#include "position_estimator/position_estimator_zone.h"
#include "level_estimator/level_estimator_transmitters.h"

namespace navigine {
namespace navigation_core {

NavigationClientImpl::NavigationClientImpl(
    const std::shared_ptr<LevelCollector>& levelCollector,
    const NavigationSettings& navProps)  
  : mLevelIndex(levelCollector)
  , mLevelEstimator(createLevelEstimator(levelCollector, navProps))
  , mPositionEstimatorIndoor(createPostitionEstimator(levelCollector, navProps))
  , mMsrPreprocessor(std::make_unique<MeasurementsPreprocessor>(levelCollector, navProps))
  , mFlagIndoorPos(false)
{ }

std::vector<NavigationOutput> NavigationClientImpl::navigate(const std::vector<Measurement>& navBatchInput)
{
  std::vector<NavigationOutput> navBatchOutput;
  if (navBatchInput.size() == 0)
    return navBatchOutput;

  long long prevTs = navBatchInput[0].ts;
  for (const auto &navInput: navBatchInput)
  {
    if (prevTs > navInput.ts)
    {
      std::cerr << "prevTs: " << prevTs << ", t.ts: " << navInput.ts << std::endl;
      continue;
    }
    else
    {
      prevTs = navInput.ts;
    }

    NavigationStatus retStatus = NavigationStatus::OK;

    mMsrPreprocessor->updateMeasurements(navInput);
    const long long curTs = mMsrPreprocessor->getCurrentTs();
    const long long lastSignalTs = mMsrPreprocessor->getLastSignalTs();
    const SensorMeasurement sensorMsr = mMsrPreprocessor->getValidSensor();
    const auto radioMsr = mMsrPreprocessor->extractRadioMeasurements();
    LevelId levelId = mLevelEstimator->calculateLevel(radioMsr, sensorMsr);

    if (mLevelIndex->hasLevel(levelId))
    {
      const Level& level = mLevelIndex->level(levelId);
      mPosition = mPositionEstimatorIndoor->calculatePosition(level, curTs, radioMsr, retStatus);
    }
    else
    {
      retStatus = NavigationStatus::NO_LEVEL;
    }

    // Filling navOutput
    NavigationOutput navOutput;
    navOutput.status = retStatus;

    // TODO: optimize NavClient state variables (mPosition, mRetStatus, etc.)
    // https://redmine.navigine.com/issues/2554
    if (mPosition.isEmpty)
    {
      if (retStatus == NavigationStatus::OK)
      {
        navOutput.status = NavigationStatus::NAVIGATION_ERROR;
        //TODO ret rid of empty position status
      }
      navBatchOutput.emplace_back(navOutput);
      continue;
    }
    else
    {
      navOutput.status = NavigationStatus::OK;
    }

    navOutput.posLevel = mPosition.levelId;

    if (navOutput.posLevel.isValid())
    {
      const Level& level = mLevelIndex->level(navOutput.posLevel);
      const GeoPoint gpsPosition = localToGps(XYPoint(mPosition.x, mPosition.y), level.bindingPoint());
      navOutput.posLatitude = gpsPosition.latitude;
      navOutput.posLongitude = gpsPosition.longitude;
    }

    navOutput.posR = mPosition.deviationM;
    navOutput.provider = mPosition.provider;
    navBatchOutput.emplace_back(navOutput);
  }
  return navBatchOutput;
}

std::unique_ptr<LevelEstimator> NavigationClientImpl::createLevelEstimator(
  const std::shared_ptr<navigine::navigation_core::LevelCollector>& levelCollector,
  const NavigationSettings& navProps)
{
  return std::make_unique<LevelEstimatorTransmitters>(levelCollector, navProps);
}

std::unique_ptr<PositionEstimator> NavigationClientImpl::createPostitionEstimator(
  const std::shared_ptr<navigine::navigation_core::LevelCollector>& levelCollector,
  const NavigationSettings& navProps)
{
  return std::make_unique<PositionEstimatorZone>(levelCollector, navProps);
}

std::shared_ptr<NavigationClient> createNavigationClient(
  const std::shared_ptr<navigine::navigation_core::LevelCollector>& levelCollector,
  const NavigationSettings& navProps)
{
  return std::make_shared<NavigationClientImpl>(levelCollector, navProps);
}

} } // namespace navigine::navigation_core
