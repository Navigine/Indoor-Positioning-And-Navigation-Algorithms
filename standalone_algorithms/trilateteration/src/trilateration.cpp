  /** trilateration.cpp
 *
 * Author: Aleksei Smirnov <aleksei.smirnov@navigine.ru>
 * Copyright (c) 2014 Navigine. All rights reserved.
 *
 */

#if defined WIN32 || defined _WIN32
#define  _CRT_SECURE_NO_WARNINGS
#endif

#include <algorithm>

#include "trilateration.h"

using namespace std;



Trilateration::Trilateration()
{
  mXY.assign( 2, 0.0 );
  mCurLocationId = 0;
}

Trilateration::Trilateration( const Trilateration& trilat )
  : mLocationBeacons( trilat.mLocationBeacons )
  , mBeaconMeas     ( trilat.mBeaconMeas )
  , mXY             ( trilat.mXY )
  , mCurLocationId  ( trilat.mCurLocationId )
  , mErrorMessage   ( trilat.mErrorMessage )
{

}


Trilateration::~Trilateration()
{

}


void Trilateration::updateMeasurements( std::vector < BeaconMeas >& beaconMeas )
{
  Trilateration::mBeaconMeas = beaconMeas;
}


void Trilateration::fillLocationBeacons( std::vector <Beacon>& beaconsOnFloor )
{
  Trilateration::mLocationBeacons = beaconsOnFloor;
}


int Trilateration::calculateCoordinates()
{
  int errorCode = deleteDuplicateMeasurements( Trilateration::mBeaconMeas );

  if (errorCode)
    return errorCode;

  // sort into ascending order (from -100 to 0)
  std::sort( mBeaconMeas.begin(), mBeaconMeas.end() );

  //filter out beacons that are not in the map
  filterUnknownBeacons (mBeaconMeas, mLocationBeacons);

  if (mBeaconMeas.size () == 0 )
  {
    printf("The number of visible beacon = %ld \n",
              mBeaconMeas.size ());
    return ERROR_NO_SOLUTION_TRILATERATION;
  }

  // number of measurements that we'll use to obtain coordinates
  unsigned int nOfMeas = (mBeaconMeas.size() >= 3 ? 3 : mBeaconMeas.size());
  
  std::vector<BeaconMeas> mostStrongMeas;
  if (mBeaconMeas.size() >= 3)
  {
    for (unsigned int i = 0; i < nOfMeas; i++)
      mostStrongMeas.push_back(mBeaconMeas.at(i));
  }

  errorCode = calculateTrilaterationCoordinates();
  if (errorCode)
    return errorCode;

  const int UseOls = 0;
  if (UseOls)
  {
    getLeastSquaresCoordinates();
    getLeastSquaresCoordinates();
  }

#ifdef TRILATERATION_DEBUG_
  printXyToFile ("trilateration_xy_debug.txt");
  printf ("x = %lf \n y = %lf \n",mXY[0],mXY[1]);
#endif
  return 0;
}


int Trilateration::getCurrentLocationId() const
{
  return Trilateration::mCurLocationId;
}


void Trilateration::setCurrentLocationId( int curSubloc )
{
  Trilateration::mCurLocationId = curSubloc;
}


// this function erase unknown transmitters that don't presence in a map
// and fill x,y, coordinates of beacons
void Trilateration::filterUnknownBeacons( vector <BeaconMeas>& beaconMeas,
                                          const vector<Beacon>& mapBeacons )
{
  // don't increment iterator in for 'cause there is erase inside loop.
  // otherwise we'll skip 1 element after erased
  for (vector <BeaconMeas>::iterator it = beaconMeas.begin();
        it != beaconMeas.end(); )
  {
    std::vector<Beacon>::const_iterator itBeacon;
    itBeacon = findBeaconForMeas( mapBeacons,
                                  it->getBeaconId() );

    if (itBeacon == mapBeacons.end())
    {
      it = beaconMeas.erase( it );
      if (it == beaconMeas.end())
        break;
    }
    else
    {
      it->setBeaconPtr( &(*itBeacon) );
      ++it;
    }

  }
  return;
}


double Trilateration::getX() const
{
  return Trilateration::mXY[ 0 ];
}


double Trilateration::getY() const
{
  return Trilateration::mXY[ 1 ];
}


const char* Trilateration::getErrorMessage() const
{
  return mErrorMessage.c_str();
}


void Trilateration::printXyToFile( char* filename ) const
{
  FILE *f;
  f = fopen( filename, "w" );
  fprintf( f, "%lf %lf \n", mXY[ 0 ], mXY[ 1 ] );
  fclose( f );
}


