#include <fstream>
#include <sstream>
#include <iostream>

#include <pedometer.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Pedometer
#include <boost/test/unit_test.hpp>

using namespace navigine::navigation_core;

namespace
{
  static const double STEP_NUM_TEST_THRESHOLD_PERCENTAGE = 5.0;
  static const double STEP_LEN_TEST_THRESHOLD_PERCENTAGE = 20.0;
  static const std::string TEST_DATA_FOLDER = "logs";
}

std::pair<std::pair<int, double>, std::vector<SensorMeasurement>> parseTestData(const std::string& logFile)
{
  std::vector<SensorMeasurement> measurements;
  std::ifstream is(logFile);
  if (!is.is_open())
  {
    std::cout << "could not open file " << logFile << std::endl;
  }
  std::string line;
  int stepsNum = 0;
  double stepLen = 0.0;
  while (std::getline(is, line))
  {
    std::istringstream iss(line);
    if (stepsNum == 0) // first line with ground truth
    {
      iss >> stepsNum >> stepLen;
    }
    else
    {
      SensorMeasurement msr;
      if ((iss >> msr.ts >> msr.values.x >> msr.values.y >> msr.values.z))
        measurements.emplace_back(msr);
      else
        break;
    }
  }
  is.close();
  return std::make_pair(std::make_pair(stepsNum, stepLen), measurements);
}

std::pair<int, double> getStepsNumLenPair(const std::vector<SensorMeasurement>& measuremetns)
{
  Pedometer pedometer;
  int stepCounter = 0;
  double stepLen = 0.0;
  for (const SensorMeasurement& msr: measuremetns)
  {
    std::vector<SensorMeasurement> singleMsrVector;
    singleMsrVector.emplace_back(msr);
    pedometer.update(singleMsrVector);
    std::deque<Step> steps = pedometer.calculateSteps();
    for (const Step& s: steps)
        stepLen += s.len;

    stepCounter += steps.size();
  }
  stepLen /= static_cast<double>(stepCounter);
  return std::make_pair(stepCounter, stepLen);
}

BOOST_AUTO_TEST_CASE(stepsHuaweiLong1)
{
  std::string testDir = TEST_DATA_FOLDER;
  std::pair<std::pair<int, double>,
            std::vector<SensorMeasurement> > testData = parseTestData(testDir + "/HuaweiLong1.log");

  std::pair<int, double> trueStepsNumLenPair = testData.first;
  std::pair<int, double> stepsNumLenPair = getStepsNumLenPair(testData.second);
  BOOST_CHECK_CLOSE(static_cast<double>(stepsNumLenPair.first),
                    static_cast<double>(trueStepsNumLenPair.first), STEP_NUM_TEST_THRESHOLD_PERCENTAGE);
  BOOST_CHECK_CLOSE(stepsNumLenPair.second, trueStepsNumLenPair.second, STEP_LEN_TEST_THRESHOLD_PERCENTAGE);
}

BOOST_AUTO_TEST_CASE(stepsHuaweiLong2)
{
  std::string testDir = TEST_DATA_FOLDER;
  std::pair<std::pair<int, double>,
            std::vector<SensorMeasurement> > testData = parseTestData(testDir + "/HuaweiLong2.log");

  std::pair<int, double> trueStepsNumLenPair = testData.first;
  std::pair<int, double> stepsNumLenPair = getStepsNumLenPair(testData.second);
  BOOST_CHECK_CLOSE(static_cast<double>(stepsNumLenPair.first),
                    static_cast<double>(trueStepsNumLenPair.first), STEP_NUM_TEST_THRESHOLD_PERCENTAGE);
  BOOST_CHECK_CLOSE(stepsNumLenPair.second, trueStepsNumLenPair.second, STEP_LEN_TEST_THRESHOLD_PERCENTAGE);
}

BOOST_AUTO_TEST_CASE(stepsIPhoneShort1)
{
  std::string testDir = TEST_DATA_FOLDER;
  std::pair<std::pair<int, double>,
            std::vector<SensorMeasurement> > testData = parseTestData(testDir + "/iPhoneShort1.log");

  std::pair<int, double> trueStepsNumLenPair = testData.first;
  std::pair<int, double> stepsNumLenPair = getStepsNumLenPair(testData.second);
  BOOST_CHECK_CLOSE(static_cast<double>(stepsNumLenPair.first),
                    static_cast<double>(trueStepsNumLenPair.first), STEP_NUM_TEST_THRESHOLD_PERCENTAGE);
  BOOST_CHECK_CLOSE(stepsNumLenPair.second, trueStepsNumLenPair.second, STEP_LEN_TEST_THRESHOLD_PERCENTAGE);
}

