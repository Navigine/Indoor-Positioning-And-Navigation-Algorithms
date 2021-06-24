/** polynomial_fit.h
 *
 * Copyright (c) 2018 Navigine.
 *
 */

#ifndef POLYNOMIAL_FIT_H
#define POLYNOMIAL_FIT_H

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <vector>

namespace navigine {
namespace navigation_core {

void solveLinearSystem(const std::vector <double>& A, const std::vector <double>& b,
                  std::vector <double>& x, const int n);

class PolynomialFit
{
    public:
        PolynomialFit(int power = 1, double timeInterval = 2.0, double timeShift = 0.7, double newWeight = 0.2);

        void addSequencePoint(double x, double y);
        double predict(double x);
        void clear();
    
    private:
        const int mPower;
        const double mTimeInterval;
        const double mTimeShift;
        const double mNewSolutionWeight;
        double mLastOutput;

        std::deque<double> mY;
        std::deque<double> mX;
        std::vector<double> mCoeff;

        void removeOldPoints();
        void getLeastSquaresCoeff();
        double getLagrangeSolution(const double x);
        double getLeastSquaresSolution(const double x);
};

} } // namespace navigine::navigation_core

#endif
