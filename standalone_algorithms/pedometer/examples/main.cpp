#include <pedometer.h>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace navigine::navigation_core;

std::vector<SensorMeasurement> parseMeasurements(const std::string& logFile)
{
  std::vector<SensorMeasurement> measurements;
  std::ifstream is(logFile);
  if (!is.is_open())
  {
    std::cout << "could not open file " << logFile << std::endl;
  }

  std::string line;
  std::getline(is, line); // skip test data
  while (std::getline(is, line))
  {
    SensorMeasurement msr;
    std::istringstream iss(line);
    if ((iss >> msr.ts >> msr.values.x >> msr.values.y >> msr.values.z))
      measurements.emplace_back(msr);
    else
      break;
  }
  is.close();
  return measurements;
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    std::cout << "can not find configuration file in command line argument! " << std::endl;
    exit(-1);
  }

  std::string logFile = std::string(argv[1]);
  std::vector<SensorMeasurement> measuremetns = parseMeasurements(logFile);

  Pedometer pedometer;
  long stepCounter = 0;
  for (SensorMeasurement msr: measuremetns)
  {
    std::vector<SensorMeasurement> singleMsrVector;
    singleMsrVector.emplace_back(msr);
    pedometer.update(singleMsrVector);
    std::deque<Step> steps = pedometer.calculateSteps();
    stepCounter += steps.size();
  }

  std::cout << "Number of steps: " << stepCounter << std::endl;

  return 0;
}
