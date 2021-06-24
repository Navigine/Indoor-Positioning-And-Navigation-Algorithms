/** quaternion.h
*
* Copyright (c) 2018 Navigine.
*
*/

#ifndef NAVIGINE_QUATERNION_H
#define NAVIGINE_QUATERNION_H

#include <navigine/navigation-core/vector3d.h>

#include <vector>
#include <stdexcept>
#include <cmath>
#include <tuple>

namespace navigine {
namespace navigation_core {

class Quaternion
{
  public:
    double w = 1.0;
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    
  public:
    Quaternion(){};
    Quaternion(double _w, double x, double _y, double _z);
    Quaternion(double _x, double _y, double _z);
    Quaternion(const Vector3d& vec);

    Quaternion normalized() const;
    Quaternion conj()       const { return Quaternion(w, -x, -y, -z); }
    Vector3d   toVector3d() const { return Vector3d(x, y, z); }

    std::tuple<double, double, double> toEuler() const;
    
    Quaternion& operator+=(const Quaternion& r_q);
    Quaternion& operator-=(const Quaternion& r_q);
    Quaternion& operator*=(const double& scale);
    Quaternion& operator*=(const Quaternion& r_q);
};

Quaternion operator+(Quaternion l_q, const Quaternion& r_q);
Quaternion operator-(Quaternion l_q, const Quaternion& r_q);
Quaternion operator*(Quaternion l_q, const double& scale);
Quaternion operator*(const double& scale, Quaternion l_q);
Quaternion operator*(Quaternion q1, const Quaternion& q2);

} } // namespace navigine::navigation_core

#endif //NAVIGINE_QUATERNION_H
