#pragma once

#include <cstdint>

#include <navigine/navigation-core/utils.h>

namespace navigine::navigation_core {

class Rng
{
public:
    using result_type = uint32_t;

    static constexpr uint32_t min() {
        return 1;
    }

    static constexpr uint32_t max() {
        return 0x7FFFFFFF;
    }

    Rng();
    Rng(uint32_t seed);

    //  Binary Bernoulli distribution:
    //  P{true} = p
    //  P{false} = 1 - p
    bool binary(double p);

    int discrete(int min, int max);

    double uniform(double min, double max);

    double normal(double mean, double deviation);

    uint32_t operator()();

private:
    uint32_t rand();

    uint32_t seed_;
};

} // namespace navigine::navigation_core
