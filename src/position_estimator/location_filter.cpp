#include "location_filter.h"

namespace navigine::navigation_core {

LocationFilter::LocationFilter(
    bool useGps,
    bool useGpsSigmaFilter,
    bool useGpsZoneFilter,
    double gpsMinDeviation,
    double gpsMaxDeviationThreshold,
    double gpsZoneFilterBuffer,
    unsigned int gpsMinNumSatsThreshold,
    unsigned int sigmaFilterWindow,
    double sigmaFilterDelta,
    const std::shared_ptr<const Level>& level)
    : useGps_(useGps)
    , useGpsSigmaFilter_(useGpsSigmaFilter)
    , useGpsZoneFilter_(useGpsZoneFilter)
    , gpsMinDeviation_(gpsMinDeviation)
    , gpsMaxDeviationThreshold_(gpsMaxDeviationThreshold)
    , gpsZoneFilterBuffer_(gpsZoneFilterBuffer)
    , gpsMinNumSatsThreshold_(gpsMinNumSatsThreshold)
    , sigmaFilter_(std::make_unique<SigmaFilter>(sigmaFilterWindow, sigmaFilterDelta))
    , level_(level)
{}

bool LocationFilter::addMeasurement(const LocationMeasurement& locMsr)
{
    if (!useGps_) {
        return false;
    }

    // Filtering out invalid GPS measurements
    if (std::fabs(locMsr.data.point.latitude) > 90.0 ||
        std::fabs(locMsr.data.point.longitude) > 180.0) {
        return false;
    }

    const auto isGpsProvider = isEqualCaseInsensitive(locMsr.data.provider, "gps");

    // TODO: filter out by provider [?]

    // Filtering out GPS measurement by the gpsMaxDeviationThreshold
    if (locMsr.data.accuracy > gpsMaxDeviationThreshold_ && isGpsProvider) {
        return false;
    }

    if (useGpsZoneFilter_ && level_) {
        const auto xyPoint = GeoToLocal(locMsr.data.point, level_->bindingPoint());
        const auto projection = GetProjection(level_->geometry(), xyPoint);
        if (projection.distance > gpsZoneFilterBuffer_) {
            return false;
        }
    }

    // Filtering out GPS measurement by the gpsMinNumSatsThreshold
    const auto numSats = getNumSats();
    if (0 < numSats && numSats < gpsMinNumSatsThreshold_ && isGpsProvider) {
        return false;
    }

    // Filtering out GPS measurement by sigma filter constraint
    if (useGpsSigmaFilter_ && !sigmaFilter_->addPoint(locMsr.data.point)) {
        return false;
    }

    locMsr_ = locMsr;
    locMsr_->data.accuracy = std::max(locMsr.data.accuracy, gpsMinDeviation_);

    return true;
}

bool LocationFilter::addMeasurement(const NmeaMeasurement& nmeaMsr)
{
    if (!useGps_) {
        return false;
    }

    nmeaMsr_ = nmeaMsr;
    return true;
}

std::optional<LocationMeasurement> LocationFilter::locationMeasurement() const
{
    return locMsr_;
}

std::optional<NmeaMeasurement> LocationFilter::nmeaMeasurement() const
{
    return nmeaMsr_;
}

void LocationFilter::reset()
{
    sigmaFilter_->reset();
    locMsr_ = std::nullopt;
    nmeaMsr_ = std::nullopt;
}

std::vector<LocationMeasurementData> LocationFilter::extractLocationMeasurements()
{
    if (!locMsr_) {
        return {};
    }

    auto result = {locMsr_->data};

    locMsr_ = std::nullopt;

    return result;
}

unsigned int LocationFilter::getNumSats() const
{
    return nmeaMsr_ ? nmeaMsr_->data.satellitesNumber : 0;
}

} // namespace navigine::navigation_core
