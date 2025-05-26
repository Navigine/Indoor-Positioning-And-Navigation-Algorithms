#pragma once

#include <navigine/navigation-core/measurement.h>

namespace navigine::navigation_core {

class AltitudeEstimator
{
public:
    AltitudeEstimator();

    void updateLocation(const Timestamp& ts, const LocationMeasurementData& locMsr);
    void updateBarometer(const Timestamp& ts, double baroMsr);

    std::optional<double> getAltitude() const;
    double getDeltaAltitude() const;
    void resetDeltaAltitude();

    void reset();

private:
    void update();

    std::optional<double> calculateDeltaHeightBaro();

    const double updateInterval_;   // seconds

    std::vector<LocationMeasurementData> locMsrs_;
    std::vector<double> baroMsrs_;

    std::optional<double> lastBaro_;

    std::optional<double> altitude_;
    double deltaAltitude_;

    std::optional<Timestamp> updateTs_;
};

} // namespace navigine::navigation_core
