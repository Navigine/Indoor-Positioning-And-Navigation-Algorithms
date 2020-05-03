#pragma once

#include <string>
#include <vector>

namespace navigine {
namespace navigation_core {

struct Point2D
{
  Point2D()
      : x(0.0)
      , y(0.0) {}

  Point2D(double _x, double _y)
      : x(_x)
      , y(_y) {}

  double x;
  double y;
};

struct Position
{
  double x         = 0.0;
  double y         = 0.0;
  double precision = 0.0;
  bool isEmpty     = true;
  long long ts     = -1;
};

struct RadioMeasurement
{
  enum class Type {WIFI, BEACON};

  Type            type;    
  long long       ts          = 0;
  std::string     id          = "";   // Entry id (bssid, mac, major+minor+uuid)
  double          rssi        = 0;
  double          power       = 0;
  double          frequency   = 0;
  double          distance    = 0;
  double          stddev      = 0;
};

struct NavigationInput
{
  int         packetNumber = 0;      // Message number
  std::string deviceId     = "";     // Device identifier (MAC address/id/...)
  std::string checkPoint   = "";
  long long   timeStamp    = 0;
  
  std::vector<RadioMeasurement>  signalEntries   = {};
};

using RadioMeasurements = std::vector<RadioMeasurement>;

} } // namespace navigine::navigation_core
