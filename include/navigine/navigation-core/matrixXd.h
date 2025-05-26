#pragma once

#include <initializer_list>
#include <ostream>
#include <vector>

namespace navigine::navigation_core {

class Matrix2d;
class Matrix3d;
class Matrix4d;
class Vector2d;
class Vector3d;
class Vector4d;
class VectorXd;

class MatrixXd
{
public:
    MatrixXd() = default;
    MatrixXd(const Matrix2d& m);
    MatrixXd(const Matrix3d& m);
    MatrixXd(const Matrix4d& m);
    MatrixXd(unsigned int numRows, unsigned int numCols, const std::initializer_list<double>& values = {});

    unsigned int numRows() const;
    unsigned int numColumns() const;

    void resize(unsigned int numRows, unsigned int numCols);

    inline double& at(unsigned int row, unsigned int col) {
        return data_[numCols_ * row + col];
    }

    inline double at(unsigned int row, unsigned int col) const {
        return data_[numCols_ * row + col];
    }

    inline double* row(unsigned int index) {
        return data_.data() + numCols_ * index;
    }

    inline const double* row(unsigned int index) const {
        return data_.data() + numCols_ * index;
    }

    inline double* data() {
        return data_.data();
    }

    inline const double* data() const {
        return data_.data();
    }

    MatrixXd block(
        unsigned int sizeRows,
        unsigned int sizeCols,
        unsigned int startRow,
        unsigned int startCol) const;

    Matrix2d block2x2(unsigned int startRow, unsigned int startCol) const;
    Matrix3d block3x3(unsigned int startRow, unsigned int startCol) const;
    Matrix4d block4x4(unsigned int startRow, unsigned int startCol) const;

    void fill(double value);

    void fillRow(unsigned int i, double value);
    void fillRow(unsigned int i, const VectorXd& v);
    void fillRow(unsigned int i, const std::initializer_list<double>& values);
    void fillColumn(unsigned int j, double value);
    void fillColumn(unsigned int j, const VectorXd& v);
    void fillColumn(unsigned int j, const std::initializer_list<double>& values);

    void fillBlock(const Matrix2d& block, unsigned int startRow, unsigned int startCol);
    void fillBlock(const Matrix3d& block, unsigned int startRow, unsigned int startCol);
    void fillBlock(const Matrix4d& block, unsigned int startRow, unsigned int startCol);
    void fillBlock(const MatrixXd& block, unsigned int startRow, unsigned int startCol);

    void fillBlockMul(const Matrix2d& block, double multiplier, unsigned int startRow, unsigned int startCol);
    void fillBlockMul(const Matrix3d& block, double multiplier, unsigned int startRow, unsigned int startCol);
    void fillBlockMul(const Matrix4d& block, double multiplier, unsigned int startRow, unsigned int startCol);
    void fillBlockMul(const MatrixXd& block, double multiplier, unsigned int startRow, unsigned int startCol);

    void addBlockMul(const Matrix2d& block, double multiplier, unsigned int startRow, unsigned int startCol);
    void addBlockMul(const Matrix3d& block, double multiplier, unsigned int startRow, unsigned int startCol);
    void addBlockMul(const Matrix4d& block, double multiplier, unsigned int startRow, unsigned int startCol);
    void addBlockMul(const MatrixXd& block, double multiplier, unsigned int startRow, unsigned int startCol);

    MatrixXd& setIdentity();
    MatrixXd& setTranspose();
    MatrixXd& setZero();

    double norm() const;
    double squaredNorm() const;

    MatrixXd& operator+=(const MatrixXd& m);
    MatrixXd& operator-=(const MatrixXd& m);
    MatrixXd& operator*=(const MatrixXd& m);
    MatrixXd& operator*=(double factor);
    MatrixXd& operator/=(double factor);

    MatrixXd operator-() const;
    MatrixXd operator+(const MatrixXd& m) const;
    MatrixXd operator-(const MatrixXd& m) const;
    MatrixXd operator*(const MatrixXd& m) const;
    VectorXd operator*(const Vector2d& v) const;
    VectorXd operator*(const Vector3d& v) const;
    VectorXd operator*(const Vector4d& v) const;
    VectorXd operator*(const VectorXd& v) const;
    MatrixXd operator*(double factor) const;
    MatrixXd operator/(double factor) const;

    bool operator==(const MatrixXd& m) const;
    bool operator!=(const MatrixXd& m) const;

    MatrixXd& addMul(const MatrixXd& m, double multiplier);
    MatrixXd& mulLeft(const MatrixXd& m);
    MatrixXd& mulRight(const MatrixXd& m);

    MatrixXd transpose() const;
    MatrixXd inverse() const;

    bool isBounded(double value) const;
    bool isFinite() const;

    Matrix2d toMatrix2d(unsigned int startRow = 0, unsigned int startCol = 0) const;
    Matrix3d toMatrix3d(unsigned int startRow = 0, unsigned int startCol = 0) const;
    Matrix4d toMatrix4d(unsigned int startRow = 0, unsigned int startCol = 0) const;
    std::vector<std::vector<double>> toVectorVector() const;

    static MatrixXd zero(unsigned int numRows, unsigned int numCols);
    static MatrixXd identity(unsigned int numRows);
    static MatrixXd diagonal(const VectorXd& v);

private:
    unsigned int numRows_ = 0;
    unsigned int numCols_ = 0;
    std::vector<double> data_;
};

std::ostream& operator<<(std::ostream& os, const MatrixXd& m);

} // namespace navigine::navigation_core
