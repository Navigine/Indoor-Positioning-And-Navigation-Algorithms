/** test_pedometer.cpp
 *
 * Author: Aleksei Smirnov <aleksei.smirnov@navigine.com>
 * Copyright (c) 2014 Navigine. All rights reserved.
 *
 */

#if defined WIN32 || defined _WIN32
#define  _CRT_SECURE_NO_WARNINGS
#endif

#include "pedometer.h"
#include <iostream>

void PrintHelp()
{
  printf( "This is unit test that demonstrates how you can use \n"
          "Navigine pedometer class in order to detect steps and \n"
          "step lengths of the pedestrian on the basis of accelerometer \n"
          "measurements. If you have any questions please contact\n"
          "aleksei.smirnov@navigine.com \n\n"
          );
}


int main( void )
{
  PrintHelp();
  //set name of file with scenario that contain accelerometer measurements 
  //and their timestamps
  char *scenarioFileName = "../scenario/pedometer_scenario.txt";
  FILE *scenarioFile = fopen( scenarioFileName, "r" );
  if ( scenarioFile == 0)
  {
    printf ("ERROR opening %s file\n", scenarioFileName);
    return -1;
  }

  AccelComponents acc;
  std::vector<AccelComponents> accelerations;
  while (fscanf( scenarioFile, "%lf %lf %lf %lf", 
    &acc.mAccX, &acc.mAccY, &acc.mAccZ, &acc.mAccTime ) != EOF)
  {
    accelerations.push_back(acc);
  }

  Pedometer pedometer;
  pedometer.updateAccMeasurements(accelerations);
  pedometer.detectSteps();
  int nOfSteps = pedometer.getNumOfSteps();
  if ( nOfSteps != 24)
  {
    printf("Test wasn't passed\n");
    return -1;
  }
  else
  {
    printf( "Unit test was passed. Number of steps = %d\n", pedometer.getNumOfSteps() );
  }
 
  std::cin.get();
  return 0;
}