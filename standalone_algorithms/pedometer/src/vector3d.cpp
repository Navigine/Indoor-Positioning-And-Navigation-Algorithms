#include <vector3d.h>

namespace navigine {
namespace navigation_core {

Vector3d::Vector3d(double _x, double _y, double _z)
{
    x = _x;
    y = _y;
    z = _z;
}

double Vector3d::magnitude() const
{
  return std::sqrt(x*x + y*y + z*z);
}

Vector3d Vector3d::normalized() const
{
    double length = this->magnitude();
    
    return (std::fabs(length) > std::numeric_limits<double>::epsilon()) ? Vector3d(x / length, y / length, z / length) : *this;
}

Vector3d& Vector3d::normalize()
{
  double length = this->magnitude();
  
  if (std::fabs(length) > std::numeric_limits<double>::epsilon())
    *this /= length;
  return *this;
}

bool Vector3d::isNull() const
{
  return x == 0 && y == 0 && z == 0;
}

Vector3d& Vector3d::operator+=(const Vector3d& v)
{
    this->x += v.x;
    this->y += v.y;
    this->z += v.z;
    return *this;
}

Vector3d& Vector3d::operator-=(const Vector3d& v)
{
    this->x -= v.x;
    this->y -= v.y;
    this->z -= v.z;
    return *this;
}

Vector3d& Vector3d::operator*=(double multiplier)
{
    this->x *= multiplier;
    this->y *= multiplier;
    this->z *= multiplier;
    return *this;
}

Vector3d& Vector3d::operator/=(double divisor)
{
  this->x /= divisor;
  this->y /= divisor;
  this->z /= divisor;
  return *this;
}

double Vector3d::dotProduct(const Vector3d& v1, const Vector3d& v2)
{
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector3d Vector3d::crossProduct(const Vector3d& v1, const Vector3d& v2)
{
  return Vector3d(v1.y * v2.z - v1.z * v2.y,
                  v1.z * v2.x - v1.x * v2.z, 
                  v1.x * v2.y - v1.y * v2.x);
}

} } // namespace navigine::navigation_core
