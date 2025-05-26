#pragma once

#include <initializer_list>
#include <ostream>
#include <vector>

#include <navigine/navigation-core/vector2d.h>
#include <navigine/navigation-core/vector3d.h>
#include <navigine/navigation-core/vector4d.h>

namespace navigine::navigation_core {

class VectorXd
{
public:
    VectorXd() = default;
    VectorXd(unsigned int size, const std::initializer_list<double>& values = {});
    VectorXd(const Vector2d& v);
    VectorXd(const Vector3d& v);
    VectorXd(const Vector4d& v);

    unsigned int size() const;

    void resize(unsigned int size);

    VectorXd segment(unsigned int startIndex, unsigned int size) const;
    Vector2d segment2(unsigned int startIndex) const;
    Vector3d segment3(unsigned int startIndex) const;
    Vector4d segment4(unsigned int startIndex) const;

    void fill(const std::initializer_list<double>& values);
    void fill(double value);

    void fillSegment(double value, unsigned int startIndex, unsigned int size);
    void fillSegment(const VectorXd& segment, unsigned int startIndex);
    void fillSegment(const Vector2d& segment, unsigned int startIndex);
    void fillSegment(const Vector3d& segment, unsigned int startIndex);
    void fillSegment(const Vector4d& segment, unsigned int startIndex);

    void setZero();

    double norm() const;
    VectorXd normalized() const;
    VectorXd& normalize();

    double dotProduct(const VectorXd& v) const;
    double dotProduct(const double* values) const;

    VectorXd& operator+=(const VectorXd& v);
    VectorXd& operator-=(const VectorXd& v);

    VectorXd operator+(const VectorXd& v) const;
    VectorXd operator-(const VectorXd& v) const;

    VectorXd& operator*=(double factor);
    VectorXd& operator/=(double factor);

    VectorXd operator*(double factor) const;
    VectorXd operator/(double factor) const;

    bool operator==(const VectorXd& v) const;
    bool operator!=(const VectorXd& v) const;

    Vector2d toVector2d(unsigned int startIndex = 0) const;
    Vector3d toVector3d(unsigned int startIndex = 0) const;
    Vector4d toVector4d(unsigned int startIndex = 0) const;
    std::vector<double> toVector() const;

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

    bool isBounded(double value) const;
    bool isFinite() const;
    bool isEmpty() const;

    static VectorXd zero(unsigned int size);
    static double dotProduct(const VectorXd& v1, const VectorXd& v2);
    static double dotProduct(
        const double* v1, unsigned int stride1,
        const double* v2, unsigned int stride2,
        unsigned int count);

private:
    std::vector<double> data_;
};

std::ostream& operator<<(std::ostream& os, const VectorXd& v);

} // namespace navigine::navigation_core
