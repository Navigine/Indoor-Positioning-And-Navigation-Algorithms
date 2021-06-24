/** complementary_filter.cpp
 *
 * Copyright (c) 2018 Navigine.
 *
 */

#include <navigine/navigation-core/navigation_input.h>
#include <navigine/navigation-core/navigation_settings.h>
#include "complementary_filter.h"

namespace navigine {
namespace navigation_core {

static const double CF_DOUBLE_EPSILON = 1e-8;
static const double MIN_SAMPLING_PERIOD_SEC = 0.02;
static const double LPF_TIME_CONSTANT_SEC   = 1.0;

double to_minus_Pi_Pi(double x)
{
  while ( x >= M_PI ) x -= 2 * M_PI;
  while ( x < -M_PI ) x += 2 * M_PI;
  return x;
}

Quaternion updateQuaternion(const Quaternion& q, const Vector3d& v)
{
  Quaternion qDot = q * Quaternion(v) * 0.5;
  Quaternion qOut = q + qDot;
  return qOut.normalized();
}
 
SensorMeasurement lowPassFilter(const SensorMeasurement& prevMsr, const SensorMeasurement& currMsr)
{
  double   dt     = (currMsr.ts - prevMsr.ts) / 1000.0;
  double   alpha  = LPF_TIME_CONSTANT_SEC / (LPF_TIME_CONSTANT_SEC + std::max(dt, MIN_SAMPLING_PERIOD_SEC));

  SensorMeasurement res = currMsr;
  res.data.values = alpha * prevMsr.data.values + (1 - alpha) * currMsr.data.values;

  return res;
}

/** This function returns calculated orientation (in radians) and current timestamp */
/* the orientation is right-handed, therefore counter-clockwise rotation has positive sign */
Orientation ComplementaryFilter::getFusedOrientation() const
{
  double roll, pitch, yaw;
  std::tie(roll, pitch, yaw) = mQ.toEuler();
  return Orientation{roll, pitch, yaw, mCurrentTs};
}

/** This function returns calculated fused azimuth (in radians), clockwise has positive sign */
double ComplementaryFilter::getFusedAzimuth() const
{
  double roll, pitch, yaw;
  std::tie(roll, pitch, yaw) = mQ.toEuler();
  return -yaw;
}

/** This function returns calculated magnetic azimuth (in radians), clockwise has positive sign */
double ComplementaryFilter::getMagneticAzimuth() const
{
  return mMagneticAzimuth;
}

/** This function returns  azimuth calculated on device (in radians), clockwise has positive sign */
double ComplementaryFilter::getDeviceAzimuth() const
{
  return mDeviceAzimuth;
}

/** This function returns calculated gyro heading (in radians), clockwise has positive sign */
double ComplementaryFilter::getGyroHeading() const
{
  return mGyroHeading;
}

/** measurements time must be in ascending order! */
void ComplementaryFilter::update(const SensorMeasurement& msr)
{
 
  if(msr.data.type == SensorMeasurementData::Type::ACCELEROMETER)
  {
    mAccelLowPassed = lowPassFilter(mAccelLowPassed, msr);
    updateUsingAccelerometer(msr);
  }
  else if (msr.data.type == SensorMeasurementData::Type::MAGNETOMETER)
  {
    mMagnLowPassed = lowPassFilter(mMagnLowPassed, msr);
    if (mLastGyroTs == -1)
      updateUsingMagnetometer(msr);

    mMagneticAzimuth = calculateMagneticAzimuth(msr);
  }
  else if (msr.data.type == SensorMeasurementData::Type::GYROSCOPE)
  {
    if (mLastGyroTs == -1)
      mLastGyroTs = msr.ts;

    updateUsingGyroscope(msr);
    mGyroHeading = getFusedAzimuth();
  }
  else if (msr.data.type == SensorMeasurementData::Type::ORIENTATION)
  {
    mDeviceAzimuth = msr.data.values.x;
    mLastDeviceAzimuthTs = msr.ts;
  }

  mCurrentTs = msr.ts;
 
  return;
}

bool ComplementaryFilter::isDeviceAzimuthOutdated() const
{
  double dt = static_cast<double>(mCurrentTs - mLastDeviceAzimuthTs) / 1000.0;
  if (dt > mDeviceAzimuthLifetimeSeconds || mLastDeviceAzimuthTs == -1)
    return true;
  else
    return false;
}

void ComplementaryFilter::updateUsingGyroscope(const SensorMeasurement& gyroMeas)
{
  double dt = static_cast<double>(gyroMeas.ts - mLastGyroTs) / 1000.0;
  mLastGyroTs = gyroMeas.ts;
  Vector3d gyro = gyroMeas.data.values;
  Vector3d omega = gyro * dt + mIntergalError * dt;
  mQ = updateQuaternion(mQ, omega);
} 

void ComplementaryFilter::updateUsingAccelerometer(const SensorMeasurement& accelMeas)
{
  Vector3d acc = accelMeas.data.values.normalized();
  Vector3d gravSensorFrame = (mQ.conj() * Quaternion(0.0, 0.0, 0.0, 1.0) * mQ).toVector3d();
  Vector3d error = Vector3d::crossProduct(acc, gravSensorFrame);
  Vector3d rotation = mKaccelerometer * error;

  mQ = updateQuaternion(mQ, rotation);
  mIntergalError += (mKintergalGain > CF_DOUBLE_EPSILON) ? (mKintergalGain * error) : Vector3d(0.0, 0.0, 0.0);
}

void ComplementaryFilter::updateUsingMagnetometer(const SensorMeasurement& magnMeas)
{
  Vector3d magn = magnMeas.data.values.normalized();
  if (magn.magnitude() < CF_DOUBLE_EPSILON)
    return;
  
  Vector3d magnGlobalFrame = (mQ * Quaternion(magn) * mQ.conj()).toVector3d();
  magnGlobalFrame.y = std::sqrt(magnGlobalFrame.y * magnGlobalFrame.y + magnGlobalFrame.x * magnGlobalFrame.x);
  magnGlobalFrame.x = 0.0;
  Vector3d magnSensorFrame = (mQ.conj() * Quaternion(magnGlobalFrame) * mQ).toVector3d();

  Vector3d error = Vector3d::crossProduct(magn, magnSensorFrame);
  Vector3d rotation = mKmagnetometer * error;

  mQ = updateQuaternion(mQ, rotation);
  mIntergalError += (mKintergalGain > CF_DOUBLE_EPSILON) ? (mKintergalGain * error) : Vector3d(0.0, 0.0, 0.0);
}

double ComplementaryFilter::calculateMagneticAzimuth(const SensorMeasurement& magn)
{
  double roll, pitch, yaw;
  std::tie(roll, pitch, yaw) = mQ.toEuler();
  double halfYaw     = yaw / 2.0;

  Quaternion quatDifference = Quaternion(sin(halfYaw), 0, 0, cos(halfYaw)).normalized();
  Quaternion quatZeroYaw    = quatDifference * mQ;

  Vector3d magnetometer    = magn.data.values.normalized();
  Vector3d magnGlobalFrame = (quatZeroYaw * Quaternion(magnetometer) * quatZeroYaw.conj()).toVector3d();

  return to_minus_Pi_Pi(M_PI / 2.0 + atan2(magnGlobalFrame.y, magnGlobalFrame.x));
}

} } // namespace navigine::navigation_core
