#pragma once

#include <cmath>

namespace navigine::navigation_core::math {

const double PI = 3.14159265358979323846;
const double PI_2 = 1.57079632679489661923;
const double PI_4 = 0.785398163397448309616;

const double E = 2.71828182845904523536;

const double LN2 = 0.693147180559945309417;
const double LN10 = 2.30258509299404568402;
const double LOG2E = 1.44269504088896340736;

const double SQRT2 = 1.41421356237309504880;
const double SQRT1_2 = 0.707106781186547524401;

double sin(double x);
double cos(double x);
double tan(double x);

double asin(double x);
double acos(double x);
double atan(double x);
double atan2(double y, double x);

double pow(double x, double y);

double log(double x);

inline double sqrt(double x)
{
    return std::sqrt(x);
}

double exp(double x);

} // namespace navigine::navigation_core