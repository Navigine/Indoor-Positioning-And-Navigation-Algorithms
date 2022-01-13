#include <fstream>
#include <sstream>
#include <iostream>
#include "measurement_preprocessor.h"
#include "nearest_transmitter_estimator.h"

const std::string TEST_DATA_FOLDER = "logs";
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
      transmitters.push_back(t);
    }
    else
      break;
  }
  is.close();

  std::cout << "transmitters size: " << transmitters.size() << std::endl;
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
    std::string typeStr;
    if ((iss >> msr.ts >> msr.id >> msr.rssi >> typeStr))
    {
      if (typeStr.find("WIFI") == std::string::npos)
        msr.type = RadioMeasurement::Type::BEACON;
      else
        msr.type = RadioMeasurement::Type::WIFI;

      measurements.push_back(msr);
    }
    else
      break;
  }
  is.close();
  std::cout << "measurements size: " << measurements.size() << std::endl;
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
  std::cout << "measurements packets size: " << inputMeasuremetnsPackets.size() << std::endl;
  return inputMeasuremetnsPackets;
}

int main()
{
  std::string testDataFolder = TEST_DATA_FOLDER;
  std::string transmittersFileName = testDataFolder + "transmitters.txt";
  std::string msrFileName = testDataFolder + "measurements.log";
  std::string outputFileName = testDataFolder + "output.log";

  std::vector<Transmitter> transmitters = getTransmitters(transmittersFileName);
  std::vector<RadioMeasurement> inputMeasuremetns = getMeasurements(msrFileName);
  std::vector<RadioMeasurements> inputMeasuremetnsPackets = splitToPackets(inputMeasuremetns);

  NearestTransmitterPositionEstimator nearestBeaconPositionEstimator = NearestTransmitterPositionEstimator(transmitters);

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
