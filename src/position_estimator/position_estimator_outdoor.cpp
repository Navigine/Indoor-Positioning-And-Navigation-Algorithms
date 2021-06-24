/** position_estimator_outdoor.cpp
 *
 * Copyright (c) 2018 Navigine.
 *
 */
#include <navigine/navigation-core/navigation_settings.h>
#include "position_estimator_outdoor.h"

namespace navigine {
namespace navigation_core {

bool SigmaFilter::add(const XYPoint& point)
{
  bool pntValid = false;
  if (mGNSSbuffer.size() == mSigmaWindow) {
    double sumX = 0;
    double sumY = 0;
    for (auto const& pnt : mGNSSbuffer)
    {
      sumX += pnt.x;
      sumY += pnt.y;
    }
    double meanX = sumX / mSigmaWindow;
    double meanY = sumY / mSigmaWindow;

    double accumX = 0.0;
    double accumY = 0.0;
    std::for_each(std::begin(mGNSSbuffer), std::end(mGNSSbuffer), [&](auto const d) {
        accumX += (d.x - meanX) * (d.x - meanX);
        accumY += (d.y - meanY) * (d.y - meanY);
    });

    auto deviationX = 3.0 * std::sqrt(accumX / mSigmaWindow) + mSigmaDelta;
    auto deviationY = 3.0 * std::sqrt(accumY / mSigmaWindow) + mSigmaDelta;

    if (std::abs(meanX - point.x) <= deviationX &&
        std::abs(meanY - point.y) <= deviationY)
        pntValid = true;

    mGNSSbuffer.pop_front();
  }
  mGNSSbuffer.push_back(point);
  return pntValid;
}

void SigmaFilter::clear()
{
  mGNSSbuffer.clear();
}

PositionEstimatorOutdoor::PositionEstimatorOutdoor(
  const std::shared_ptr<LevelCollector>& levelCollector,
  const NavigationSettings& navProps)
  : mLevelIndex (levelCollector)
  , mSigmaFilter(navProps.commonSettings.sigmaFilterWindow, navProps.commonSettings.sigmaFilterDelta)
  , mGpsValidTimeWindowSec(navProps.commonSettings.gpsValidTimeWindowSec)
  , mUseGpsOutsideMap(navProps.commonSettings.useGpsOutsideMap)
  , mUseSigmaFilter(navProps.commonSettings.useGpsSigmaFilter)
  , mMinNumSats(navProps.commonSettings.minNumSats)
  , mMinGpsDeviation(navProps.commonSettings.minGpsDeviation)
  , mMaxGpsDeviation(navProps.commonSettings.maxGpsDeviation)
{ }

GpsPosition PositionEstimatorOutdoor::extractGpsPosition(long long ts, const SensorMeasurement& sensorMsr)
{

  if (sensorMsr.data.type == SensorMeasurementData::Type::LOCATION)
  {
    double lat = sensorMsr.data.values.x;
    double lng = sensorMsr.data.values.y;
    double dev = std::max(sensorMsr.data.values.z, mMinGpsDeviation);
    GpsPosition pos = GpsPosition(lat, lng, dev, ts);
    return pos;
  }
  else
  {
    return GpsPosition();
  }
}

int PositionEstimatorOutdoor::extractNumberOfSatellites(const NmeaMeasurement& nmeaEntry)
{
  int totalNumSats = 0;

  if ((nmeaEntry.data.sentenceNumber == 1) && (nmeaEntry.data.satellitesNumber != 0))
    totalNumSats += nmeaEntry.data.satellitesNumber;
  return totalNumSats;
}

Position PositionEstimatorOutdoor::calculatePosition(
  long long ts,
  const SensorMeasurement& sensorEntry,
  const NmeaMeasurement& nmeaEntry,
  const bool flagIndoorPos)
{
  GpsPosition gpsPos = extractGpsPosition(ts, sensorEntry);
  int totalNumSats = extractNumberOfSatellites(nmeaEntry);

  if (flagIndoorPos || (gpsPos.deviation > mMaxGpsDeviation))
  {
    mSigmaFilter.clear(); //If there is indoor solution then clear sigma filter GNSS buffer.
  }

  if (gpsPos.isEmpty)
  {
    double timeSinceLastGpsPosition = (ts - mLastGpsPosition.ts) / 1000.0;
    if (timeSinceLastGpsPosition > mGpsValidTimeWindowSec)
        return Position(); //TODO error code: gps is empty!
    else
        return mLastGpsPosition;
  }

  //TODO find map closest to previous position (at least check if inside)!!!!
  for (const Level& level: mLevelIndex->levels())
  {
    XYPoint point = gpsToLocal(GeoPoint(gpsPos.latitude, gpsPos.longitude), level.bindingPoint());

    if ((!std::isnan(point.x) && !std::isnan(point.y)
      && boost::geometry::covered_by(Point(point.x, point.y), level.geometry().boundingBox())) || mUseGpsOutsideMap)
    {
      Position outPos;

      outPos.ts         = gpsPos.ts;
      outPos.x          = point.x;
      outPos.y          = point.y;
      outPos.levelId    = level.id();
      outPos.isEmpty    = false;
      outPos.deviationM = gpsPos.deviation;
      outPos.provider   = Provider::GNSS;
      
      bool isValid = true;
      if (outPos.ts != mLastGpsPosition.ts)
      {
        if (mUseSigmaFilter && !mSigmaFilter.add(point))
          isValid = false;
      }

      if (!isValid || (totalNumSats != 0 && totalNumSats < mMinNumSats))
        return Position();
      mLastGpsPosition = outPos;

      return outPos;
    }
  }

  return Position(); //TODO error code: "can not find map for gps position!"
}

} } // namespace navigine::navigation_core
