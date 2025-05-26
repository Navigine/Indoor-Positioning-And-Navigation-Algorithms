#pragma once

#include "util.h"
#include "type_checks.h"

namespace GEOLIB_NAMESPACE::detail {

template <typename Point>
double precisionX()
{
    static_assert(isPoint<Point>());

    int exponent = 0;
    std::frexp(util::maxX<Point>(), &exponent);
    return std::exp2(exponent - 53);
}

template <typename Point>
double precisionY()
{
    static_assert(isPoint<Point>());

    int exponent = 0;
    std::frexp(util::maxY<Point>(), &exponent);
    return std::exp2(exponent - 53);
}

} // namespace GEOLIB_NAMESPACE::detail
