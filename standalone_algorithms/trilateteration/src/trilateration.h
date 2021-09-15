/** trilateration.h
 *
 * Author: Aleksei Smirnov <aleksei.smirnov@navigine.ru>
 * Copyright (c) 2014 Navigine. All rights reserved.
 *
 */

#ifndef NAVIGINE_TRILATERATION_ALGORITHM
#define NAVIGINE_TRILATERATION_ALGORITHM

static const int ERROR_NO_SOLUTION_TRILATERATION = 4;
static const int ERROR_IN_TRILATER = 28;

#include <stdio.h>
#include <string>
#include <math.h>
#include <vector>

#include "beacon.h"


// this class allows calculate coordinates of the user using distance or RSSI
// measurements to nearest transmitters such as bluetooth Ibeacons or WiFi Access Points
// 
class Trilateration
{
  public:
    //constructor and destructor
    Trilateration();
    ~Trilateration();
    //copy constructor
    Trilateration(const Trilateration& trilat);

    // update measurements if we got new ones
      void updateMeasurements( std::vector < BeaconMeas >& beaconMeasurements );

    // fill all the beacons located at the map
    void fillLocationBeacons( std::vector <Beacon>& beaconsOnFloor );

    // calculate desired coordinates on the measurement basis
    int calculateCoordinates( );

    //get id of current location
    int getCurrentLocationId() const;

    //set id of current location
    void setCurrentLocationId( int curLoc );

    static void filterUnknownBeacons( std::vector <BeaconMeas>& beaconMeas,
                                      const std::vector<Beacon>& mapBeacons );

    // get X and Y coordinate calculated by trilateration
    double getX() const;
    double getY() const;
    
    //get error message in a case of error
    const char* getErrorMessage() const;

    //print X,Y to file in "%lf %lf \n" format
    void printXyToFile (char* filename) const;

  private:

    std::vector <Beacon>     mLocationBeacons;      //set of beacons at the current location
    std::vector <BeaconMeas> mBeaconMeas;           //set of beacon measurements
    std::vector <double>     mXY;                   //desired X,Y coordinates
    int                      mCurLocationId;        //id of location
    
    std::string              mErrorMessage;         //descriptive error message

    int calculateTrilaterationCoordinates( );       //calculate coordinates (when data is prepared)

    int deleteDuplicateMeasurements (std::vector<BeaconMeas>& BeaconMeasurements );
                                                    ////delete duplicate meas and get average RSSI, Dist.
    
    void getLinearSystem(                           //if we have precise measurements,
      std::vector<double> &matrixA,                 //we determine coordinates by OLS method
      std::vector<double> &b,                       //by solving overdetermined system
      int dim );

    void getLeastSquaresCoordinates(  );           

    void solveLinearSystem(                         //solve overdetermined system if
      std::vector<double> matrixA,                  //pseudo distance equations
      std::vector <double> b );
   
    void setErrorMessage( const char* errorMsg );   //set error message
};


//we use this func in order to sort the beacon measurements
bool compareBeaconMeasByName(
      BeaconMeas first,
      BeaconMeas second );


//find iterator of beacon, from which we got measurements
std::vector<Beacon>::const_iterator findBeaconForMeas( 
                             const std::vector<Beacon>& mapBeacons,
                             const std::string& measureBeaconId );



#endif
