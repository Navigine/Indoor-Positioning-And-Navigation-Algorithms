#pragma once

#include <array>
#include <cmath>
#include <ostream>

namespace navigine::navigation_core {

struct XYPoint;

class Vector2d
{
public:
    Vector2d();
    Vector2d(double x, double y);
    Vector2d(const XYPoint& point);

    bool isNull() const;
    bool isFinite() const;
    bool isBounded(double value) const;

    void setZero();

    double norm() const;
    double squaredNorm() const;
    Vector2d normalized() const;
    Vector2d& normalize();

    double dotProduct(const Vector2d& v) const;
    double dotProduct(const double* values) const;

    bool operator==(const Vector2d& v) const;
    bool operator!=(const Vector2d& v) const;

    Vector2d& operator+=(const Vector2d& v);
    Vector2d& operator-=(const Vector2d& v);
    Vector2d& operator*=(double multiplier);
    Vector2d& operator/=(double divisor);
    Vector2d& addMul(const Vector2d& v, double multiplier);

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

    static Vector2d zero();
    static double dotProduct(const Vector2d& v1, const Vector2d& v2);

private:
    std::array<double, 2> data_;
};

inline Vector2d operator*(double multiplier, const Vector2d& v)
{
    return Vector2d(v.at(0) * multiplier, v.at(1) * multiplier);
}

inline Vector2d operator*(const Vector2d& v, double multiplier)
{
    return Vector2d(v.at(0) * multiplier, v.at(1) * multiplier);
}

inline Vector2d operator+(const Vector2d& v1, const Vector2d& v2)
{
    return Vector2d(v1.at(0) + v2.at(0), v1.at(1) + v2.at(1));
}

inline Vector2d operator-(const Vector2d& v1, const Vector2d& v2)
{
    return Vector2d(v1.at(0) - v2.at(0), v1.at(1) - v2.at(1));
}

inline Vector2d operator-(const Vector2d& v)
{
    return Vector2d(-v.at(0), -v.at(1));
}

inline Vector2d operator/(const Vector2d& v, double divisor)
{
    return Vector2d(v.at(0) / divisor, v.at(1) / divisor);
}

std::ostream& operator<<(std::ostream& os, const Vector2d& v);

} // namespace navigine::navigation_core
