#pragma once

#include <array>
#include <cmath>
#include <ostream>

namespace navigine::navigation_core {

class Vector4d
{
public:
    Vector4d();
    Vector4d(double x, double y, double z, double w);

    bool isNull() const;
    bool isFinite() const;
    bool isBounded(double value) const;

    void setZero();

    double norm() const;
    double squaredNorm() const;
    Vector4d normalized() const;
    Vector4d& normalize();

    double dotProduct(const Vector4d& v) const;
    double dotProduct(const double* values) const;

    bool operator==(const Vector4d& v) const;
    bool operator!=(const Vector4d& v) const;

    Vector4d& operator+=(const Vector4d& v);
    Vector4d& operator-=(const Vector4d& v);
    Vector4d& operator*=(double multiplier);
    Vector4d& operator/=(double divisor);
    Vector4d& addMul(const Vector4d& v, double multiplier);

    inline double& at(unsigned int index) {
        return data_[index];
    }

    inline double at(unsigned int index) const {
        return data_[index];
    }

    inline double* data() {
        return data_.data();
    }

    inline const double* data() const {
        return data_.data();
    }

    static Vector4d zero();
    static double dotProduct(const Vector4d& v1, const Vector4d& v2);

private:
    std::array<double, 4> data_;
};

inline Vector4d operator*(double multiplier, const Vector4d& v)
{
    return Vector4d(
        v.at(0) * multiplier,
        v.at(1) * multiplier,
        v.at(2) * multiplier,
        v.at(3) * multiplier);
}

inline Vector4d operator*(const Vector4d& v, double multiplier)
{
    return Vector4d(
        v.at(0) * multiplier,
        v.at(1) * multiplier,
        v.at(2) * multiplier,
        v.at(3) * multiplier);
}

inline Vector4d operator+(const Vector4d& v1, const Vector4d& v2)
{
    return Vector4d(
        v1.at(0) + v2.at(0),
        v1.at(1) + v2.at(1),
        v1.at(2) + v2.at(2),
        v1.at(3) + v2.at(3));
}

inline Vector4d operator-(const Vector4d& v1, const Vector4d& v2)
{
    return Vector4d(
        v1.at(0) - v2.at(0),
        v1.at(1) - v2.at(1),
        v1.at(2) - v2.at(2),
        v1.at(3) - v2.at(3));
}

inline Vector4d operator-(const Vector4d& v)
{
    return Vector4d(-v.at(0), -v.at(1), -v.at(2), -v.at(3));
}

inline Vector4d operator/(const Vector4d& v, double divisor)
{
    return Vector4d(
        v.at(0) / divisor,
        v.at(1) / divisor,
        v.at(2) / divisor,
        v.at(3) / divisor);
}

std::ostream& operator<<(std::ostream& os, const Vector4d& v);

} // namespace navigine::navigation_core
