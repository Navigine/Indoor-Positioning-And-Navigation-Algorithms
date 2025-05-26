#pragma once

#include <navigine/navigation-core/level.h>
#include <navigine/navigation-core/position.h>

namespace navigine::navigation_core {

class BarrierCalculator
{
public:
    BarrierCalculator(const std::shared_ptr<const Level>& level);

    void updatePosition(const std::optional<Position>& position);

    bool getBarrierHit() const;
    bool getBarrierIntersection() const;

    void reset();

private:
    const std::shared_ptr<const Level> level_;
    std::optional<Position> position_;
    std::optional<Position> prevPosition_;
};

} // namespace navigine::navigation_core
