#ifndef POSITION_ESTIMATOR_H
#define POSITION_ESTIMATOR_H

#include <navigine/navigation-core/navigation_output.h>
#include <navigine/navigation-core/navigation_input.h>
#include <navigine/navigation-core/navigation_state.h>
#include <navigine/navigation-core/navigation_settings.h>
#include <navigine/navigation-core/level_collector.h>
#include <navigine/navigation-core/motion_info.h>

#include "../position.h"

namespace navigine {
namespace navigation_core {

class PositionEstimator
{
public:
  virtual ~PositionEstimator() = default;

  PositionEstimator(const std::shared_ptr<LevelCollector>& levelCollector)
    : mLevelIndex(levelCollector)
    , mPosition(Position())
    , mParticleFilterState(NavigationState())
  { }

  virtual Position calculatePosition(
    const Level& level,
    long long ts,
    const RadioMeasurementsData& radioMsr,
    const MotionInfo& motionInfo,
    NavigationStatus& retStatus) = 0;

  virtual void reInit() {};

  NavigationState getParticleFilterState() const { return mParticleFilterState; }

protected:
  std::shared_ptr<LevelCollector> mLevelIndex;
  Position mPosition;
  NavigationState mParticleFilterState;
};

} } // namespace navigine::navigation_core

#endif // POSITION_ESTIMATOR_H
