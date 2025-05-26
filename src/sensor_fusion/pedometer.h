#pragma once

#include <navigine/navigation-core/measurement.h>

namespace navigine::navigation_core {

struct Step
{
    Timestamp ts = 0;
    double len = 0.0;
};

struct Magnitude
{
    Timestamp ts = 0;
    double value = 0.0;
};

/// This class detects step of the human on the basis of accelerometer measurements
/// and empirical model of humans gate.
/// Step length is calculated using minimal and maximal accelerations
class Pedometer
{
public:
    void update(const SensorMeasurement& accMsr);
    std::deque<Step> calculateSteps();

private:
    std::optional<Magnitude> calculateFilteredAccMagnitudes() const;

    double calculateStepLength(
        long long timeIntervalMs,
        std::deque<Magnitude>::const_iterator rightBorderIt);

    std::deque<SensorMeasurement> accelMeasurements_;   // Array for saving acceleration measurements
    std::deque<Magnitude> filteredAccMagnitudes_;       // Array for saving filtered acceleration magnitudes
    std::deque<long long> times_;                       // Array for saving step durations

    std::size_t magnitudeSize_ = 0;
    Timestamp possibleStepTs_ = -1;                     // The time when step possibly occurred
    Timestamp stepTime_ = -1;                           // The time when previous step possibly occurred
    bool isStep_ = false;                               // We use this variable to mark possible step
};

} // namespace navigine::navigation_core
