#pragma once

#include <quaternion.h>
#include <vector3d.h>

namespace navigine {
namespace navigation_core {

struct SensorMeasurement
{
  enum class Type { ACCELEROMETER, MAGNETOMETER, GYROSCOPE, BAROMETER, LOCATION, ORIENTATION};

  Type        type;
  long long   ts      = -1;
  Vector3d    values  = Vector3d();
};

double to_minus_Pi_Pi(double x);

struct Orientation
{
  double roll  = 0.0;
  double pitch = 0.0;
  double yaw   = 0.0;
  long long ts = 0;

  Orientation(double roll_, double pitch_, double yaw_, long long ts_) 
   : roll  {roll_ }
   , pitch {pitch_}
   , yaw   {yaw_  }
   , ts    {ts_   }
  {}
};

class ComplementaryFilter
{
  public:
    ComplementaryFilter () {}
    ComplementaryFilter (double Ka, double Km, double Ki)
      : mKaccelerometer {Ka}
      , mKmagnetometer  {Km}
      , mKintergalGain  {Ki}
    {}
  
    double      getMagneticAzimuth()  const;
    double      getFusedAzimuth() const;
    double      getDeviceAzimuth() const;
    Orientation getFusedOrientation() const;
    void        update(const SensorMeasurement& msr);
  
  private:
    void   updateUsingGyroscope     (const SensorMeasurement& gyro);
    void   updateUsingAccelerometer (const SensorMeasurement& accel);
    void   updateUsingMagnetometer  (const SensorMeasurement& magn);
    double caclulateMagneticAzimuth (const SensorMeasurement& magn);

    long long  mCurrentTs         = -1;
    long long  mLastGyroTs        = -1;
    double     mKaccelerometer    = 0.1;
    double     mKmagnetometer     = 0.05;
    double     mKintergalGain     = 0.0;
    Vector3d   mIntergalError     = {};
    Quaternion mQ                 = {};
    double     mMagneticAzimuth   = 0.0;
    double     mDeviceAzimuth     = 0.0;
    SensorMeasurement mMagnLowPassed;                // Magnetometer measurements passed through low-pass filter
    SensorMeasurement mAccelLowPassed;               // Accelerometer measurements passed through low-pass filter
};

} } // namespace navigine::navigation_core
