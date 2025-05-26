#include "barrier_calculator.h"

namespace navigine::navigation_core {

BarrierCalculator::BarrierCalculator(const std::shared_ptr<const Level>& level)
    : level_(level)
{}

void BarrierCalculator::updatePosition(const std::optional<Position>& position)
{
    prevPosition_ = position_;
    position_ = position;
}

bool BarrierCalculator::getBarrierHit() const
{
    return level_ && position_ && !level_->containsGeoPoint(position_->point);
}

bool BarrierCalculator::getBarrierIntersection() const
{
    bool res = level_ && position_ && prevPosition_ &&
        position_->point != prevPosition_->point &&
        level_->containsGeoPoint(prevPosition_->point) &&
        !level_->containsGeoSegment({prevPosition_->point, position_->point});

    return res;
}

void BarrierCalculator::reset()
{
    position_ = prevPosition_ = std::nullopt;
}

} // namespace navigine::navigation_core
