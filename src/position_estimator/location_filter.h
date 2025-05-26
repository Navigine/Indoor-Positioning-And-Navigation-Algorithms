#pragma once

#include <navigine/navigation-core/level.h>
#include <navigine/navigation-core/measurement.h>
#include <navigine/navigation-core/utils.h>

#include "sigma_filter.h"

namespace navigine::navigation_core {

class LocationFilter
{
public:
    LocationFilter(
        bool useGps,
        bool useGpsSigmaFilter,
        bool useGpsZoneFilter,
        double gpsMinDeviation,
        double gpsMaxDeviationThreshold,
        double gpsZoneFilterBuffer,
        unsigned int gpsMinNumSatsThreshold,
        unsigned int sigmaFilterWindow,
        double sigmaFilterDelta,
        const std::shared_ptr<const Level>& level);

    bool addMeasurement(const LocationMeasurement& locMsr);
    bool addMeasurement(const NmeaMeasurement& nmeaMsr);

    std::optional<LocationMeasurement> locationMeasurement() const;
    std::optional<NmeaMeasurement> nmeaMeasurement() const;

    void reset();

    std::vector<LocationMeasurementData> extractLocationMeasurements();

private:
    unsigned int getNumSats() const;

private:
    const bool useGps_;
    const bool useGpsSigmaFilter_;
    const bool useGpsZoneFilter_;
    const double gpsMinDeviation_;
    const double gpsMaxDeviationThreshold_;
    const double gpsZoneFilterBuffer_;
    const unsigned int gpsMinNumSatsThreshold_;

    std::unique_ptr<SigmaFilter> sigmaFilter_;
    std::optional<LocationMeasurement> locMsr_;     // Last location measurement
    std::optional<NmeaMeasurement> nmeaMsr_;        // Last NMEA measurement
    std::shared_ptr<const Level> level_;
};

} // namespace navigine::navigation_core
