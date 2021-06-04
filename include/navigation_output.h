#ifndef NAVIGINE_NAVIGATION_OUTPUT_H
#define NAVIGINE_NAVIGATION_OUTPUT_H

#include <string>
#include <vector>

#include "level.h"

namespace navigine {
namespace navigation_core {

enum NavigationStatus : int
{
  OK                   = 0,
  NAVIGATION_ERROR     = -1,
  NO_SOLUTION          = 4,
  NO_RPS               = 7,
  NO_LEVEL             = 30,
  PF_MUTATION_FAILED   = 32,
  PF_SAMPLING_FAILED   = 33,
};

enum class Provider { GNSS, INDOOR, FUSED, NONE };

struct NavigationOutput
{
  NavigationStatus status;

  LevelId posLevel;       // Level identifier
  double posLatitude = 0.0;
  double posLongitude = 0.0;
  double posAltitude = 0.0;
  double posR;           // Output radius (meters)
  double posOrientation; // Output orientation in radians (clockwise)
  Provider provider;
};

} } // namespace navigine::navigation_core

#endif // NAVIGINE_NAVIGATION_OUTPUT_H
