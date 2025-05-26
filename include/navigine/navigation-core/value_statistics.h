#pragma once

#include <navigine/navigation-core/utils.h>

namespace navigine::navigation_core {

class ValueStatistics
{
public:
    ValueStatistics();

    void addValue(std::optional<double> value);

    unsigned int count() const;

    std::optional<double> average() const;

    std::optional<double> deviation() const;

    ValueStatistics& operator+=(const ValueStatistics& other);

    void reset();

private:
    unsigned int valueCount_;
    double valueSum_;
    double valueSumSq_;
};

} // namespace navigine::navigation_core
