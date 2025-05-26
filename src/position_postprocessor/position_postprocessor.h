#pragma once

#include <navigine/navigation-core/level.h>
#include <navigine/navigation-core/navigation_settings.h>
#include <navigine/navigation-core/position.h>

#include "graph_snapper.h"
#include "position_smoother.h"

namespace navigine::navigation_core {

class PositionPostprocessor
{
public:
    PositionPostprocessor(
        const std::shared_ptr<const Level>& level,
        const PositionEstimatorSettings::MotionModel& mmSettings,
        const PositionEstimatorSettings::Postprocessor& ppSettings);

    std::optional<Position> processPosition(const Position& pos);

private:
    const std::shared_ptr<const Level> level_;
    const bool useSmoothing_;
    const bool useGraphProjection_;
    const bool useProjectionToWhiteArea_;
    const double maxProjectionDistance_;
    const double maxProjectionSpeed_;

    std::unique_ptr<PositionSmoother> positionSmoother_;
    std::unique_ptr<GraphSnapper> graphSnapper_;
};

} // namespace navigine::navigation_core