BOOST_AUTO_TEST_CASE(stepsIPhoneShort2)
{
  std::string testDir = TEST_DATA_FOLDER;
  std::pair<std::pair<int, double>,
            std::vector<SensorMeasurement> > testData = parseTestData(testDir + "/iPhoneShort2.log");

  std::pair<int, double> trueStepsNumLenPair = testData.first;
  std::pair<int, double> stepsNumLenPair = getStepsNumLenPair(testData.second);
  BOOST_CHECK_CLOSE(static_cast<double>(stepsNumLenPair.first),
                    static_cast<double>(trueStepsNumLenPair.first), STEP_NUM_TEST_THRESHOLD_PERCENTAGE);
  BOOST_CHECK_CLOSE(stepsNumLenPair.second, trueStepsNumLenPair.second, STEP_LEN_TEST_THRESHOLD_PERCENTAGE);
}

BOOST_AUTO_TEST_CASE(stepsIPhoneShort3)
{
  std::string testDir = TEST_DATA_FOLDER;
  std::pair<std::pair<int, double>,
            std::vector<SensorMeasurement> > testData = parseTestData(testDir + "/iPhoneShort3.log");

  std::pair<int, double> trueStepsNumLenPair = testData.first;
  std::pair<int, double> stepsNumLenPair = getStepsNumLenPair(testData.second);
  BOOST_CHECK_CLOSE(static_cast<double>(stepsNumLenPair.first),
                    static_cast<double>(trueStepsNumLenPair.first), STEP_NUM_TEST_THRESHOLD_PERCENTAGE);
  BOOST_CHECK_CLOSE(stepsNumLenPair.second, trueStepsNumLenPair.second, STEP_LEN_TEST_THRESHOLD_PERCENTAGE);
}

BOOST_AUTO_TEST_CASE(stepsIPhoneShort4)
{
  std::string testDir = TEST_DATA_FOLDER;
  std::pair<std::pair<int, double>,
            std::vector<SensorMeasurement> > testData = parseTestData(testDir + "/iPhoneShort4.log");

  std::pair<int, double> trueStepsNumLenPair = testData.first;
  std::pair<int, double> stepsNumLenPair = getStepsNumLenPair(testData.second);
  BOOST_CHECK_CLOSE(static_cast<double>(stepsNumLenPair.first),
                    static_cast<double>(trueStepsNumLenPair.first), STEP_NUM_TEST_THRESHOLD_PERCENTAGE);
  BOOST_CHECK_CLOSE(stepsNumLenPair.second, trueStepsNumLenPair.second, STEP_LEN_TEST_THRESHOLD_PERCENTAGE);
}

BOOST_AUTO_TEST_CASE(stepsNexusShort1)
{
  std::string testDir = TEST_DATA_FOLDER;
  std::pair<std::pair<int, double>,
            std::vector<SensorMeasurement> > testData = parseTestData(testDir + "/NexusShort1.log");

  std::pair<int, double> trueStepsNumLenPair = testData.first;
  std::pair<int, double> stepsNumLenPair = getStepsNumLenPair(testData.second);
  BOOST_CHECK_CLOSE(static_cast<double>(stepsNumLenPair.first),
                    static_cast<double>(trueStepsNumLenPair.first), STEP_NUM_TEST_THRESHOLD_PERCENTAGE);
  BOOST_CHECK_CLOSE(stepsNumLenPair.second, trueStepsNumLenPair.second, STEP_LEN_TEST_THRESHOLD_PERCENTAGE);
}

BOOST_AUTO_TEST_CASE(stepsNexusShort2)
{
  std::string testDir = TEST_DATA_FOLDER;
  std::pair<std::pair<int, double>,
            std::vector<SensorMeasurement> > testData = parseTestData(testDir + "/NexusShort2.log");

  std::pair<int, double> trueStepsNumLenPair = testData.first;
  std::pair<int, double> stepsNumLenPair = getStepsNumLenPair(testData.second);
  BOOST_CHECK_CLOSE(static_cast<double>(stepsNumLenPair.first),
                    static_cast<double>(trueStepsNumLenPair.first), STEP_NUM_TEST_THRESHOLD_PERCENTAGE);
  BOOST_CHECK_CLOSE(stepsNumLenPair.second, trueStepsNumLenPair.second, STEP_LEN_TEST_THRESHOLD_PERCENTAGE);
}

