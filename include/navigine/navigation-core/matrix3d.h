#pragma once

#include <array>
#include <cmath>
#include <ostream>

namespace navigine::navigation_core {

class Vector3d;

class Matrix3d
{
public:
    Matrix3d();
    Matrix3d(double a11, double a12, double a13,
             double a21, double a22, double a23,
             double a31, double a32, double a33);

    void fill(
        double a11, double a12, double a13,
        double a21, double a22, double a23,
        double a31, double a32, double a33);

    void fillRow(unsigned int row, double x, double y, double z);
    void fillRow(unsigned int row, const Vector3d& v);
    void fillColumn(unsigned int col, double x, double y, double z);
    void fillColumn(unsigned int col, const Vector3d& v);

    Matrix3d& setZero();
    Matrix3d& setIdentity();
    Matrix3d& setTranspose();

    double norm() const;
    double squaredNorm() const;

    inline double& at(unsigned int row, unsigned int col) {
        return data_[row * 3 + col];
    }

    inline double at(unsigned int row, unsigned int col) const {
        return data_[row * 3 + col];
    }

    inline double* row(unsigned int index) {
        return data_.data() + index * 3;
    }

    inline const double* row(unsigned int index) const {
        return data_.data() + index * 3;
    }

    inline double* data() {
        return data_.data();
    }

    inline const double* data() const {
        return data_.data();
    }

    Matrix3d& operator+=(const Matrix3d& m);
    Matrix3d& operator-=(const Matrix3d& m);
    Matrix3d& operator*=(const Matrix3d& m);
    Matrix3d& operator*=(double multiplier);
    Matrix3d& operator/=(double divisor);

    Matrix3d operator-() const;
    Matrix3d operator+(const Matrix3d& m) const;
    Matrix3d operator-(const Matrix3d& m) const;
    Matrix3d operator*(const Matrix3d& m) const;
    Vector3d operator*(const Vector3d& v) const;
    Matrix3d operator*(double multiplier) const;
    Matrix3d operator/(double divisor) const;

    bool operator==(const Matrix3d& m) const;
    bool operator!=(const Matrix3d& m) const;

    Matrix3d& addMul(const Matrix3d& m, double multiplier);  // Add to the current matrix `m * multiplier`
    Matrix3d& mulLeft(const Matrix3d& m);                    // Multiply the current matrix by `m` from left
    Matrix3d& mulRight(const Matrix3d& m);                   // Multiply the current matrix by `m` from right

    Matrix3d inverse() const;
    Matrix3d transpose() const;
    Matrix3d orthogonalize() const;
    double determinant() const;

    bool isBounded(double value) const;
    bool isFinite() const;

    static Matrix3d zero();                         // Make zero matrix
    static Matrix3d identity();                     // Make indetity matrix
    static Matrix3d diagonal(const Vector3d& v);    // Make diagonal matrix
    static Matrix3d skew(const Vector3d& v);        // Make skew-symmetric matrix from vector
    static Matrix3d rotationX(double angle);        // Make rotation matrix from angle (radians, counter-clockwise around X-axis)
    static Matrix3d rotationY(double angle);        // Make rotation matrix from angle (radians, counter-clockwise around Y-axis)
    static Matrix3d rotationZ(double angle);        // Make rotation matrix from angle (radians, counter-clockwise around Z-axis)
    static Matrix3d tensorProduct(const Vector3d& u, const Vector3d& v);    // Make matrix: u * v^t

    static double determinant(
        double a11, double a12, double a13,
        double a21, double a22, double a23,
        double a31, double a32, double a33);

private:
    std::array<double, 9> data_;
};

std::ostream& operator<<(std::ostream& os, const Matrix3d& m);

} // namespace navigine::navigation_core
