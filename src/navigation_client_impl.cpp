#include <iostream>
#include <navigine/navigation-core/navigation_settings.h>
#include "navigation_client_impl.h"
#include "position_estimator/position_estimator_outdoor.h"
#include "position_estimator/position_estimator_zone.h"
#include "position_estimator/position_estimator_knn.h"
#include "level_estimator/level_estimator_radiomap.h"
#include "level_estimator/level_estimator_transmitters.h"
#include "trilateration.h"

namespace navigine {
namespace navigation_core {

NavigationClientImpl::NavigationClientImpl(
    const std::shared_ptr<LevelCollector>& levelCollector,
    const NavigationSettings& navProps)  
  : mUseEnuAzimuth(navProps.commonSettings.useEnuAzimuth)
  , mNoSignalTimeThreshold((long long)(1000 * navProps.commonSettings.noSignalTimeThreshold))
  , mNoActionTimeThreshold((long long)(1000 * navProps.commonSettings.noActionTimeThreshold))
  , mLevelIndex(levelCollector)
  , mLevelEstimator(createLevelEstimator(levelCollector, navProps))
  , mPositionEstimatorIndoor(createPostitionEstimator(levelCollector, navProps))
  , mMsrPreprocessor(std::make_unique<MeasurementsPreprocessor>(levelCollector, navProps))
  , mPositionEstimatorOutdoor(std::make_unique<PositionEstimatorOutdoor>(levelCollector, navProps))
  , mPositionPostprocessor(std::make_unique<PositionPostprocessor>(navProps))
  , mSensorFusion(std::make_unique<SensorFusion>(navProps))
  , mFlagIndoorPos(false)
  , mPrevFusedPosHeading(boost::none)
  , mPrevGyroHeading(boost::none)
  , mUseAltitude(navProps.commonSettings.useAltitude)
{ }

std::vector<NavigationOutput> NavigationClientImpl::navigate(const std::vector<Measurement>& navBatchInput)
{
  std::vector<NavigationOutput> navBatchOutput;
  if (navBatchInput.size() == 0)
    return navBatchOutput;

  mNavigationStates.clear();
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
    const NmeaMeasurement nmeaMsr = mMsrPreprocessor->getCurrentNmea();
    MotionInfo motionInfo = mSensorFusion->calculateDisplacement(sensorMsr, curTs);
    const auto radioMsr = mMsrPreprocessor->extractRadioMeasurements(motionInfo.lastMotionTs);

    LevelId levelId = mLevelEstimator->calculateLevel(radioMsr, sensorMsr);
    const Position outdoorPos = mPositionEstimatorOutdoor->calculatePosition(curTs, sensorMsr, nmeaMsr, mFlagIndoorPos);
    mFlagIndoorPos = false; // TODO refactoring needed

    if (curTs - lastSignalTs >= mNoSignalTimeThreshold)
    {
      mPositionEstimatorIndoor->reInit();
      levelId = LevelId();
    }

    if (curTs - lastSignalTs >= mNoActionTimeThreshold && curTs - motionInfo.lastMotionTs >= mNoActionTimeThreshold && motionInfo.lastMotionTs > 0)
    { 
      mPositionEstimatorIndoor->reInit();
      levelId = LevelId();
    }

    if (mLevelIndex->hasLevel(levelId))
    {
      const Level& level = mLevelIndex->level(levelId);
      const Position indoorPos = mPositionEstimatorIndoor->calculatePosition(level, curTs, radioMsr, motionInfo, retStatus);
      if (!indoorPos.isEmpty)
        mFlagIndoorPos = true;

      const Position fusedPos = mPositionPostprocessor->fusePositions(curTs, indoorPos, outdoorPos, retStatus);

      if (!fusedPos.isEmpty && mPrevFusedPosHeading.is_initialized() && (fusedPos.heading != mPrevFusedPosHeading.get()))
      {
        mPrevGyroHeading = motionInfo.gyroHeading;
        motionInfo.isAzimuthValid = false;
      }
      else if (mPrevGyroHeading.is_initialized())
      {
        double deltaGyroHeading = motionInfo.gyroHeading - mPrevGyroHeading.get();
        motionInfo.azimuth = - (mPrevFusedPosHeading.get() - deltaGyroHeading - M_PI_2); // NED
        motionInfo.isAzimuthValid = true;
      }

      if (!fusedPos.isEmpty)
        mPrevFusedPosHeading = fusedPos.heading;

      mPosition = mPositionPostprocessor->getProcessedPosition(fusedPos, curTs, motionInfo, level);
      if (!motionInfo.isAzimuthValid) {
        motionInfo.azimuth = - (fusedPos.heading - M_PI_2); // convert from ENU to NED
      }

      if (mUseEnuAzimuth)
        motionInfo.azimuth = -motionInfo.azimuth + M_PI_2; // convert from NED to ENU
    }
    else if (!outdoorPos.isEmpty)
    {
      mPosition = outdoorPos;
    }
    else
    {
      retStatus = NavigationStatus::NO_LEVEL;
    }

    // Filling navigationSate for Debug
    auto navigationState = mPositionEstimatorIndoor->getParticleFilterState();
    navigationState.setStepCounter(motionInfo.stepCounter);
    navigationState.setStepLen(motionInfo.distance);
    navigationState.setIndoorPosition(XYPoint(mPositionPostprocessor->getIndoorPosition().x,
                                              mPositionPostprocessor->getIndoorPosition().y));
    navigationState.setOutdoorPosition(XYPoint(mPositionPostprocessor->getOutdoorPosition().x,
                                                mPositionPostprocessor->getOutdoorPosition().y));
    mNavigationStates.push_back(navigationState);

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
      if (mUseAltitude)
      {
        Trilateration altitudeWorker;
        boost::optional<double> altitude = altitudeWorker.calculateAltitude(level, radioMsr);
        if (altitude.is_initialized())
            navOutput.posAltitude = altitude.get();
      }
    }

