#include "complementary_filter.h"

namespace navigine {
namespace navigation_core {

static const double MIN_SAMPLING_PERIOD_SEC = 0.02;
static const double LPF_TIME_CONSTANT_SEC   = 1.0;
static const double DOUBLE_EPSILON          = 1e-8;

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
  res.values = alpha * prevMsr.values + (1 - alpha) * currMsr.values;

  return res;
}

/** This function returns calculated orientation (in radians) and current timestamp.
 *  The orientation is right-handed, therefore counter-clockwise rotation has positive sign
 */
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

/** measurements time must be in ascending order! */
void ComplementaryFilter::update(const SensorMeasurement& msr)
{
 
  if(msr.type == SensorMeasurement::Type::ACCELEROMETER)
  {
    mAccelLowPassed = lowPassFilter(mAccelLowPassed, msr);
    updateUsingAccelerometer(msr);
  }
  else if (msr.type == SensorMeasurement::Type::MAGNETOMETER)
  {
    mMagnLowPassed = lowPassFilter(mMagnLowPassed, msr);
    if (mLastGyroTs == -1)
      updateUsingMagnetometer(msr);

    mMagneticAzimuth = caclulateMagneticAzimuth(msr);
  }
  else if (msr.type == SensorMeasurement::Type::GYROSCOPE)
  {
    if (mLastGyroTs == -1)
      mLastGyroTs = msr.ts;

    updateUsingGyroscope(msr);
  }
  else if (msr.type == SensorMeasurement::Type::ORIENTATION)
  {
    mDeviceAzimuth = msr.values.x;
  }

  mCurrentTs = msr.ts;
 
  return;
}

void ComplementaryFilter::updateUsingGyroscope(const SensorMeasurement& gyroMeas)
{
  double   dt    = static_cast<double>(gyroMeas.ts - mLastGyroTs) / 1000.0;
  mLastGyroTs    = gyroMeas.ts;
  Vector3d gyro  = gyroMeas.values;
  Vector3d omega = gyro * dt + mIntegralError * dt;
  mQ = updateQuaternion(mQ, omega);
} 

void ComplementaryFilter::updateUsingAccelerometer(const SensorMeasurement& accelMeas)
{
  Vector3d acc             = accelMeas.values.normalized();
  Vector3d gravSensorFrame = (mQ.conj() * Quaternion(0.0, 0.0, 0.0, 1.0) * mQ).toVector3d();
  Vector3d error           = Vector3d::crossProduct(acc, gravSensorFrame);
  Vector3d rotation        = mKaccelerometer * error;

  mQ     = updateQuaternion(mQ, rotation);
  mIntegralError += (mKintegralGain > DOUBLE_EPSILON) ? (mKintegralGain * error) : Vector3d(0.0, 0.0, 0.0);
}

void ComplementaryFilter::updateUsingMagnetometer(const SensorMeasurement& magnMeas)
{
  Vector3d magn = magnMeas.values.normalized();
  if (magn.magnitude() < DOUBLE_EPSILON)
    return;
  
  Vector3d magnGlobalFrame = (mQ * Quaternion(magn) * mQ.conj()).toVector3d();
  magnGlobalFrame.y        = std::sqrt(magnGlobalFrame.y * magnGlobalFrame.y + magnGlobalFrame.x * magnGlobalFrame.x);
  magnGlobalFrame.x        = 0.0;
  Vector3d magnSensorFrame = (mQ.conj() * Quaternion(magnGlobalFrame) * mQ).toVector3d();

  Vector3d error           = Vector3d::crossProduct(magn, magnSensorFrame);
  Vector3d rotation        = mKmagnetometer * error;

  mQ = updateQuaternion(mQ, rotation);
  mIntegralError += (mKintegralGain > DOUBLE_EPSILON) ? (mKintegralGain * error) : Vector3d(0.0, 0.0, 0.0);
}

double ComplementaryFilter::caclulateMagneticAzimuth(const SensorMeasurement& magn)
{
  double roll, pitch, yaw;
  std::tie(roll, pitch, yaw) = mQ.toEuler();
  double halfYaw = yaw / 2.0;

  Quaternion quatDifference = Quaternion(sin(halfYaw), 0, 0, cos(halfYaw)).normalized();
  Quaternion quatZeroYaw    = quatDifference * mQ;

  Vector3d magnetometer    = magn.values.normalized();
  Vector3d magnGlobalFrame = (quatZeroYaw * Quaternion(magnetometer) * quatZeroYaw.conj()).toVector3d();

  return to_minus_Pi_Pi(M_PI / 2.0 + atan2(magnGlobalFrame.y, magnGlobalFrame.x));
}

} } // namespace navigine::navigation_core
