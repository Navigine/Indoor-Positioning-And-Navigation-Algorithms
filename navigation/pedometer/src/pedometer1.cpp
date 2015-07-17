/** pedometer.cpp
*
* Author: Aleksei Smirnov <aleksei.smirnov@navigine.ru>
* Copyright (c) 2014 Navigine. All rights reserved.
*
*/

#if defined (WIN32) || defined (_WIN32)
//#define  DEBUG_PEDOMETER_CLASS
#define  _CRT_SECURE_NO_WARNINGS
#endif

#include "pedometer.h"

using namespace std;

static const double FILTER_ACT_TIME = 0.3;
static const double K_Length = 0.3;
static const double K_const = 6.;
static const double GRAVITY_FORCE = 9.80665;
static const double CHECK_TIME_SEC = 0.05;
static const double UPDATE_TIME_INTERVAL = 1.;  //interval on which we update accelMax, Min, threshold
static const int    MAX_MEASUREMENT_ARRAY_SIZE = 2000;

//constructor
AccelComponents::AccelComponents()
  : mAccX( 0. )
  , mAccY( 0. )
  , mAccZ( 0. )
  , mAccMagnitude( 0. )
  , mAccelFilteredMagnitude( 0. )
  , mAccTime( 0. )
  , mQuality( 0. )
{

}


//copy constructor
AccelComponents::AccelComponents( const AccelComponents& accel )
  : mAccX( accel.mAccX )
  , mAccY( accel.mAccY )
  , mAccZ( accel.mAccZ )
  , mAccMagnitude( accel.mAccMagnitude )
  , mAccelFilteredMagnitude( accel.mAccelFilteredMagnitude )
  , mAccTime( accel.mAccTime )
  , mQuality( accel.mQuality )
{

}


// this method fills acceleration components of AccelComponents class
void AccelComponents::fillAccComponents(double accX, double accY, double accZ)
{
  mAccX = accX;
  mAccY = accY;
  mAccZ = accZ;
}


void AccelComponents::setTime( const double time )
{
  mAccTime = time;
}


void AccelComponents::setQuality( const double quality )
{
  mQuality = quality;
}


// this function calculates magnitude of acceleration vector
// and subtract gravity, therefore final magnitude may be negative
void AccelComponents::calcAccelMagnitude()
{
  double offset = 0.45;
  mAccMagnitude = sqrt( mAccX * mAccX + mAccY * mAccY + mAccZ * mAccZ ) - GRAVITY_FORCE -
    offset;
  return;
}


Pedometer::Pedometer()
{
  //initial empiric threshold values from article
  mPedometerThreshold = 0.05 * GRAVITY_FORCE;
}


// copy constructor of pedometer class
Pedometer::Pedometer( const Pedometer& pedometer )
  : mAccel( pedometer.mAccel )
  , mStepsTime( pedometer.mStepsTime )
  , mPossibleStepsTime( pedometer.mPossibleStepsTime)
  , mPedometerThreshold( pedometer.mPedometerThreshold )
  , mAccMax( pedometer.mAccMax )
  , mAccMin( pedometer.mAccMin )
  , mIsPossibleStep( pedometer.mIsPossibleStep )
  , mPossibleStepTimer( pedometer.mPossibleStepTimer )
{

}


Pedometer::~Pedometer()
{

}


// function update private measurement variables of the class
// accelerations - calibrated measurements of acc in m/s2 units
// time - the times when measurements were obtained
// acceleration array size should be equal to time array size
// the time should be in ascending order!
int Pedometer::updateAccMeasurements( std::vector<AccelComponents>& accelerations )
{
  if ( accelerations.size() == 0 )
  {
    printf( "acceleration vector size = 0\n" );
    return -1;
  }

  //iterate through accelerations vector
  for ( unsigned int j = 0; j < accelerations.size(); ++j )
  {
    int error = updateAccMeasurement( accelerations[ j ] );
    if ( error == -1 )
      return error;
  }

  return 0;
}


// if we have new  acceleration measurement we add it
// to private acceleration variable mAccel
// acceleration should be in m/s2 units
int Pedometer::updateAccMeasurement( AccelComponents& accValue )
{
  if ( accValue.mAccTime <= 0. )
  {
    printf( "ERROR: bad acceleration time. It should be > 0\n" );
    return -1;
  }
  //fill private acceleration arrays

  if ( mAccel.size () != 0 && accValue.mAccTime < mAccel.back().mAccTime )
  {
    printf( "acceleration with bigger time was filled before acceleration with less time\n"
            "You should fill the time in ascending order\n" );
    return -1;
  }

  //calculate acceleration magnitude and add accValue to acceleration array
  accValue.calcAccelMagnitude();
  Pedometer::mAccel.push_back( accValue );
  int error = calcFilteredAccelMagnitude();
  if ( error )
    return error;

   while ( mAccel.size() > MAX_MEASUREMENT_ARRAY_SIZE &&
           mAccel.back().mAccTime - mAccel.front().mAccTime > FILTER_ACT_TIME)
   {
     mAccel.pop_front();
   }

  return 0;
}


