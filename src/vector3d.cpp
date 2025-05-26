#include <cmath>
#include <limits>

// #include <navigine/navigation_core/math/math.h>
#include <navigine/navigation-core/vector3d.h>
#include <navigine/navigation-core/utils.h>

namespace navigine::navigation_core {

Vector3d::Vector3d()
{
    setZero();
}

Vector3d::Vector3d(double x, double y, double z)
{
    data_[0] = x;
    data_[1] = y;
    data_[2] = z;
}

void Vector3d::setZero()
{
    data_[0] = data_[1] = data_[2] = 0.0;
}

double Vector3d::norm() const
{
    return std::sqrt(squaredNorm());
}

double Vector3d::squaredNorm() const
{
    return data_[0] * data_[0] + data_[1] * data_[1] + data_[2] * data_[2];
}

Vector3d Vector3d::normalized() const
{
    const auto length = norm();
    if (length > std::numeric_limits<double>::epsilon()) {
        return Vector3d(
            data_[0] / length,
            data_[1] / length,
            data_[2] / length);
    }
    return *this;
}

Vector3d& Vector3d::normalize()
{
    const auto length = norm();
    if (length > std::numeric_limits<double>::epsilon()) {
        data_[0] /= length;
        data_[1] /= length;
        data_[2] /= length;
    }
    return *this;
}

bool Vector3d::isNull() const
{
    return data_[0] == 0.0 &&
           data_[1] == 0.0 &&
           data_[2] == 0.0;
}

bool Vector3d::isFinite() const
{
    return std::isfinite(data_[0]) &&
           std::isfinite(data_[1]) &&
           std::isfinite(data_[2]);
}

bool Vector3d::isBounded(double value) const
{
    return std::isfinite(data_[0]) && std::fabs(data_[0]) <= value &&
           std::isfinite(data_[1]) && std::fabs(data_[1]) <= value &&
           std::isfinite(data_[2]) && std::fabs(data_[2]) <= value;
}

double Vector3d::dotProduct(const Vector3d& v) const
{
    return data_[0] * v.data_[0] + data_[1] * v.data_[1] + data_[2] * v.data_[2];
}

double Vector3d::dotProduct(const double* values) const
{
    return data_[0] * values[0] + data_[1] * values[1] + data_[2] * values[2];
}

bool Vector3d::operator==(const Vector3d& v) const
{
    return data_[0] == v.data_[0] &&
           data_[1] == v.data_[1] &&
           data_[2] == v.data_[2];
}

bool Vector3d::operator!=(const Vector3d& v) const
{
    return data_[0] != v.data_[0] ||
           data_[1] != v.data_[1] ||
           data_[2] != v.data_[2];
}

Vector3d& Vector3d::operator+=(const Vector3d& v)
{
    data_[0] += v.data_[0];
    data_[1] += v.data_[1];
    data_[2] += v.data_[2];
    return *this;
}

Vector3d& Vector3d::operator-=(const Vector3d& v)
{
    data_[0] -= v.data_[0];
    data_[1] -= v.data_[1];
    data_[2] -= v.data_[2];
    return *this;
}

Vector3d& Vector3d::operator*=(double multiplier)
{
    data_[0] *= multiplier;
    data_[1] *= multiplier;
    data_[2] *= multiplier;
    return *this;
}

Vector3d& Vector3d::operator/=(double divisor)
{
    data_[0] /= divisor;
    data_[1] /= divisor;
    data_[2] /= divisor;
    return *this;
}

Vector3d& Vector3d::addMul(const Vector3d& v, double multiplier)
{
    data_[0] += v.data_[0] * multiplier;
    data_[1] += v.data_[1] * multiplier;
    data_[2] += v.data_[2] * multiplier;
    return *this;
}

Vector3d Vector3d::zero()
{
    return Vector3d(0, 0, 0);
}

Vector3d Vector3d::crossProduct(const Vector3d& v1, const Vector3d& v2)
{
    return Vector3d(
        v1.at(1) * v2.at(2) - v1.at(2) * v2.at(1),
        v1.at(2) * v2.at(0) - v1.at(0) * v2.at(2),
        v1.at(0) * v2.at(1) - v1.at(1) * v2.at(0));
}

double Vector3d::dotProduct(const Vector3d& v1, const Vector3d& v2)
{
    return v1.at(0) * v2.at(0) + v1.at(1) * v2.at(1) + v1.at(2) * v2.at(2);
}

std::ostream& operator<<(std::ostream& os, const Vector3d& v)
{
    os << "[" << v.at(0) << ", " << v.at(1) << ", " << v.at(2) << "]";
    return os;
}

} // namespace navigine::navigation_core