BOOST_AUTO_TEST_CASE(stepsNexusShort3)
{
  std::string testDir = TEST_DATA_FOLDER;
  std::pair<std::pair<int, double>,
            std::vector<SensorMeasurement> > testData = parseTestData(testDir + "/NexusShort3.log");

  std::pair<int, double> trueStepsNumLenPair = testData.first;
  std::pair<int, double> stepsNumLenPair = getStepsNumLenPair(testData.second);
  BOOST_CHECK_CLOSE(static_cast<double>(stepsNumLenPair.first),
                    static_cast<double>(trueStepsNumLenPair.first), STEP_NUM_TEST_THRESHOLD_PERCENTAGE);
  BOOST_CHECK_CLOSE(stepsNumLenPair.second, trueStepsNumLenPair.second, STEP_LEN_TEST_THRESHOLD_PERCENTAGE);
}

BOOST_AUTO_TEST_CASE(stepsXiaomiLong1)
{
  std::string testDir = TEST_DATA_FOLDER;
  std::pair<std::pair<int, double>,
            std::vector<SensorMeasurement> > testData = parseTestData(testDir + "/XiaomiLong1.log");

  std::pair<int, double> trueStepsNumLenPair = testData.first;
  std::pair<int, double> stepsNumLenPair = getStepsNumLenPair(testData.second);
  BOOST_CHECK_CLOSE(static_cast<double>(stepsNumLenPair.first),
                    static_cast<double>(trueStepsNumLenPair.first), STEP_NUM_TEST_THRESHOLD_PERCENTAGE);
  BOOST_CHECK_CLOSE(stepsNumLenPair.second, trueStepsNumLenPair.second, STEP_LEN_TEST_THRESHOLD_PERCENTAGE);
}

BOOST_AUTO_TEST_CASE(stepsXiaomiLong2)
{
  std::string testDir = TEST_DATA_FOLDER;
  std::pair<std::pair<int, double>,
            std::vector<SensorMeasurement> > testData = parseTestData(testDir + "/XiaomiLong2.log");

  std::pair<int, double> trueStepsNumLenPair = testData.first;
  std::pair<int, double> stepsNumLenPair = getStepsNumLenPair(testData.second);
  BOOST_CHECK_CLOSE(static_cast<double>(stepsNumLenPair.first),
                    static_cast<double>(trueStepsNumLenPair.first), STEP_NUM_TEST_THRESHOLD_PERCENTAGE);
  BOOST_CHECK_CLOSE(stepsNumLenPair.second, trueStepsNumLenPair.second, STEP_LEN_TEST_THRESHOLD_PERCENTAGE);
}

BOOST_AUTO_TEST_CASE(stepsXiaomiLong3)
{
  std::string testDir = TEST_DATA_FOLDER;
  std::pair<std::pair<int, double>,
            std::vector<SensorMeasurement> > testData = parseTestData(testDir + "/XiaomiLong3.log");

  std::pair<int, double> trueStepsNumLenPair = testData.first;
  std::pair<int, double> stepsNumLenPair = getStepsNumLenPair(testData.second);
  BOOST_CHECK_CLOSE(static_cast<double>(stepsNumLenPair.first),
                    static_cast<double>(trueStepsNumLenPair.first), STEP_NUM_TEST_THRESHOLD_PERCENTAGE);
  BOOST_CHECK_CLOSE(stepsNumLenPair.second, trueStepsNumLenPair.second, STEP_LEN_TEST_THRESHOLD_PERCENTAGE);
}

BOOST_AUTO_TEST_CASE(stepsXiaomiLong4)
{
  std::string testDir = TEST_DATA_FOLDER;
  std::pair<std::pair<int, double>,
            std::vector<SensorMeasurement> > testData = parseTestData(testDir + "/XiaomiLong4.log");

  std::pair<int, double> trueStepsNumLenPair = testData.first;
  std::pair<int, double> stepsNumLenPair = getStepsNumLenPair(testData.second);
  BOOST_CHECK_CLOSE(static_cast<double>(stepsNumLenPair.first),
                    static_cast<double>(trueStepsNumLenPair.first), STEP_NUM_TEST_THRESHOLD_PERCENTAGE);
  BOOST_CHECK_CLOSE(stepsNumLenPair.second, trueStepsNumLenPair.second, STEP_LEN_TEST_THRESHOLD_PERCENTAGE);
}

