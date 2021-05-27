#pragma once

#include <cmath>
#include <vector>
#include <limits>

namespace navigine {
namespace navigation_core {

struct Vector3d 
{
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;

  Vector3d() = default;
  Vector3d(double _x, double _y, double _z);

  double    magnitude()  const;
  Vector3d  normalized()  const;
  Vector3d& normalize();
  bool      isNull() const;


  static Vector3d crossProduct(const Vector3d& v1, const Vector3d& v2);
  static double   dotProduct  (const Vector3d& v1, const Vector3d& v2);

  Vector3d& operator+=(const Vector3d& v2);
  Vector3d& operator-=(const Vector3d& v2);
  Vector3d& operator*=(double multiplier);
  Vector3d& operator/=(double divisor);
};


inline bool operator==(const Vector3d& v1, const Vector3d& v2)
{
  return std::fabs(v1.x - v2.x) < std::numeric_limits<double>::epsilon() &&
         std::fabs(v1.y - v2.y) < std::numeric_limits<double>::epsilon() &&
         std::fabs(v1.z - v2.z) < std::numeric_limits<double>::epsilon();
}

inline bool operator!=(const Vector3d& v1, const Vector3d& v2)
{
  return !(v1 == v2);
}

inline Vector3d operator*(double multiplier, const Vector3d& v)
{
  return Vector3d(v.x * multiplier, v.y * multiplier, v.z * multiplier);
}

inline Vector3d operator*(const Vector3d& v, double multiplier)
{
  return Vector3d(v.x * multiplier, v.y * multiplier, v.z * multiplier);
}

inline Vector3d operator+(const Vector3d& v1, const Vector3d& v2)
{
  return Vector3d(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

inline Vector3d operator-(const Vector3d& v1, const Vector3d& v2)
{
  return Vector3d(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

inline Vector3d operator-(const Vector3d& v)
{
  return Vector3d(-v.x, -v.y, -v.z);
}

inline Vector3d operator/(const Vector3d& v1, double divisor)
{
  return Vector3d(v1.x / divisor, v1.y / divisor, v1.z / divisor);
}

} } // namespace navigine::navigation_core
