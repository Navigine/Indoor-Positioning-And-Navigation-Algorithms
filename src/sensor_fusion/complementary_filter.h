/** complementary_filter
 *
 * Copyright (c) 2018 Navigine.
 *
 */

#ifndef COMPLEMENTARY_FILTER_H
#define COMPLEMENTARY_FILTER_H

#include <navigine/navigation-core/navigation_input.h>
#include <navigine/navigation-core/navigation_settings.h>
#include <navigine/navigation-core/vector3d.h>

#include "quaternion.h"

#include "../measurement_types.h"

namespace navigine {
namespace navigation_core {

double to_minus_Pi_Pi(double x);

struct Orientation
{
  double roll  = 0.0;
  double pitch = 0.0;
  double yaw   = 0.0;
  long long ts = 0;
  Orientation(double roll_, double pitch_, double yaw_, long long ts_) 
   : roll{roll_}
   , pitch{pitch_}
   , yaw{yaw_}
   , ts{ts_} {};
};

class ComplementaryFilter
{
  public:
    ComplementaryFilter (const NavigationSettings& navProps, const double Ka = 0.1, const double Km = 0.05, const double Ki = 0.0)
      : mKaccelerometer (Ka)
      , mKmagnetometer (Km)
      , mKintergalGain (Ki)
      , mDeviceAzimuthLifetimeSeconds(navProps.commonSettings.deviceAzimuthLifetimeSeconds)
    {};
  
    double      getMagneticAzimuth()  const;
    double      getFusedAzimuth() const;
    double      getDeviceAzimuth() const;
    double      getGyroHeading() const;
    Orientation getFusedOrientation() const;
    void        update(const SensorMeasurement& msr);
    bool        isDeviceAzimuthOutdated() const;
  
  private:
    void   updateUsingGyroscope     (const SensorMeasurement& gyro);
    void   updateUsingAccelerometer (const SensorMeasurement& accel);
    void   updateUsingMagnetometer  (const SensorMeasurement& magn);
    double calculateMagneticAzimuth (const SensorMeasurement& magn);

    long long  mCurrentTs = -1;
    long long  mLastGyroTs = -1;
    long long  mLastDeviceAzimuthTs = -1;
    const double mKaccelerometer;
    const double mKmagnetometer;
    const double mKintergalGain;
    const double mDeviceAzimuthLifetimeSeconds;
    Vector3d mIntergalError = {};
    Quaternion mQ = {};
    double mMagneticAzimuth = 0.0;
    double mDeviceAzimuth = 0.0;
    double mGyroHeading = 0.0;
    SensorMeasurement mMagnLowPassed;                // Magnetometer measurements passed through low-pass filter
    SensorMeasurement mAccelLowPassed;               // Accelerometer measurements passed through low-pass filter
};

} } // namespace navigine::navigation_core

#endif // COMPLEMENTARY_FILTER_H
