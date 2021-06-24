#ifndef NAVIGINE_POSITION_SMOOTHER_H
#define NAVIGINE_POSITION_SMOOTHER_H

#include <navigine/navigation-core/navigation_settings.h>
#include <navigine/navigation-core/level.h>
#include "../position.h"

namespace navigine {
namespace navigation_core {

class PositionSmoother
{
public:
    virtual ~PositionSmoother() {}
    virtual Position smoothPosition(Position pos, const Level& level) = 0;
    virtual void reset(Position pos) = 0;
};

} } // namespace navigine::navigation_core

#endif // NAVIGINE_POSITION_SMOOTHER_H
