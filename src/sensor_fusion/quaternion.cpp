/** quaternion.cpp
*
* Copyright (c) 2018 Navigine.
*
*/

#include <limits>
#include "quaternion.h"

namespace navigine {
namespace navigation_core {

Quaternion::Quaternion(double _w, double _x, double _y, double _z)
  : w{_w}, x{_x}, y{_y}, z{_z}
{ }

Quaternion::Quaternion(double _x, double _y, double _z)
  : w{0}, x{_x}, y{_y}, z{_z}
{ }

Quaternion::Quaternion(const Vector3d& vec)
  : w{0.0}, x{vec.x}, y{vec.y}, z{vec.z}
{ }

Quaternion Quaternion::normalized() const
{
  double norm = std::sqrt(w * w + x * x + y * y + z * z);

  return (std::fabs(norm) > std::numeric_limits<double>::epsilon())
    ? Quaternion(w /norm, x/norm, y / norm, z / norm)
    : *this;
}

std::tuple<double, double, double> Quaternion::toEuler() const
{
	double sinr = 2.0 * (w * x + y * z);
	double cosr = 1.0 - 2.0 * (x * x + y * y);
	auto roll = std::atan2(sinr, cosr);

	double sinp  = 2.0 * (w * y - z * x);
  double pitch = 0.0;
	if (std::fabs(sinp) >= 1)
		pitch = std::copysign(M_PI / 2, sinp); // use 90 degrees if out of range
	else
		pitch = asin(sinp);

	double siny = 2.0 * (w * z + x * y);
	double cosy = 1.0 - 2.0 * (y * y + z * z);  
	double yaw = std::atan2(siny, cosy);
  
  return std::make_tuple(roll, pitch, yaw);
}

Quaternion& Quaternion::operator+=(const Quaternion& r_q)
{
  this->w += r_q.w;
  this->x += r_q.x;
  this->y += r_q.y;
  this->z += r_q.z;
  return *this;
}

Quaternion& Quaternion::operator-=(const Quaternion& r_q)
{
  this->w -= r_q.w;
  this->x -= r_q.x;
  this->y -= r_q.y;
  this->z -= r_q.z;
  return *this;
}

Quaternion& Quaternion::operator*=(const double& scale)
{
  this->w *= scale;
  this->x *= scale;
  this->y *= scale;
  this->z *= scale;
  return *this;
}

Quaternion& Quaternion::operator*=(const Quaternion& other)
{
  double qw = w, qx = x, qy = y, qz = z;
  this->w = qw*other.w - qx*other.x - qy*other.y - qz*other.z;
  this->x = qw*other.x + qx*other.w + qy*other.z - qz*other.y;
  this->y = qw*other.y - qx*other.z + qy*other.w + qz*other.x;
  this->z = qw*other.z + qx*other.y - qy*other.x + qz*other.w;
  return *this;
}

Quaternion operator+(Quaternion l_q, const Quaternion& r_q)
{
  return l_q += r_q;
}

Quaternion operator-(Quaternion l_q, const Quaternion& r_q)
{
  return l_q -= r_q;
}

Quaternion operator*(Quaternion l_q, const double& scale)
{
  return l_q *= scale;
}

Quaternion operator*(const double& scale, Quaternion l_q)
{
  return l_q *= scale;
}

Quaternion operator*(Quaternion l_q, const Quaternion& r_q)
{
  return l_q *= r_q;
}

} } // namespace navigine::navigation_core
