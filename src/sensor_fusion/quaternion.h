#pragma once

#include <tuple>

#include <navigine/navigation-core/matrix3d.h>
#include <navigine/navigation-core/orientation.h>
#include <navigine/navigation-core/vector3d.h>
#include <navigine/navigation-core/vector4d.h>

namespace navigine::navigation_core {

class Quaternion
{
public:
    double w;
    double x;
    double y;
    double z;

public:
    Quaternion();
    Quaternion(double w, double x, double y, double z);
    Quaternion(const Vector3d& v);
    Quaternion(const Vector4d& v);

    Quaternion normalized() const;
    Quaternion& normalize();

    Quaternion conj() const;

    Vector3d toVector3d() const;
    Vector4d toVector4d() const;

    Orientation toOrientation() const;

    Matrix3d rotationMatrix() const;

    static Quaternion fromOrientation(const Orientation& orient);

    Quaternion& operator+=(const Quaternion& other);
    Quaternion& operator-=(const Quaternion& other);
    Quaternion& operator*=(const Quaternion& other);
    Quaternion& operator*=(double scale);
};

Quaternion operator+(const Quaternion& q1, const Quaternion& q2);
Quaternion operator-(const Quaternion& q1, const Quaternion& q2);
Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
Quaternion operator*(const Quaternion& q1, double scale);
Quaternion operator*(double scale, const Quaternion& q2);

} // namespace navigine::navigation_core
