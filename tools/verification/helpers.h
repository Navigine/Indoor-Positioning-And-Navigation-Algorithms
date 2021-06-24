#ifndef _HELPERS_H_
#define _HELPERS_H_

#include <vector>
#include <navigine/navigation-core/level.h>
#include <navigine/navigation-core/navigation_input.h>
#include <navigine/navigation-core/navigation_settings.h>
#include <navigine/navigation-core/geolevel.h>

struct NavigationPoint
{
  NavigationPoint(){}

  NavigationPoint(long long _timeMs, double _lat, double _lng,
                  double _angle, const navigine::navigation_core::LevelId& _level)
    : timeMs (_timeMs)
    , lat (_lat)
    , lng (_lng)
    , angle (_angle)
    , level (_level)
  { }

  long long timeMs = 0;
  double lat = 0;
  double lng = 0;
  double angle = 0;

  navigine::navigation_core::LevelId level;
};

template <typename Map1, typename Map2>
bool haveEqualKeys(const Map1& lhs, const Map2& rhs)
{
  auto pred = [](decltype(*lhs.begin()) a, decltype(*rhs.begin()) b)
  {
    return a.first == b.first;
  };

  return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), pred);
}

std::vector<navigine::navigation_core::Measurement> GetNavMessages(const std::string& jsonFile);

navigine::navigation_core::GeoLevels ParseGeojson(
  const std::string& jsonFile,
  int& errorCode);

navigine::navigation_core::NavigationSettings CreateSettingsFromJson(
  const std::string& jsonFile,
  int& errorCode);

#endif //_HELPERS_H_
