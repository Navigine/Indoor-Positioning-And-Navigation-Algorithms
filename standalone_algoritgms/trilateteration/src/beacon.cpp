/** beacon.cpp
 *
 * Author: Aleksei Smirnov <aleksei.smirnov@navigine.ru>
 * Copyright (c) 2014 Navigine. All rights reserved.
 *
 */

#include "beacon.h"

#include <stdio.h>

using namespace std;

Beacon::Beacon()
: mX(0.)
, mY(0.)
, mLocationId(0)
{

}


//copy constructor
Beacon::Beacon(const Beacon& beacon)
: mX            (beacon.mX)
, mY            (beacon.mY)
, mId           (beacon.mId)
, mName         (beacon.mName)
, mLocationName (beacon.mLocationName)
, mLocationId   (beacon.mLocationId)
{

}


void Beacon::setBeaconId(const string& beaconId)
{
  Beacon::mId = beaconId;
}


void Beacon::fillData(const double x,
                      /*planar coordinate: x */ const double y,
                      /*planar coordinate: y */ const std::string& id,
                      /*beacon identifier */ const std::string& name,
                      /*beacon name */ const std::string& locName)
{
  Beacon::mX = x;
  Beacon::mY = y;
  Beacon::mId = id;
  Beacon::mName = name;
  Beacon::mLocationName = locName;
}


const char* Beacon::getId () const
{
  return mId.c_str();
}


double Beacon::getX() const
{
  return Beacon::mX;
}


double Beacon::getY( ) const
{
  return Beacon::mY;
}


void Beacon::setX( const double x )
{
  Beacon::mX = x;
}


void Beacon::setY( const double y )
{
  Beacon::mY = y;
}


void Beacon::setLocationName( const char* name )
{
  Beacon::mLocationName += name;
}


void Beacon::setLocationId( int sublocId )
{
  Beacon::mLocationId = sublocId;
}


int Beacon::getLocationId()
{
  return Beacon::mLocationId;
}


bool Beacon::operator==(const Beacon& entry)const
{
  return strcmp(getId(), entry.getId()) == 0;
}


bool Beacon::operator!=(const Beacon& entry)const
{
  return !(*this == entry);
}


IBeacon::IBeacon()
: Beacon()
, major(0)
, minor(0)
{

}


//copy constructor
IBeacon::IBeacon(const IBeacon& iBeacon)
: Beacon(iBeacon)
, uuid(iBeacon.uuid)
, major(iBeacon.major)
, minor(iBeacon.minor)
{
  
}


const char* IBeacon::getUuid ()const
{
  return uuid.c_str();
}


int IBeacon::getMajor()const
{
  return major;
}


int IBeacon::getMinor()const
{
  return minor;
}


void IBeacon::setMajor( const unsigned int major )
{
  IBeacon::major = major;
}


void IBeacon::setMinor( const unsigned int minor )
{
  IBeacon::minor = minor;
}


void IBeacon::setUuid( const char * uuid )
{
  IBeacon::uuid = uuid;
}


// constructor
BeaconMeas::BeaconMeas()
: mBeaconPtr (0)
, mRssi      (TRANSMITTER_POINT_UNUSED)
{

}


//constructor
BeaconMeas::BeaconMeas( Beacon* beacon,
                        double rssi,
                        double distance )
{
  BeaconMeas::mBeaconPtr = beacon;
  BeaconMeas::mRssi      = rssi;
  BeaconMeas::mDistance  = distance;
  mBeaconId              = beacon->getId(); 
}


//copy constructor
BeaconMeas::BeaconMeas(const BeaconMeas& beaconMeas)
: mBeaconId  (beaconMeas.mBeaconId)
, mBeaconPtr (beaconMeas.mBeaconPtr)
, mRssi      (beaconMeas.mRssi)
, mDistance  (beaconMeas.mDistance)
{

}


BeaconMeas::~BeaconMeas()
{
  mBeaconPtr = 0;
}


double BeaconMeas::getRssi()const
{
  return mRssi;
}


double BeaconMeas::getDistance()const
{
  return mDistance;
}


void BeaconMeas::setRssi(const double rssi)
{
  BeaconMeas::mRssi = rssi;
}


void BeaconMeas::setDistance(const double distance)
{
  BeaconMeas::mDistance = distance;
}


bool BeaconMeas::operator<(const BeaconMeas& entry)const
{
  return mDistance < entry.mDistance;
}


bool BeaconMeas::operator>(const BeaconMeas& entry)const
{
  return entry < *this;
}


bool BeaconMeas::operator==(const BeaconMeas& entry)const
{
  if (getBeaconId() == 0 || entry.getBeaconId() == 0)
  {
    printf ("ERROR: id of measurement == NULL!\n");
  }
  if ( *getBeaconId() == '\0' || *entry.getBeaconId() == '\0')
    printf("ERROR: id is empty\n");

  return strcmp( getBeaconId(), entry.getBeaconId() ) == 0;
}


bool BeaconMeas::operator!=(const BeaconMeas& entry)const
{
  return !(*this == entry);
}


// specify Ptr that correspond to beacon from wich we got *this measurement
void BeaconMeas::setBeaconPtr( const Beacon* beaconPtr )
{
  BeaconMeas::mBeaconPtr = const_cast<Beacon*> (beaconPtr);
}


Beacon* BeaconMeas::getBeaconPtr() const
{
  if (BeaconMeas::mBeaconPtr == NULL)
    printf( "beaconId = %s : mBeaconPtr == NULL\n", BeaconMeas::getBeaconId() );
    //throw exception here

  return BeaconMeas::mBeaconPtr;
}


void BeaconMeas::setBeaconId( const string& beaconId )
{
  BeaconMeas::mBeaconId = beaconId;
}


const char* BeaconMeas::getBeaconId() const
{
  return mBeaconId.c_str();
}

