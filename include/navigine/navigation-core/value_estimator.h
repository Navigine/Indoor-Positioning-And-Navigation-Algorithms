#pragma once

#include <vector>

#include <navigine/navigation-core/utils.h>

namespace navigine::navigation_core {

// Value estimator allows to calculate the average value for some
// variable within the certain period of time. It is implemented as a cyclic
// buffer of "epoch accumulators" --- items which store variable values within
// some short periods of time ("epochs").
//
// Every time when the variable is changed, its value should be passed to the
// estimator, which puts it to the current epoch's counter.
//
// Every time when the average variable value is required, estimator calculates
// the weighted sum of values over the epoch counters (ignoring too old epochs).
// The counters' weights decrease exponentially with the specified attenuation
// factor:
//   weight_0 = attenuationFactor^0 (current epoch)
//   weight_1 = attenuationFactor^1 (previous epoch)
//   weight_2 = attenuationFactor^2 (pre-previous epoch)
//   ...

class ValueEstimator
{
public:
    ValueEstimator(
        unsigned int numberOfEpochs = 30,
        int64_t epochLengthMs = 1000);

    void addValue(double value, Timestamp timestamp);

    double getCurrentRate(Timestamp timestamp) const;
    double getCurrentValue(Timestamp timestamp) const;
    double getCurrentSum(Timestamp timestamp, double attenuationFactor = 1.0) const;

    void reset();

private:
    struct EpochAccumulator {
        int64_t epoch = 0LL; // Epoch number
        double value  = 0.0; // Accumulated epoch value
        size_t count = 0;
    };

    // sum, count, deltatime
    std::tuple<double, size_t, double> getCurrentCounters(Timestamp timestamp, double attenuationFactor) const;

    const unsigned int numberOfEpochs_;
    const int64_t epochLengthMs_;
    std::vector<EpochAccumulator> epochAccumulators_;
};

} // namespace navigine::navigation_core
