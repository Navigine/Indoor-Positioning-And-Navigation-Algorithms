#include <fstream>
#include <sstream>
#include <iostream>
#include "measurement_preprocessor.h"
#include "nearest_beacon_estimator.h"

using namespace navigine::navigation_core;

struct NavigationPoint
{
  NavigationPoint(){}

  NavigationPoint(long long _timeMs, double _x, double _y, double _angle, int _sublocation)
    : timeMs      (_timeMs)
    , x           (_x)
    , y           (_y)
    , angle       (_angle)
    , sublocation (_sublocation)
  { }

  long long timeMs      = 0;
  double    x           = 0;
  double    y           = 0;
  double    angle       = 0;
  int       sublocation = 0;
};

std::vector<NavigationPoint> getReferenceData(const std::string &filename)
{
  double x = 0., y = 0., t = 0., fi = 0.;
  int subloc = 0;
  std::ifstream infile(filename);
  if (!infile.is_open())
  {
    std::cout << "ERROR: can't open reference file: " << filename << std::endl;
    return std::vector<NavigationPoint>();
  }

  std::vector<NavigationPoint> navPoints;
  std::string line;
  while (std::getline(infile, line))
  {
    if (line.find("#") != 0){ //ignore comments
      std::istringstream iss(line);
      iss >> x >> y >> t >> fi >> subloc;
      long long ts = 1000 * t;
      navPoints.push_back({ts, x, y, fi, subloc});
    }
  }
  infile.close();
  return navPoints;
}

std::vector<Transmitter> getTransmitters(const std::string& transmittersFileName)
{
  std::vector<Transmitter> transmitters;
  std::ifstream is(transmittersFileName);
  if (!is.is_open())
  {
    std::cout << "could not open file " << transmittersFileName << std::endl;
  }
  std::string line;
  while (std::getline(is, line))
  {
    std::istringstream iss(line);
    std::string typeStr;
    Transmitter t;
    if ((iss >> t.id >> t.x >> t.y >> typeStr ))
    {
      t.type = typeStr.find("WIFI") == std::string::npos
              ? Transmitter::Type::BEACON
              : Transmitter::Type::WIFI;
      transmitters.emplace_back(t);
    }
    else
      break;
  }
  is.close();

  return transmitters;
}

std::vector<RadioMeasurement> getMeasurements(const std::string& msrFileName)
{
  std::vector<RadioMeasurement> measurements;
  std::ifstream is(msrFileName);
  if (!is.is_open())
  {
    std::cout << "could not open file " << msrFileName << std::endl;
  }
  std::string line;
  while (std::getline(is, line))
  {
    std::istringstream iss(line);
    RadioMeasurement msr;
    if ((iss >> msr.ts >> msr.id >> msr.rssi))
      measurements.emplace_back(msr);
    else
      break;
  }
  is.close();
  return measurements;
}

std::vector<RadioMeasurements> splitToPackets(const std::vector<RadioMeasurement>& inputMeasuremetns)
{
  MeasurementsPreprocessor measurementsPreprocessor;
  std::vector<RadioMeasurements> inputMeasuremetnsPackets;
  for (const RadioMeasurement& msr: inputMeasuremetns) {
    measurementsPreprocessor.update(msr);
    RadioMeasurements measurementsPacket = measurementsPreprocessor.extractRadioMeasurements();
    if (!measurementsPacket.empty()) {
      inputMeasuremetnsPackets.emplace_back(measurementsPacket);
    }
  }
  return inputMeasuremetnsPackets;
}

int main()
{
  std::string testDataFolder = TEST_DATA_FOLDER;
  std::string transmittersFileName = testDataFolder + "transmitters.txt";
  std::string msrFileName = testDataFolder + "measuremetns.log";
  std::string benchFileName = testDataFolder + "benchmarks.log";
  std::string outputFileName = testDataFolder + "output.log";

  std::vector<Transmitter> transmitters = getTransmitters(transmittersFileName);
  std::vector<RadioMeasurement> inputMeasuremetns = getMeasurements(msrFileName);
  std::vector<NavigationPoint> benchmarkPoints = getReferenceData(benchFileName);

  std::vector<RadioMeasurements> inputMeasuremetnsPackets = splitToPackets(inputMeasuremetns);

  NearestBeaconEstimator nearestBeaconPositionEstimator = NearestBeaconEstimator(transmitters);

  std::ofstream os;
  os.open(outputFileName);
  for (const RadioMeasurements &msr: inputMeasuremetnsPackets)
  {
    Position p = nearestBeaconPositionEstimator.calculatePosition(msr);
    if (!p.isEmpty)
    {
      os << p.ts << " " << p.x << " " << p.y << " " << p.precision << std::endl;
    }
  }
  os.close();
  std::cout << "calculated positions were written to " << outputFileName << std::endl;

  return 0;
}
