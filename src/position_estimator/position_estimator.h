#ifndef POSITION_ESTIMATOR_H
#define POSITION_ESTIMATOR_H

#include <navigation_output.h>
#include <navigation_input.h>
#include <navigation_settings.h>
#include <level_collector.h>

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
  { }

  virtual Position calculatePosition(
    const Level& level,
    long long ts,
    const RadioMeasurementsData& radioMsr,
    NavigationStatus& retStatus) = 0;

  virtual void reInit() {};

protected:
  std::shared_ptr<LevelCollector> mLevelIndex;
  Position mPosition;
};

} } // namespace navigine::navigation_core

#endif // POSITION_ESTIMATOR_H
