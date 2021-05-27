#include <complementary_filter.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <random>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ComplementaryFilter
#include <boost/test/unit_test.hpp>

using namespace navigine::navigation_core;

static const double MAGNETIC_FLUX_DENSITY = 0.48;
static const double MAGNETIC_FIELD_INCLINATION = 60.48 * M_PI / 180;
static const double G = 9.81;

std::tuple<std::vector<SensorMeasurement>,
           std::vector<SensorMeasurement>,
           std::vector<SensorMeasurement>> getTiltedImuMeasurements(double durationStaticSec,
                                                                    double tiltPitch = 0.0,
                                                                    double freqHz = 100)
{
  std::random_device rd;
  std::mt19937 gen(rd());

  std::vector<long long> times;
  long long t = 1583315232492;
  double tick = 1.0 / freqHz;
  for (int i = 0; i < durationStaticSec / tick; i++) {
    times.emplace_back(t);
    t += 1000 * tick;
  }

  std::vector<SensorMeasurement> accelMeasurements;
  for (long long ts: times)
  {
    //the variance is choosen to be equal to typical accelerometer noise variance of cheap IMU sensor
    double var = 2 * M_PI * freqHz * G * pow(0.001, 2);
    std::normal_distribution<double> distr(0.0, var);

    double ax = -G * sin(tiltPitch) + distr(gen);
    double ay = 0.0 + distr(gen);
    double az = G * cos(tiltPitch) + distr(gen);
    SensorMeasurement msr;
    msr.ts = ts;
    msr.values = Vector3d(ax, ay, az);
    msr.type = SensorMeasurement::Type::ACCELEROMETER;
    accelMeasurements.emplace_back(msr);
  }

  std::vector<SensorMeasurement> gyroMeasurements;
  for (long long ts: times)
  {
    //the variance is choosen to be equal to typical gyroscope noise variance of cheap IMU sensor
    double var = 2 * M_PI * freqHz * pow(0.01 * M_PI / 180.0, 2);
    std::normal_distribution<double> distr(0.005, var);

    double wx = 0.0 + distr(gen);
    double wy = 0.0 + distr(gen);
    double wz = 0.0 + distr(gen);
    SensorMeasurement msr;
    msr.ts = ts;
    msr.values = Vector3d(wx, wy, wz);
    msr.type = SensorMeasurement::Type::GYROSCOPE;
    gyroMeasurements.emplace_back(msr);
  }

  std::vector<SensorMeasurement> magnMeasurements;
  for (long long ts: times)
  {
    double mx = MAGNETIC_FLUX_DENSITY * cos(MAGNETIC_FIELD_INCLINATION + tiltPitch);
    double my = 0.0;
    double mz = MAGNETIC_FLUX_DENSITY * sin(MAGNETIC_FIELD_INCLINATION + tiltPitch);
    SensorMeasurement msr;
    msr.ts = ts;
    msr.values = Vector3d(mx, my, mz);
    msr.type = SensorMeasurement::Type::MAGNETOMETER;
    magnMeasurements.emplace_back(msr);
  }

  return std::make_tuple(accelMeasurements, gyroMeasurements, magnMeasurements);
}

BOOST_AUTO_TEST_CASE(staticTilt)
{
  std::vector<SensorMeasurement> accelMeasurements;
  std::vector<SensorMeasurement> gyroMeasurements;
  std::vector<SensorMeasurement> magnMeasurements;

  double durationStaticSec = 10.0;
  double tiltPitch = 0.0;
  std::tie(accelMeasurements, gyroMeasurements, magnMeasurements) =
          getTiltedImuMeasurements(durationStaticSec, tiltPitch);

  Orientation ori(0, 0, 0, 0);
  ComplementaryFilter complementaryFilter;
  for (std::size_t i = 0; i < accelMeasurements.size(); i++)
  {
    const SensorMeasurement& accelMsr = accelMeasurements.at(i);
    const SensorMeasurement& gyroMsr = gyroMeasurements.at(i);
    const SensorMeasurement& magnMsr = magnMeasurements.at(i);
    complementaryFilter.update(accelMsr);
    complementaryFilter.update(gyroMsr);
    complementaryFilter.update(magnMsr);
    ori = complementaryFilter.getFusedOrientation();
  }

  BOOST_CHECK(std::abs(ori.roll) < M_PI * 1.0 / 180);
  BOOST_CHECK(std::abs(ori.pitch - tiltPitch) < M_PI * 1.0 / 180);
}
