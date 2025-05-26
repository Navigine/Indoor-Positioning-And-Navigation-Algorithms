#pragma once

#include <navigine/navigation-core/geometry.h>

namespace navigine::navigation_core {

class SigmaFilter
{
public:
    SigmaFilter(unsigned int sigmaWindow, double sigmaDelta);

    bool addPoint(const GeoPoint& point);
    void reset();

private:
    const unsigned int sigmaWindow_;
    const double sigmaDelta_;
    std::deque<GeoPoint> buffer_;
};

} // namespace navigine::navigation_core
