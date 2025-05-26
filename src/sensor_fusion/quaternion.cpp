#include <cmath>
#include <limits>

#include <navigine/navigation-core/geometry.h>
#include "quaternion.h"
// #include <navigine/navigation-core/math/math.h>

namespace navigine::navigation_core {

Quaternion::Quaternion()
    : w(0), x(0), y(0), z(0)
{}

Quaternion::Quaternion(double w, double x, double y, double z)
    : w(w), x(x), y(y), z(z)
{}

Quaternion::Quaternion(const Vector3d& v)
    : w(0), x(v.at(0)), y(v.at(1)), z(v.at(2))
{}

Quaternion::Quaternion(const Vector4d& v)
    : w(v.at(0)), x(v.at(1)), y(v.at(2)), z(v.at(3))
{}

Quaternion Quaternion::conj() const
{
    return Quaternion(w, -x, -y, -z);
}

Vector3d Quaternion::toVector3d() const
{
    return Vector3d(x, y, z);
}

Vector4d Quaternion::toVector4d() const
{
    return Vector4d(w, x, y, z);
}

Quaternion Quaternion::normalized() const
{
    return Quaternion(*this).normalize();
}

Quaternion& Quaternion::normalize()
{
    const auto norm = math::sqrt(w * w + x * x + y * y + z * z);

    if (norm > std::numeric_limits<double>::epsilon()) {
        w /= norm;
        x /= norm;
        y /= norm;
        z /= norm;
    }

    return *this;
}

Orientation Quaternion::toOrientation() const
{
    const auto pitch = math::atan2(2 * (y * z + w * x),
        math::sqrt(math::pow(2 * (x * z - w * y), 2) + math::pow((w * w - x * x - y * y + z * z), 2)));

    const auto roll = -math::atan2(2 * (x * z - w * y), (w * w - x * x - y * y + z * z));
    const auto heading = math::atan2(2 * (x * y - w * z), (w * w - x * x + y * y - z * z));

    return Orientation{
        .heading = NormalizedAngle(heading, 0, 2 * M_PI),
        .pitch = pitch,
        .roll = roll};
}

Quaternion Quaternion::fromOrientation(const Orientation& orient)
{
    const auto cosRoll = math::cos(orient.roll / 2);
    const auto sinRoll = math::sin(orient.roll / 2);
    const auto cosPitch = math::cos(orient.pitch / 2);
    const auto sinPitch = math::sin(orient.pitch / 2);
    const auto cosHeading = math::cos(orient.heading / 2);
    const auto sinHeading = math::sin(orient.heading / 2);

    return Quaternion(
        cosRoll * cosPitch * cosHeading + sinRoll * sinPitch * sinHeading,
        cosRoll * sinPitch * cosHeading + sinRoll * cosPitch * sinHeading,
        sinRoll * cosPitch * cosHeading - cosRoll * sinPitch * sinHeading,
        sinRoll * sinPitch * cosHeading - cosRoll * cosPitch * sinHeading);
}

Matrix3d Quaternion::rotationMatrix() const
{
    // Implementation taken from:
    // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

    Matrix3d rotationMatrix;
    rotationMatrix.at(0, 0) = w * w + x * x - y * y - z * z;
    rotationMatrix.at(0, 1) = 2 * (x * y - w * z);
    rotationMatrix.at(0, 2) = 2 * (w * y + x * z);

    rotationMatrix.at(1, 0) = 2 * (x * y + w * z);
    rotationMatrix.at(1, 1) = w * w - x * x + y * y - z * z;
    rotationMatrix.at(1, 2) = 2 * (y * z - w * x);

    rotationMatrix.at(2, 0) = 2 * (x * z - w * y);
    rotationMatrix.at(2, 1) = 2 * (w * x + y * z);
    rotationMatrix.at(2, 2) = w * w - x * x - y * y + z * z;

    return rotationMatrix;
}

Quaternion& Quaternion::operator+=(const Quaternion& other)
{
    w += other.w;
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Quaternion& Quaternion::operator-=(const Quaternion& other)
{
    w -= other.w;
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Quaternion& Quaternion::operator*=(const Quaternion& other)
{
    double qw = w, qx = x, qy = y, qz = z;
    w = qw * other.w - qx * other.x - qy * other.y - qz * other.z;
    x = qw * other.x + qx * other.w + qy * other.z - qz * other.y;
    y = qw * other.y - qx * other.z + qy * other.w + qz * other.x;
    z = qw * other.z + qx * other.y - qy * other.x + qz * other.w;
    return *this;
}

Quaternion& Quaternion::operator*=(double scale)
{
    w *= scale;
    x *= scale;
    y *= scale;
    z *= scale;
    return *this;
}

Quaternion operator+(const Quaternion& q1, const Quaternion& q2)
{
    return Quaternion(q1) += q2;
}

Quaternion operator-(const Quaternion& q1, const Quaternion& q2)
{
    return Quaternion(q1) -= q2;
}

Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
{
    return Quaternion(q1) *= q2;
}

Quaternion operator*(const Quaternion& q1, double scale)
{
    return Quaternion(q1) *= scale;
}

Quaternion operator*(double scale, const Quaternion& q2)
{
    return Quaternion(q2) *= scale;
}

} // namespace navigine::navigation_core
