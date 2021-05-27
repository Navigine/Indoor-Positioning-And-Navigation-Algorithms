#pragma once

#include "navigation_input.h"
#include "navigation_output.h"
#include "navigation_settings.h"
#include "level_collector.h"

namespace navigine {
namespace navigation_core {

class NavigationClient
{
  public:
    // Main navigation function. Calculates current position based on incoming measurements
    virtual std::vector<NavigationOutput> navigate(const std::vector<Measurement>& navInput) = 0;

    virtual ~NavigationClient() {}
};

std::shared_ptr<NavigationClient> createNavigationClient(
  const std::shared_ptr<LevelCollector>& levelCollector,
  const NavigationSettings& navProps);

} } // namespace navigine::navigation_core