//clear all private variable with measurements
void Pedometer::clearMeasurements()
{
  Pedometer::mAccel.clear();
}


void Pedometer::clearStepsHistory()
{
  mStepsTime.clear();
}


//this method calculates and form array with filtered acceleration magnitude
// sliding window average is used (low pass)
int Pedometer::calcFilteredAccelMagnitude()
{
  //check size of accelerations array
  if ( mAccel.size() == 0 )
  {
    printf( "ERROR: acceleration vector size = 0! \n" );
    return -1;
  }

  double filteredAccMagnitude = 0.;
  int nMeasurements = 0;

  deque<AccelComponents>::const_reverse_iterator it = mAccel.rbegin();
  for (; it != mAccel.rend(); ++it)
  {
    //sum all the measurements in order to get average
    filteredAccMagnitude += it->mAccMagnitude;
    ++nMeasurements;

    //we sum while filter slope window is within FILTER_ACT_TIME
    if ( mAccel.back().mAccTime - it->mAccTime > FILTER_ACT_TIME )
      break;
  }

  if ( nMeasurements == 0 )
  {
    printf( "There is no measurements at acceleration array "
            "nMeasurements == 0\n" );
    return -1;
  }

  //divide on number of measurements
  filteredAccMagnitude /= static_cast<double> ( nMeasurements );
  mAccel.back().mAccelFilteredMagnitude = filteredAccMagnitude;

  return 0;
}


// Function returns -1 in a case of any error
int Pedometer::detectSteps()
{

  if ( mAccel.size() <= 3 )
  {
    printf( "ERROR: there is no enough accelerometer measurements to detect steps" );
    return -1;
  }

  //to detect every 1 sec. interval
  double oneSecTickStart = mAccel.front().mAccTime;

  //clear step if we already processed these measurements 
  // otherwise we'll have duplicate steps
  if (mPossibleStepsTime.size() != 0 && mPossibleStepsTime.front() > mAccel.front().mAccTime)
  {
    mStepsTime.clear();
    mPossibleStepsTime.clear();
  }

  double negativeAccMagnTime = 0.;
  //we start iterating from 2nd acceleration
  for ( deque<AccelComponents>::const_iterator currAcc = mAccel.begin() + 2;
        currAcc != mAccel.end();
        ++currAcc )
  {
    bool crossThreshold = ((currAcc - 1)->mAccelFilteredMagnitude < mPedometerThreshold ||
                            (currAcc - 2)->mAccelFilteredMagnitude < mPedometerThreshold);
    
    if ( currAcc->mAccelFilteredMagnitude > mPedometerThreshold &&
          crossThreshold &&
          !mIsPossibleStep )
    {
      mIsPossibleStep = true;

      // save time of likely step
      mPossibleStepTimer = currAcc->mAccTime;
      mPossibleStepsTime.push_back(mPossibleStepTimer);
    }
    
    //if acceleration filtered magnitude got to negative zone  
    if ( currAcc->mAccelFilteredMagnitude < 0 &&
         ( currAcc - 1 )->mAccelFilteredMagnitude > 0 )
    {
      // A detected step is verified by two additional computations:
      // by the zero - crossing technique and by
      // the human body model
      // We remember the time of first occurrence of negative acceleration magnitude
      negativeAccMagnTime = ( currAcc )->mAccTime;
    }

    //if acceleration magnitude crossed zero threshold value, then it's a step
    if ( mIsPossibleStep &&
         currAcc->mAccelFilteredMagnitude > 0 &&
         ( currAcc - 1 )->mAccelFilteredMagnitude < 0 &&
         currAcc->mAccTime - negativeAccMagnTime > CHECK_TIME_SEC )
    {
      //We detected the step
      mStepsTime.push_back( mPossibleStepTimer );
      mIsPossibleStep = false;

       //calculate step length here
    }
     else if (currAcc->mAccelFilteredMagnitude > 0 &&
              (currAcc - 1)->mAccelFilteredMagnitude < 0 &&
               currAcc->mAccTime - negativeAccMagnTime < CHECK_TIME_SEC )
       mIsPossibleStep = false;

    // if UPDATE_TIME_INTERVAL passed we update max and min
    // accelerations and thresholds
    if ( currAcc->mAccTime - oneSecTickStart > UPDATE_TIME_INTERVAL )
    {
      int error = calcMinMaxAcc( currAcc, UPDATE_TIME_INTERVAL );
      if ( error )
        return error;

      updateThresholds(currAcc);
      // renew oneSecTickStart
      oneSecTickStart = currAcc->mAccTime;
    }

    // if we didn't detect step during last 2 seconds
    if ( currAcc->mAccTime - mPossibleStepTimer > 2. )
      mIsPossibleStep = false;
  }
  
#ifdef DEBUG_PEDOMETER_CLASS
  fprintfResults();
#endif

  return 0;
}