    navOutput.posR = mPosition.deviationM;
    navOutput.provider = mPosition.provider;
    navOutput.posOrientation = to_minus_Pi_Pi(motionInfo.azimuth);
    navBatchOutput.emplace_back(navOutput);
  }
  return navBatchOutput;
}

std::unique_ptr<LevelEstimator> NavigationClientImpl::createLevelEstimator(
  const std::shared_ptr<navigine::navigation_core::LevelCollector>& levelCollector,
  const NavigationSettings& navProps)
{
  const bool useRadiomap = navProps.commonSettings.useRadiomap;

  // the following three lines are used for navigation.xml backcompatibility
  const CommonSettings::UseAlgorithm algName = navProps.commonSettings.useAlgorithm;
  const bool useKnn = (algName == CommonSettings::UseAlgorithm::KNN);

  if (useRadiomap || useKnn)
    return std::make_unique<LevelEstimatorRadiomap>(levelCollector, navProps);

  return std::make_unique<LevelEstimatorTransmitters>(levelCollector, navProps);
}

std::unique_ptr<PositionEstimator> NavigationClientImpl::createPostitionEstimator(
  const std::shared_ptr<navigine::navigation_core::LevelCollector>& levelCollector,
  const NavigationSettings& navProps)
{
  const CommonSettings::UseAlgorithm algName = navProps.commonSettings.useAlgorithm;
 
  bool usePDR;
  if (algName == CommonSettings::UseAlgorithm::KNN) return std::make_unique<PositionEstimatorKnn>(levelCollector, navProps);

  return std::make_unique<PositionEstimatorZone>(levelCollector, navProps);
}

std::vector<NavigationState> NavigationClientImpl::getStates() const
{
  return mNavigationStates;
}

std::shared_ptr<NavigationClient> createNavigationClient(
  const std::shared_ptr<navigine::navigation_core::LevelCollector>& levelCollector,
  const NavigationSettings& navProps)
{
  return std::make_shared<NavigationClientImpl>(levelCollector, navProps);
}

} } // namespace navigine::navigation_core
