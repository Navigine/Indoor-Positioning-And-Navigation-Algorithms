#pragma once

#include <array>
#include <cmath>
#include <ostream>

namespace navigine::navigation_core {

class Vector3d
{
public:
    Vector3d();
    Vector3d(double x, double y, double z);

    bool isNull() const;
    bool isFinite() const;
    bool isBounded(double value) const;

    void setZero();

    double norm() const;
    double squaredNorm() const;
    Vector3d normalized() const;
    Vector3d& normalize();

    bool operator==(const Vector3d& v2) const;
    bool operator!=(const Vector3d& v2) const;

    double dotProduct(const Vector3d& v) const;
    double dotProduct(const double* values) const;

    Vector3d& operator+=(const Vector3d& v2);
    Vector3d& operator-=(const Vector3d& v2);
    Vector3d& operator*=(double multiplier);
    Vector3d& operator/=(double divisor);
    Vector3d& addMul(const Vector3d& v, double multiplier);

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

    static Vector3d zero();
    static Vector3d crossProduct(const Vector3d& v1, const Vector3d& v2);
    static double dotProduct(const Vector3d& v1, const Vector3d& v2);

private:
    std::array<double, 3> data_;
};

inline Vector3d operator*(double multiplier, const Vector3d& v)
{
    return Vector3d(v.at(0) * multiplier, v.at(1) * multiplier, v.at(2) * multiplier);
}

inline Vector3d operator*(const Vector3d& v, double multiplier)
{
    return Vector3d(v.at(0) * multiplier, v.at(1) * multiplier, v.at(2) * multiplier);
}

inline Vector3d operator+(const Vector3d& v1, const Vector3d& v2)
{
    return Vector3d(v1.at(0) + v2.at(0), v1.at(1) + v2.at(1), v1.at(2) + v2.at(2));
}

inline Vector3d operator-(const Vector3d& v1, const Vector3d& v2)
{
    return Vector3d(v1.at(0) - v2.at(0), v1.at(1) - v2.at(1), v1.at(2) - v2.at(2));
}

inline Vector3d operator-(const Vector3d& v)
{
    return Vector3d(-v.at(0), -v.at(1), -v.at(2));
}

inline Vector3d operator/(const Vector3d& v, double divisor)
{
    return Vector3d(v.at(0) / divisor, v.at(1) / divisor, v.at(2) / divisor);
}

std::ostream& operator<<(std::ostream& os, const Vector3d& v);

} // namespace navigine::navigation_core
