#include <cmath>

#include <navigine/navigation-core/geometry.h>
#include <navigine/navigation-core/graph.h>

#include "position_postprocessor.h"
#include "position_smoother.h"

namespace navigine::navigation_core {

PositionPostprocessor::PositionPostprocessor(
    const std::shared_ptr<const Level>& level,
    const PositionEstimatorSettings::MotionModel& mmSettings,
    const PositionEstimatorSettings::Postprocessor& ppSettings)
    : level_(level)
    , useSmoothing_(ppSettings.useSmoothing.value_or(PositionEstimatorSettings::Postprocessor::DEFAULT_USE_SMOOTHING))
    , useGraphProjection_(ppSettings.useGraphProjection.value_or(PositionEstimatorSettings::Postprocessor::DEFAULT_USE_GRAPH_PROJECTION))
    , useProjectionToWhiteArea_(ppSettings.useProjectionToWhiteArea.value_or(PositionEstimatorSettings::Postprocessor::DEFAULT_USE_PROJECTION_TO_WHITE_AREA))
    , maxProjectionDistance_(ppSettings.maxProjectionDistance.value_or(PositionEstimatorSettings::Postprocessor::DEFAULT_MAX_PROJECTION_DISTANCE))
    , maxProjectionSpeed_(ppSettings.maxProjectionSpeed.value_or(PositionEstimatorSettings::Postprocessor::DEFAULT_MAX_PROJECTION_SPEED))
    , positionSmoother_(std::make_unique<PositionSmoother>(mmSettings, ppSettings))
    , graphSnapper_(useGraphProjection_ && level_ ?
        std::make_unique<GraphSnapper>(level_->graph(), maxProjectionDistance_, maxProjectionSpeed_) :
        nullptr)
{
}

std::optional<Position> PositionPostprocessor::processPosition(const Position& position)
{
    auto result = useSmoothing_ ?
        positionSmoother_->smoothPosition(position) :
        position;

    if (!level_ || !result.levelId || *result.levelId != level_->id()) {
        return result;
    }

    // Projecting position to level geometry
    if (useProjectionToWhiteArea_) {
        const auto levelPoint = GeoToLocal(result.point, level_->bindingPoint());
        const auto projection = GetProjection(level_->geometry(), levelPoint);

        if (projection.distance < maxProjectionDistance_) {
            result.point = LocalToGeo(projection.point, level_->bindingPoint());
        }
    }

    // Projecting position to level graph (if necessary)
    if (useGraphProjection_) {
        auto origPoint = GeoToLocal(result.point, level_->bindingPoint());
        auto projPoint = graphSnapper_->addPoint(origPoint, position.timestamp);
        result.point = LocalToGeo(projPoint, level_->bindingPoint());
    }

    return result;
}

} // namespace navigine::navigation_core
