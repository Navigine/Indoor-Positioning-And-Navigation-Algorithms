/** navigation_test.cpp
 *
 * Author: Fedor Puchkov <fedormex@gmail.com>
 * Copyright (c) 2014 Navigine. All rights reserved.
 *
 */ 

#include <navigation_client.h>
#include <navigation_input.h>
#include <navigation_output.h>
#include <navigation_settings.h>
#include <level.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include "../tools/verification/helpers.h"
#include "../include/geolevel.h"

using namespace navigine::navigation_core;

void PrintUsage(const char* program)
{
  printf("Usage:\n"
         "\t%s {MAP_FILE} {LOG_FILE} {SETTINGS_JSON} {NAV_BATCH_SIZE}\n",
         program);
}

void BuildPdrTrack(const double pdrDistance, const double pdrHeading, double& pdrX, double& pdrY)
{
  static double prevPdrDistance = 0.0, prevPdrHeading = 0.0;
  if (pdrDistance != prevPdrDistance && pdrHeading != prevPdrHeading)
  {
    prevPdrDistance = pdrDistance;
    prevPdrHeading  = pdrHeading;
    pdrX += pdrDistance * std::cos(pdrHeading);
    pdrY += pdrDistance * std::sin(pdrHeading);
  }
}

int main(int argc, char** argv)
{
  if (argc < 5)
  {
    PrintUsage(argv[0]);
    return -1;
  }
  
  std::string geojsonFile = argv[1];
  std::string logFile = argv[2];
  std::string settingsFile = argv[3];
  const int navBatchSize = std::stoi(argv[4]); // This variable responsible for how many messages we want to parse and pass to navigation in one batch.

  int errorCode = 0;
  GeoLevels geolevels = ParseGeojson(geojsonFile, errorCode);
  NavigationSettings navProps = CreateSettingsFromJson(settingsFile, errorCode);

  if (errorCode != 0)
  {
    std::cerr << "error parsing json file " << errorCode << std::endl;
    return errorCode;
  }

  std::shared_ptr<LevelCollector> levelCollector = createLevelCollector();
  for (const auto& geolevel: geolevels)
  {
    levelCollector->addGeoLevel(*(geolevel.get()));
  }

  std::shared_ptr<NavigationClient> navClient = createNavigationClient(levelCollector, navProps);

  double pdrX = 0.0, pdrY = 0.0;
  auto navMessages = GetNavMessages(logFile);
  std::vector<std::vector<Measurement>> navBatchMessages;

  for(size_t i = 0; i < navMessages.size(); i += navBatchSize) 
  {
    auto last = std::min(navMessages.size(), i + navBatchSize);
    navBatchMessages.emplace_back(navMessages.begin() + i, navMessages.begin() + last);
  }

  long long firstTs = 0;
  for (const auto& navBatchInput: navBatchMessages)
  {
    if (firstTs == 0) {
        firstTs = navBatchInput[0].ts;
    }
    const std::vector<NavigationOutput> navBatchOutput = navClient->navigate(navBatchInput);
    
    for (std::size_t i = 0; i < navBatchOutput.size(); ++i)
    {
      std::string providerStr;
      switch (navBatchOutput[i].provider)
      {
      case Provider::GNSS:
        providerStr = "GNSS";
        break;
      case Provider::INDOOR:
        providerStr = "Indoor";
        break;
      case Provider::FUSED:
        providerStr = "Fused";
        break;
      case Provider::NONE:
        providerStr = "none";
        break;
      default:
        providerStr = "unknown provider";
      }

      std::cout << std::setprecision(10);
      std::cout << std::fixed;

      long long currentTs = navBatchInput[i].ts;
      std::cout << ((currentTs - firstTs) / 1000) << " "
                << navBatchOutput[i].posLatitude << " "
                << navBatchOutput[i].posLongitude << " "
                << navBatchOutput[i].posAltitude << " "
                << pdrX << " "
                << pdrY << " "
                << navBatchOutput[i].posOrientation << " "
                << navBatchOutput[i].posLevel.value.c_str() << " "
                << navBatchOutput[i].status << std::endl;
    }
  }
  return 0;
}
