#include <complementary_filter.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <random>

using namespace navigine::navigation_core;

static const double MAGNETIC_FLUX_DENSITY = 0.48;
static const double MAGNETIC_FIELD_INCLINATION = 60.48 * M_PI / 180;
static const double G = 9.81;
const std::string TEST_DATA_FOLDER = "test_data/";

/**
 * duration - duration of pendulum motion in seconds
 * freq - sampling frequency in Hz
 * L - pendulum length
 * theta0 - initial angle in radians
 *
 * Returns:
 * the angle, angular velocity and angular acceleration for pendulum motion
 */
std::tuple<std::vector<double>,
           std::vector<double>,
           std::vector<double>> simulatePendulumMotion(double durationStaticSec,
                                                       double durationDynamicSec,
                                                       double freqHz = 100,
                                                       double L = 1.0,
                                                       double theta0 = M_PI / 60.0)
{
    std::vector<double> times;
    std::vector<double> thetas;
    std::vector<double> thetaVels;

    double t = 0;
    double tick = 1.0 / freqHz;
    //first 10 seconds keep static pendulum tilt
    for (int i = 0; i < durationStaticSec / tick; i++) {
      times.emplace_back(t);
      thetas.emplace_back(theta0);
      thetaVels.emplace_back(0);
      t += tick;
    }

    double thetaPrev = theta0;
    for (int i = 0; i < durationDynamicSec / tick; i++)
    {
      double theta = theta0 * cos(sqrt(G / L) * t);
      double thetaVel = (theta - thetaPrev) / tick;

      times.emplace_back(t);
      thetas.emplace_back(theta);
      thetaVels.emplace_back(thetaVel);

      t += tick;
      thetaPrev = theta;
    }

    return std::make_tuple(times, thetas, thetaVels);
}

std::tuple<std::vector<SensorMeasurement>,
           std::vector<SensorMeasurement>,
           std::vector<SensorMeasurement>> generateImuPendulum(std::vector<double> t,
                                                               std::vector<double> theta,
                                                               std::vector<double> thetaVel,
                                                               double freqHz = 100,
                                                               double L = 1)
{
  long long refTs = 1583315232492;
  std::random_device rd;
  std::mt19937 gen(rd());

  std::vector<SensorMeasurement> accelMeasurements;
  for (std::size_t i = 0; i < t.size(); i++)
  {
    //the variance is choosen to be equal to typical accelerometer noise variance of cheap IMU sensor
    double var = 2 * M_PI * freqHz * G * pow(0.001, 2);
    std::normal_distribution<double> distr(0.0, var);

    double ax = -G * sin(theta[i]) + distr(gen);
    double ay = 0.0 + distr(gen);
    double az = -L * pow(thetaVel.at(i), 2) + G * cos(theta[i]) + distr(gen);
    SensorMeasurement msr;
    msr.ts = refTs + 1000 * t.at(i);
    msr.values = Vector3d(ax, ay, az);
    msr.type = SensorMeasurement::Type::ACCELEROMETER;
    accelMeasurements.emplace_back(msr);
  }

  std::vector<SensorMeasurement> gyroMeasurements;
  for (std::size_t i = 0; i < t.size(); i++)
  {
    //the variance is choosen to be equal to typical gyroscope noise variance of cheap IMU sensor
    double var = 2 * M_PI * freqHz * pow(0.01 * M_PI / 180.0, 2);
    std::normal_distribution<double> distr(0.005, var);

    double wx = 0.0 + distr(gen);
    double wy = thetaVel.at(i) + distr(gen);
    double wz = 0.0 + distr(gen);
    SensorMeasurement msr;
    msr.ts = refTs + 1000 * t.at(i);
    msr.values = Vector3d(wx, wy, wz);
    msr.type = SensorMeasurement::Type::GYROSCOPE;
    gyroMeasurements.emplace_back(msr);
  }

  std::vector<SensorMeasurement> magnMeasurements;
  for (std::size_t i = 0; i < t.size(); i++)
  {
    double mx = MAGNETIC_FLUX_DENSITY * cos(MAGNETIC_FIELD_INCLINATION + theta[i]);
    double my = 0.0;
    double mz = MAGNETIC_FLUX_DENSITY * sin(MAGNETIC_FIELD_INCLINATION + theta[i]);
    SensorMeasurement msr;
    msr.ts = refTs + 1000 * t.at(i);
    msr.values = Vector3d(mx, my, mz);
    msr.type = SensorMeasurement::Type::MAGNETOMETER;
    magnMeasurements.emplace_back(msr);
  }

  return std::make_tuple(accelMeasurements, gyroMeasurements, magnMeasurements);
}

int main()
{
  std::string testDataFolder = TEST_DATA_FOLDER;
  std::vector<double> times;
  std::vector<double> thetas;
  std::vector<double> thetaVels;

  std::tie(times, thetas, thetaVels) = simulatePendulumMotion(10, 10);

  std::vector<SensorMeasurement> accelMeasurements;
  std::vector<SensorMeasurement> gyroMeasurements;
  std::vector<SensorMeasurement> magnMeasurements;

  std::tie(accelMeasurements, gyroMeasurements, magnMeasurements) =
           generateImuPendulum(times, thetas, thetaVels);

  std::ofstream os;
  std::string simulatedMsrFileName = testDataFolder + "simulation.log";
  os.open(simulatedMsrFileName);
  for (std::size_t i = 0; i < times.size(); i++)
  {
    SensorMeasurement accelMsr = accelMeasurements.at(i);
    SensorMeasurement gyroMsr = gyroMeasurements.at(i);
    SensorMeasurement magnMsr = magnMeasurements.at(i);
    double theta = thetas.at(i);
    os << accelMsr.ts << " " << theta << " "
       << accelMsr.values.x << " " << accelMsr.values.y << " " << accelMsr.values.z << " "
       << gyroMsr.values.x << " " << gyroMsr.values.y << " " << gyroMsr.values.z << " "
       << magnMsr.values.x << " " << magnMsr.values.y << " " << magnMsr.values.z << std::endl;
  }
  os.close();
  std::cout << "simulated data was written to " << simulatedMsrFileName << std::endl;

  ComplementaryFilter complementaryFilter;

  std::string calculatedAngleFileName = testDataFolder + "calculated_angles.log";
  os.open(calculatedAngleFileName);
  for (std::size_t i = 0; i < accelMeasurements.size(); i++)
  {
    const SensorMeasurement& accelMsr = accelMeasurements.at(i);
    const SensorMeasurement& gyroMsr = gyroMeasurements.at(i);
    const SensorMeasurement& magnMsr = magnMeasurements.at(i);
    complementaryFilter.update(accelMsr);
    complementaryFilter.update(gyroMsr);
    complementaryFilter.update(magnMsr);
    Orientation ori = complementaryFilter.getFusedOrientation();
    os << accelMsr.ts << " " << ori.roll << " " << ori.pitch  << " " << ori.yaw << std::endl;
  }
  os.close();
  std::cout << "calculated angles were written to " << calculatedAngleFileName << std::endl;

  return 0;
}