int Trilateration::calculateTrilaterationCoordinates()
{
  double normalizeCoefficient = 0.0;
  //take revert values, because lower is the distance, greater the weight gets
  for (unsigned int i = 0; i < mBeaconMeas.size(); i++)
    normalizeCoefficient += 1.0 / fabs( mBeaconMeas[ i ].getDistance() );

  vector <double> weight( mBeaconMeas.size(), 0.0 );

  for (unsigned int i = 0; i < mBeaconMeas.size(); i++)
  {
    if (mBeaconMeas[i].getBeaconPtr() == 0)
    {
      setErrorMessage("ERROR: BeaconMes it =%s : Beacon ptr == NULL\n");
      printf( "%s\n", getErrorMessage() );
      return ERROR_IN_TRILATER;
    }
    // calculate probability of being at beacons x,y coordinates
    weight[ i ] += 1.0 / (fabs( mBeaconMeas[ i ].getDistance() *
      normalizeCoefficient ));

    double beaconX = 0, beaconY = 0.;
    beaconX = mBeaconMeas[ i ].getBeaconPtr()->getX();
    beaconY = mBeaconMeas[ i ].getBeaconPtr()->getY();

    //find final coordinates according to probability
    mXY[ 0 ] += weight[ i ] * beaconX;
    mXY[ 1 ] += weight[ i ] * beaconY;
  }
  return 0;
}


// this function deletes Duplicate beacon measurements and averages rssi signals
// from equal beacons
int Trilateration::deleteDuplicateMeasurements( vector<BeaconMeas>& beaconMeas )
{
  //sort measurements in beacon name order
  std::sort( beaconMeas.begin(), beaconMeas.end(), compareBeaconMeasByName );
  //for (int i = 0; i < beaconEntry.size(); i++)
  for (std::vector<BeaconMeas>::iterator itBeaconMeas = beaconMeas.begin();
       itBeaconMeas != beaconMeas.end(); ++itBeaconMeas)
  {
    //count number of occurrences of itBeaconMeas
    std::vector<BeaconMeas>::iterator it = beaconMeas.begin();

    //find first occurrence
    std::count( beaconMeas.begin(), beaconMeas.end(), *itBeaconMeas);
    it = std::find( it, beaconMeas.end(), *itBeaconMeas );
    
    int nOfMeasFromSameAp = 0;
    double rssi = 0.0, distance = 0.0;

    //find all similar entries
    while (it != beaconMeas.end())
    {
      nOfMeasFromSameAp++;

      if ( it->getDistance() < 0)
      {
        printf( "beacon id = %s distance = %lf < 0\n", 
                it->getBeaconId(), it->getDistance() );
      
        return ERROR_IN_TRILATER;
      }

      //calc sum to get average
      rssi     += it->getRssi();
      distance += it->getDistance();
     
      //clear duplicate entries
      // we don't clear first occurrence!
      if (nOfMeasFromSameAp != 1)
        it = beaconMeas.erase( it );
      else it++;
 
      if (it == beaconMeas.end())
        break;
      it = std::find( it, beaconMeas.end(), *itBeaconMeas );
    }
 
    if (nOfMeasFromSameAp == 0)
    {
      setErrorMessage("number of measurements from the same AP == 0 ! something wrong!\n");
      printf( "%s\n", getErrorMessage() );
      return ERROR_IN_TRILATER;
    }
 
    //set average rssi to the beacon that doesn't have duplicates now
    rssi     /= (std::max)(nOfMeasFromSameAp, 1);
    distance /= (std::max)(nOfMeasFromSameAp, 1);
     
    itBeaconMeas->setRssi( rssi );
  }

  if (beaconMeas.size() < 3)
  {
    setErrorMessage( "less then 3 visible beacons in measurements "
                     "It's not enough for trilateration\n" );
    printf( "%s\n", getErrorMessage() );
    return ERROR_NO_SOLUTION_TRILATERATION;
  }

  return 0;
}