int Pedometer::getNumOfSteps() const
{
  return mStepsTime.size();
}


// This function modify mAccMax, mAccMin members of the class and 
// takes rightBorderAcc iterator as a right border of search interval and TimeInterval
// as a length of search interval in seconds
int Pedometer::calcMinMaxAcc( std::deque<AccelComponents>::const_iterator& rightBorderAcc,
                              const double TimeInterval )
{
  if ( mAccel.size() < 3 )
  {
    printf( "ERROR: there is no enough accelerometer measurements to detect steps" );
    return -1;
  }

  double curTime = rightBorderAcc->mAccTime;
  //set initial max and min accelerations
  mAccMax = rightBorderAcc->mAccelFilteredMagnitude;
  mAccMin = rightBorderAcc->mAccelFilteredMagnitude;
  for ( deque<AccelComponents>::const_iterator accIt = rightBorderAcc;
        accIt != mAccel.begin() /*|| curTime - accIt->mAccTime > 1.*/; --accIt )
  {
    if ( curTime - accIt->mAccTime > 1. ) 
      break;

    if ( accIt->mAccelFilteredMagnitude > mAccMax )
    {
      mAccMax = accIt->mAccelFilteredMagnitude;
    }

    if ( accIt->mAccelFilteredMagnitude < mAccMin )
    {
      mAccMin = accIt->mAccelFilteredMagnitude;
    }

  }
  return 0;
}


void Pedometer::updateThresholds( deque<AccelComponents>::const_iterator& currAcc )
{
  double thresholdLastStep = mPedometerThreshold;
  mPedometerThreshold = (thresholdLastStep + (mAccMax - mAccMin) / K_const) / 2.;

  //to avoid cases when threshold is too little
  mPedometerThreshold = (std::max)(mPedometerThreshold, 0.05 * GRAVITY_FORCE);

  return;
}


// this function print some useful data such as step moments, acc reading and so forth 
// to the files. This function is mostly used for debugging and analyzing measurements
void Pedometer::fprintfResults()
{
  remove( "steps.txt" );
  remove( "possible_steps.txt" );
  remove( "accelerations.txt" );
  remove( "accelerations.txt" );
  remove( "acc_scenario.txt" );

  FILE *f;
  if (!mStepsTime.empty())
  {
    f = fopen( "steps.txt", "a" );
    if (f != 0)
    {
      for (vector<double>::const_iterator it = mStepsTime.begin();
            it != mStepsTime.end(); ++it)
        fprintf( f, "%lf\n", *it );
    
      fclose( f );
    }
    else
      printf( "ERROR opening steps.txt\n" );
  }

  if (!mPossibleStepsTime.empty())
  {
    f = fopen( "possible_steps.txt", "a" );
    if (f != 0 )
    {
      for (vector<double>::const_iterator it = mPossibleStepsTime.begin();
            it != mPossibleStepsTime.end(); ++it)
      {
        fprintf( f, "%lf\n", *it );
      }
      fclose( f );
    }
    else
      printf("ERROR opening possible_steps.txt\n");
  }

  if (!mAccel.empty())
  {
    f = fopen( "acc_scenario.txt", "a" );
    if (f != 0)
    {
      for (deque<AccelComponents>::const_iterator it = mAccel.begin();
            it != mAccel.end(); ++it)
      {
        fprintf( f, "%lf %lf %lf %lf\n", it->mAccX, it->mAccY, it->mAccZ, it->mAccTime );
      }
      fclose( f );
    }
    else
      printf( "Error opening acc_scenario file!\n" );



    f = fopen( "accelerations.txt", "a" );
    if (f != 0)
    {
      for (deque<AccelComponents>::const_iterator it = mAccel.begin();
            it != mAccel.end(); ++it)
      {
        fprintf( f, "%%lf lf %lf %lf %lf\n", it->mAccMagnitude, it->mAccelFilteredMagnitude,
                 mPedometerThreshold, it->mAccTime );
      }
      fclose( f );
    }
    else
      printf( "Error opening accelerations.txt file!\n" );
  }

  return;
}

