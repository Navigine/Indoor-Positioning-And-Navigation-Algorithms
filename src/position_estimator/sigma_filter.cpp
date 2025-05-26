#include <navigine/navigation-core/vector3d.h>
#include <navigine/navigation-core/math/math.h>

#include "sigma_filter.h"

namespace navigine::navigation_core {

SigmaFilter::SigmaFilter(unsigned int sigmaWindow, double sigmaDelta)
    : sigmaWindow_(sigmaWindow)
    , sigmaDelta_(sigmaDelta)
{
    if (sigmaWindow_ <= 0) {
        throw std::runtime_error("SigmaFilter: window SHOULD be positive");
    }
}

bool SigmaFilter::addPoint(const GeoPoint& point)
{
    bool isValid = false;

    if (buffer_.size() == sigmaWindow_) {
        // Calculating the mean geo point among the buffer points.
        // Using formulas for converting spherical coordinates to the cartesian and vice versa:
        // https://en.wikipedia.org/wiki/Spherical_coordinate_system
        Vector3d sum;
        for(const auto& p : buffer_) {
            const double theta = M_PI / 2 - DegToRad(p.latitude); // polar angle or colatitude
            const double phi = DegToRad(p.longitude); // azimuthal angle
            sum += Vector3d(
                math::sin(theta) * math::cos(phi),
                math::sin(theta) * math::sin(phi),
                math::cos(theta));
        }

        sum.normalize();
        const double meanTheta = math::acos(std::clamp(sum.at(2), -1.0, 1.0));
        const double meanPhi = math::atan2(sum.at(1), sum.at(0));

        const auto meanPoint = GeoPoint{
            .latitude = RadToDeg(M_PI / 2 - meanTheta),
            .longitude = RadToDeg(meanPhi)};

        double meanSqDeviationX = 0.0;
        double meanSqDeviationY = 0.0;
        for(const auto& p : buffer_) {
            const auto d = GeoToLocal(p, meanPoint);
            meanSqDeviationX += d.x * d.x / sigmaWindow_;
            meanSqDeviationY += d.y * d.y / sigmaWindow_;
        }

        const auto maxDeviationX = 3.0 * math::sqrt(meanSqDeviationX) + sigmaDelta_;
        const auto maxDeviationY = 3.0 * math::sqrt(meanSqDeviationY) + sigmaDelta_;

        const auto d = GeoToLocal(point, meanPoint);

        isValid =
            std::abs(d.x) <= maxDeviationX &&
            std::abs(d.y) <= maxDeviationY;

        buffer_.pop_front();
    }

    buffer_.push_back(point);
    return isValid;
}

void SigmaFilter::reset()
{
    buffer_.clear();
}

} // namespace navigine::navigation_core