void Trilateration::getLinearSystem( vector<double> &matrixA,
                                     vector<double> &b,
                                     int dim )
{
  int nOfVisibleBeacons = mBeaconMeas.size ();
  
  BeaconMeas firstBeacon = mBeaconMeas.front ();
  double xFirstBeacon = 0, yFirstBeacon = 0;
  xFirstBeacon = mBeaconMeas[ 0 ].getBeaconPtr()->getX();
  yFirstBeacon = mBeaconMeas[ 0 ].getBeaconPtr()->getY();

  double firstBeaconDistance = mBeaconMeas.front().getDistance();
  double normaFirstBeacon = xFirstBeacon * xFirstBeacon +
    yFirstBeacon * yFirstBeacon;

  for ( int i = 0; i < nOfVisibleBeacons - 1; i++ )
  {
    // fill the matrix A and right part of linear system b
    double x = 0.0, y = 0.0;
    x = mBeaconMeas[ i + 1 ].getBeaconPtr()->getX();
    y = mBeaconMeas[ i + 1 ].getBeaconPtr()->getY();

    double distance = mBeaconMeas[ i + 1].getDistance();
    
    matrixA[ i * dim ] = 2 * (x - xFirstBeacon);
    matrixA[ i * dim  + 1 ] = 2 * ( y - yFirstBeacon );

    double norma = x * x + y * y;
    b[ i ] = firstBeaconDistance * firstBeaconDistance - distance * distance -
      normaFirstBeacon + norma;
  }

  return;
}


// This function estimate x,y coordinates by triangulation algorithm
// using ordinary least squares method for solving linear system
void Trilateration::getLeastSquaresCoordinates()
{
  // How many coordinates do we consider? In planar case it equal to 2
  int dim = 2;
  int nOfVisibleBeacons = mBeaconMeas.size();
  // create matrix for triangulation linear system, that we will solve
  // for obtaining the coordinates we need at least dim + 1 beacons
  // index [i][j] = i * dim + j

  // By subtracting the last equation from each other we bring our 
  // nonlinear system to linear matrixA
   
  vector <double> matrixA((nOfVisibleBeacons - 1) * dim, 0.0);
  vector <double> b(nOfVisibleBeacons - 1, 0.0 );

  getLinearSystem(matrixA, b, dim);
  solveLinearSystem (matrixA, b);
}


// this function solve overdetermined linear system 
// by ordinary least squares method x = A_ * B
// where A_ - pseudo inverse matrix
void Trilateration::solveLinearSystem( vector<double> matrixA,
                                                 vector <double> b )
{
  int nOfEquations = b.size();
  int dim = matrixA.size() / nOfEquations;

  vector <double> xy(dim, 0.);
  vector <double> aTransposeA(dim * dim, 0.);
  
  // find pseudoInverseMatrix
  for (int row = 0; row < dim; row++)
  {
    for (int col = 0; col < dim; col++)
    {
      for ( int inner = 0; inner < nOfEquations; inner++ ) 
      {
        // Multiply the row of A_transpose by the column of A 
        // to get the row, column of multiplyAATranspose.
        aTransposeA[ row * dim + col ] +=
          matrixA[ inner * dim + row ] * matrixA[ inner * dim + col ];
      }
    }
  }

  vector <double> revertMatrix( dim * dim, 0. );
  double det = aTransposeA[ 0 ] * aTransposeA[ 3 ] -
    aTransposeA[ 2 ] * aTransposeA[ 1 ];

  //simple formula for invert matrix 2x2
  revertMatrix[ 0 ] = aTransposeA[ 3 ] / det;
  revertMatrix[ 1 ] = -aTransposeA[ 1 ] / det;
  revertMatrix[ 2 ] = - aTransposeA[2] / det;
  revertMatrix[ 3 ] = aTransposeA[0] / det;

  //Multiply revertMatrix on A transpose
  vector <double> matrix2xN (dim * nOfEquations, 0.0);
  for ( int row = 0; row < dim; row++ )
  {
    for ( int col = 0; col < nOfEquations; col++ )
    {
      for ( int inner = 0; inner < dim; inner++ )
      {
        // Multiply the row of A_transpose by the column of A 
        // to get the row, column of multiplyAATranspose.
        matrix2xN[ row * nOfEquations + col ] +=
          revertMatrix[ row * dim + inner] * matrixA[ col * dim + inner ];
      }
    }
  }

  //Multiply matrix2xN on B vector 
  for ( int col = 0; col < dim; col++ )
  {
    for ( int inner = 0; inner < nOfEquations; inner++ )
    {
      xy[col] += matrix2xN[col * nOfEquations + inner] * b[inner];
    }
  }
  return;
}


void Trilateration::setErrorMessage(const char* errorMsg )
{
  Trilateration::mErrorMessage += errorMsg;
}


bool compareBeaconMeasByName( BeaconMeas first, BeaconMeas second )
{
  return strcmp( first.getBeaconId(), second.getBeaconId() ) < 0;
}


std::vector<Beacon>::const_iterator findBeaconForMeas( const vector<Beacon>& mapBeacons,
                                                       const string& measureBeaconId )
{
  std::vector<Beacon>::const_iterator it;
  for (it = mapBeacons.begin(); it != mapBeacons.end(); ++it)
  {
    if (it->getId() == measureBeaconId)
      return it;
  }
  return it;
}

