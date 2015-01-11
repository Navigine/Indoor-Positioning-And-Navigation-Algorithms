/** pedometer.h
*
* Author: Aleksei Smirnov <aleksei.smirnov@navigine.ru>
* Copyright (c) 2014 Navigine. All rights reserved.
*
*/


#ifndef NAVIGINE_PEDOMETER_CLASS
#define NAVIGINE_PEDOMETER_CLASS

#include <vector>
#include <deque>
#include <cstdio>
#include <cmath>
#include <algorithm>  //include std::max

// AccelComponents class keep information about measurements of each 
// component of acceleration at certain moment of time
// Acc measurements should be calibrated and presented in m/s2 units
class AccelComponents
{
  public:
    AccelComponents();
    AccelComponents(const AccelComponents& accel);
    ~AccelComponents() {};
    
    double mAccX;                             //x axis acceleration
    double mAccY;                             //y axis acceleration
    double mAccZ;                             //z axis acceleration
    double mAccMagnitude;                     //acceleration magnitude (Euclidean vector norm)
    double mAccelFilteredMagnitude;
    double mAccTime;                          //the time in sec in UTC when it was measured
    double mQuality;                          //the quality of accel readings
    
    void fillAccComponents( double accX, double accY, double accZ);
    void setTime( const double time );
    void setQuality( const double quality);
    void calcAccelMagnitude();     
  private:

};


// this class detects step of the human on the basis of accelerometer measurements
// and empirical model of human gate
// Step length is calculated using minimal and maximal accelerations
class Pedometer
{
  public:
  //constructor and destructor
    Pedometer();
    Pedometer( const Pedometer& pedometer );
    ~Pedometer();


    int updateAccMeasurements( std::vector<AccelComponents>& accelerations );

    //if we have new measurement we add it to private acceleration variable
    int updateAccMeasurement( AccelComponents& accValue );

    void clearMeasurements();
    void clearStepsHistory();

    int detectSteps();
    int getNumOfSteps() const;

  private:
    int calcFilteredAccelMagnitude( );
    int calcMinMaxAcc( std::deque<AccelComponents>::const_iterator& accIt,
                       const double TimeInterval );
    void updateThresholds( std::deque<AccelComponents>::const_iterator& currAcc );
    void fprintfResults();

    //we use deque in order to fast delete measurements of the array when we got new ones
    std::deque<AccelComponents> mAccel;                   //array for saving accelerations
    std::vector <double>        mStepsTime;               //the time when step occurred (sec in UTC)
    std::vector <double>        mPossibleStepsTime;       //the time when step possibly occurred (sec in UTC)

    double                     mPedometerThreshold;      //threshold 
    double                     mAccMax, mAccMin;         //minimal and max acc within PEDOMETER_WINDOWS_WIDTH
    bool                       mIsPossibleStep;          //we use this variable to mark step as possible
    double                     mPossibleStepTimer;       //true if step is expected
};






#endif
