/** polynomial_fit.h
 *
 * Copyright (c) 2018 Navigine.
 *
 */

#include "polynomial_fit.h"

namespace navigine {
namespace navigation_core {

PolynomialFit::PolynomialFit(int power, double timeInterval, double timeShift, double newWeight)
  : mPower(power)
  , mTimeInterval(timeInterval)
  , mTimeShift(timeShift)
  , mCoeff(mPower + 1, 0.)
  , mNewSolutionWeight(newWeight)
  , mLastOutput(0)
{
}

void PolynomialFit::addSequencePoint(double x, double y)
{
    if (mY.empty())
        mCoeff.assign(mPower + 1, 0.);

    if (mY.empty() || mY.back() != y)
    {
        mY.push_back(y);
        mX.push_back(x);
        removeOldPoints();
        if ((int)mY.size() > mPower)
            getLeastSquaresCoeff();
    }
}

void PolynomialFit::removeOldPoints()
{
    while ((mX.back() - mX.front()) > mTimeInterval)
    {
        mX.pop_front();
        mY.pop_front();
    }
}

void PolynomialFit::clear()
{
  mX.clear();
  mY.clear();
}

double PolynomialFit::predict(double x)
{
  double out = 0;
  double slnCoef = mNewSolutionWeight;
  int size = (int)mY.size();

  if (size == 0)
  {
    return -1.0;
  }
  else if (size == 1 || size < mPower + 1)
  {
    out = getLagrangeSolution(x);
  }
  else
  {
    out = getLeastSquaresSolution(x);
    out = out * slnCoef + mLastOutput * (1.0 - slnCoef);
  }
  mLastOutput = out;
  return out;
}

double PolynomialFit::getLagrangeSolution(double curTime)
{
  size_t pwr  = mY.size();
  double out  = 0.0;
  double mult = 1.0;
  double time = curTime - mTimeShift;

  for (size_t i = 0; i < pwr; i++)
  {
    mult = 1.0;
    for (size_t j = 0; j < pwr; j++)
    {
      if (i != j)
        mult *= (time - mX[j]) / (mX[i] - mX[j]);
    }
    out += mY[i] * mult;
  }

  return out;
}

double PolynomialFit::getLeastSquaresSolution (double curTime)
{
  double out  = 0.0;
  double time = curTime - mTimeShift;
  
  for (int i = 0; i <= mPower; i++)
  {
    out += mCoeff[i] * pow(time, i);
  }
  return out;
}

void PolynomialFit::getLeastSquaresCoeff()
{
  std::vector <double> A  ( (mPower + 1)*(mPower + 1), 0.);
  std::vector <double> b  (  mPower + 1, 0.);
  std::vector <double> sub(2*mPower + 1, 0.);
  double sum;

  // Initialize sub
  for (int i = 0; i < 2 * mPower + 1; i++)
  {
    sum = 0;
    for (size_t j = 0; j < mY.size(); j++)
    {
      sum += pow(mX[j], i);
    }
    sub[i] = sum;
  }
  // Initialize matrix 'A'
  for (int i = 0; i <  mPower + 1; i++)
    for (int j = 0; j < mPower + 1; j++)
       A[i + j*(mPower + 1)] = sub[i + j];

  // Initialize vector 'b' 
  for (int i = 0; i < mPower + 1; i++)
  {
    sum = 0;
    for (size_t j = 0; j < mY.size(); j++)
    {
      sum += pow(mX[j], i) * mY[j];
    }
    b[i] = sum;
  }

  solveLinearSystem(A, b, mCoeff, mPower + 1);

  return;
}

void solveLinearSystem(const std::vector <double>& A, const std::vector <double>& b,
                       std::vector <double>& x, const int n)
{
  // Rotations method
  int i, j, k;
  double cosphi, sinphi, norm, temp;
  std::vector <double> E(n*(n + 1));
  // E initialization. E is a [n x (n+1)] matrix formed as
  // E = [A,b] to simplify the calculations 
  for (int i = 0; i < n; i++)
  {
    E[i*(n + 1) + n] = b[i];
    for (int j = 0; j < n; j++)
      E[i*(n + 1) + j] = A[i*n + j];
  }

  // Forward Gaussian
  for (k = 0; k < n; ++k)
    for (j = k + 1; j < n; ++j)
    {
    cosphi = E[k * (n + 1) + k];
    sinphi = E[j * (n + 1) + k];
    norm = sqrt(cosphi * cosphi + sinphi * sinphi);
    cosphi /= norm;
    sinphi /= norm;
    for (i = k; i < n + 1; ++i)
    {
      temp = cosphi * E[j * (n + 1) + i] - sinphi * E[k * (n + 1) + i];
      E[k * (n + 1) + i] = sinphi * E[j * (n + 1) + i] + cosphi * E[k * (n + 1) + i];
      E[j * (n + 1) + i] = temp;
    }
    }
  // Back-Gaussing
  for (k = n - 1; k >= 0; --k)
  {
    x[k] = E[k*(n + 1) + n];
    for (i = k + 1; i < n; ++i)
      x[k] -= E[k*(n + 1) + i] * x[i];
    x[k] /= E[k*(n + 1) + k];
  }
  return;
}

} } // namespace navigine::navigation_core
