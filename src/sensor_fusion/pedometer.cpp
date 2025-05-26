#include <algorithm>
#include <numeric>
#include <cmath>

#include <navigine/navigation-core/measurement.h>
// #include <navigine/navigation-core/math/math.h>

#include "pedometer.h"

namespace navigine::navigation_core {

namespace {

// Time interval on which we calculate averaged filtered magnitude
// in order to align it relative to zero value (msecs)
const long long AVERAGING_TIME_INTERVAL_MS = 2500;

// Filtered acc measurements are calculated as average over this time interval (msecs)
const long long FILTER_TIME_INTERVAL_MS = 200;

// The time interval on which we update accelMax, Min, threshold (msecs)
const long long UPDATE_TIME_INTERVAL_MS = 700;

// We cut off possible steps if time between them is less then this thresh (msecs)
const long long MIN_TIME_BETWEEN_STEPS_MS = 300;

// Constant using in algorithm which is responsible for step length calculation
const double STEP_LENGTH_CONST = 0.52;

// Empirical threshold values from article p.888
const double MINIMAL_THRESHOLD_VALUE = 0.05 * 9.80665;

// First N steps in which we accumulate data about average step time
const int MINIMAL_NUMBER_OF_STEPS = 5;

// Maximum number of steps to make an assumption about average step time
const int MAXIMUM_NUMBER_OF_STEPS = 50;

// The maximum possible time for step duration
const int MAX_STEP_TIME = 2000;

} // namespace

void Pedometer::update(const SensorMeasurement& msr)
{
    magnitudeSize_ = std::max<size_t>(filteredAccMagnitudes_.size(), 1);

    if (msr.data.type == SensorMeasurementData::Type::ACCELEROMETER) {
        accelMeasurements_.push_back(msr);

        const auto magn = calculateFilteredAccMagnitudes();
        if (magn) {
            filteredAccMagnitudes_.push_back(*magn);
        }

        while (accelMeasurements_.size() > 1 &&
               accelMeasurements_.back().ts - accelMeasurements_.front().ts > 2 * AVERAGING_TIME_INTERVAL_MS)
        {
            accelMeasurements_.pop_front();
        }
    }
}

/// This method calculates and form array with filtered acceleration magnitude
/// sliding window average is used (low pass). This function also updates average
/// filtered magnitude
std::optional<Magnitude> Pedometer::calculateFilteredAccMagnitudes() const
{
    Timestamp lastMeasTs = accelMeasurements_.back().ts;

    // TODO: rewrite using std::lower_bound
    auto lBorderAverIt = accelMeasurements_.begin();
    while (lastMeasTs - lBorderAverIt->ts >= AVERAGING_TIME_INTERVAL_MS) {
        lBorderAverIt++;
    }

    double averMagnitude = std::accumulate(lBorderAverIt, accelMeasurements_.end(), 0.0,
        [](double sum, const SensorMeasurement& msr) {
            return sum + msr.data.values.norm();
        });

    double nMeasAverage = std::distance(lBorderAverIt, accelMeasurements_.end());

    // TODO: rewrite using std::lower_bound
    auto lBorderFilterIt = accelMeasurements_.begin();
    while (lastMeasTs - lBorderFilterIt->ts >= FILTER_TIME_INTERVAL_MS) {
        lBorderFilterIt++;
    }

    double filterMagnitude = std::accumulate(lBorderFilterIt, accelMeasurements_.end(), 0.0,
        [](double sum, const SensorMeasurement& msr) {
            return sum + msr.data.values.norm();
        });

    double nMeasFiltered = std::distance(lBorderFilterIt, accelMeasurements_.end());

    // There are no measurements at acceleration array
    if (nMeasAverage == 0 || nMeasFiltered == 0) {
        return std::nullopt;
    }

    filterMagnitude /= nMeasFiltered;
    averMagnitude += filterMagnitude; // For correct processing at the very beginning
    averMagnitude /= nMeasAverage;
    filterMagnitude -= averMagnitude;

    return Magnitude{
        .ts = lastMeasTs,
        .value = filterMagnitude};
}

std::deque<Step> Pedometer::calculateSteps()
{
    std::deque<Step> steps;
    if (filteredAccMagnitudes_.size() <= 3) {
        // There is no enough accelerometer measurements to detect steps
        return steps;
    }

    Timestamp averageStepTime = 0;
    auto nSteps = std::max<int>(times_.size(), 1);

    if (nSteps >= MINIMAL_NUMBER_OF_STEPS) {
        averageStepTime = std::accumulate(times_.begin(), times_.end(), 0,
            [](long long sum, long long s) {
                return sum + s;
            });
    }

    averageStepTime /= nSteps;

    double timeBetweenSteps = std::max<double>(
        1.0 * MIN_TIME_BETWEEN_STEPS_MS,
        0.6 * averageStepTime);

    for(size_t i = magnitudeSize_; i < filteredAccMagnitudes_.size(); i++) {
        const auto curAcc  = filteredAccMagnitudes_[i];
        const auto prevAcc = filteredAccMagnitudes_[i-1];

        // True if cross threshold and if new detection isn't too early.
        if (!isStep_ &&
            prevAcc.value < MINIMAL_THRESHOLD_VALUE &&
            curAcc.value > MINIMAL_THRESHOLD_VALUE &&
            curAcc.ts - possibleStepTs_ > MIN_TIME_BETWEEN_STEPS_MS &&
            timeBetweenSteps > 0) //  && curAcc.ts - possibleStepTs_ > timeBetweenSteps
        {
            isStep_ = true;
            possibleStepTs_ = curAcc.ts;
        }

        if (isStep_) {
            double stepLength = calculateStepLength(UPDATE_TIME_INTERVAL_MS, filteredAccMagnitudes_.begin() + i);
            if (possibleStepTs_ - stepTime_ < MAX_STEP_TIME) {
                times_.push_back(possibleStepTs_ - stepTime_);
            }

            steps.push_back(Step{
                .ts = possibleStepTs_,
                .len = stepLength});

            stepTime_ = possibleStepTs_;
            isStep_ = false; // prepare to detect new step
        }
    }

    while (filteredAccMagnitudes_.size() > 1 &&
           filteredAccMagnitudes_.back().ts - filteredAccMagnitudes_.front().ts > 2 * UPDATE_TIME_INTERVAL_MS)
    {
        filteredAccMagnitudes_.pop_front();
    }

    while (times_.size() > MAXIMUM_NUMBER_OF_STEPS) {
        times_.pop_front();
    }

    return steps;
}

double Pedometer::calculateStepLength(
    long long timeIntervalMs,
    std::deque<Magnitude>::const_iterator rightBorderIt)
{
    // find left border
    // TODO: rewrite using std::lower_bound
    auto leftBorderIt = rightBorderIt;
    while (rightBorderIt->ts - leftBorderIt->ts <= timeIntervalMs && leftBorderIt != filteredAccMagnitudes_.begin()) {
        leftBorderIt--;
    }

    const auto maxMagn = std::max_element(leftBorderIt, rightBorderIt,
        [](Magnitude m1, Magnitude m2) {
            return m1.value < m2.value;
        })->value;

    const auto minMagn = std::min_element(leftBorderIt, rightBorderIt,
        [](Magnitude m1, Magnitude m2) {
            return m1.value < m2.value;
        })->value;

    const auto accAmplitude = maxMagn - minMagn;
    const auto stepLen = STEP_LENGTH_CONST * std::pow(accAmplitude, 0.25);
    return stepLen;
}

} // namespace navigine::navigation_core
