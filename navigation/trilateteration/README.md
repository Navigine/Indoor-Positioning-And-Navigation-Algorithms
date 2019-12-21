### Trilateration

This is a method of determining coordinates of a device using measurements of pseudo distances to all visible beacons or signal transmitters. For navigation purposes you may also operate with RSSI readings. We use this algorithm for indoor navigation along with the distance/RSSI measurements from Bluetooth LE 4.0 Beacons.

Algorithm with all required functions and data structures is presented as class in trilateration.h, beacon.h files.

Example of the algorithm usage and filling of data structures you can find in a unit test for this source code in the test_trilateration.cpp file.
