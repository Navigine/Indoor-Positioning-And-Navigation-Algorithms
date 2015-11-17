<a href="http://navigine.com"><img src="https://navigine.com/wp-content/themes/flat-theme/assets/images/img/w_menuLogo.svg" align="left" height="60" width="180" hspace="10" vspace="5"></a>

# Indoor-navigation-algorithms

Here is our public repository with the main focus on indoor navigation algorithms. We’d like to share some useful links, introduce Navigine Demo Applications and Navigine Algorithms.
Navigine Indoor Location Services is a set of unified tools for indoor marketing, analysis, and tracking purposes. The tools’ application scope includes (and is not limited to) airports, shopping malls, universities, and so on.

## Useful Links

- Refer to the [Navigine official website](https://navigine.com/) for complete list of downloads, useful materials, information about the company, and so on.
- [Get started](http://client.navigine.com/login) with Navigine to get full access to Navigation services, SDKs, and applications.
- Refer to the Navigine [User Manual](http://docs.navigine.com/) for complete product usage guidelines.
- Find company contact information at the official website under <a href="https://navigine.com/contacts/">Contact</a> tab.

## Navigine Demo Applications

The following sections include short descriptions of the Navigine Demo Applications for iOS and Android operating systems. 

### Android Demo Application

Navigine Demo application for Android enables you to test indoor navigation as well as measure your target location's radiomap.

To get the Navigine demo application for Android,

- Either [download the precompiled APK file](https://github.com/Navigine/Android-SDK/blob/master/Navigine/Navigine-debug.apk).
- Or compile the application yourself [using sources, available at GitHub](https://github.com/Navigine/Android-SDK/tree/master/Navigine>).

For complete guidelines on using the Demo, refer to the [corresponding sections in the Navigine User Manual](http://docs.navigine.com/ud_android_demo.html), or refer to the Help file incorporated into the application.

### iOS Demo Application

Navigine demo application for iOS enables you to test indoor navigation that you set up using Navigine CMS.

To get the Navigine demo application for iOS, 

- Either find the [Navigine application in the Apple Store](https://itunes.apple.com/ru/app/navigine/id972099798) using your iOS device
- Or compile the application yourself [using source code, available at GitHub](https://github.com/Navigine/Indoor-navigation-algorithms/tree/master/demo%20apps/ios).

For complete guidelines on using the Demo, refer to the [corresponding sections in the Navigine User Manual](http://docs.navigine.com/ud_ios_demo.html), or refer to the Help file incorporated into the application.

## Navigine Algorithms

The following sections describe the algorithms implemented in the Navigine products. Refer to the [navigation](https://github.com/Navigine/Indoor-navigation-algorithms/tree/master/navigation) sub-folder for downloads.

### Pedometer

The [pedometer algorithm](https://github.com/Navigine/Indoor-navigation-algorithms/tree/master/navigation/pedometer) implements a method of measuring the current positioning based on the information collected from the device's integrated modules such as:

- accelerometer
- magnetometer
- gyroscope

The algorithm analyses three input acceleration components (ax, ay, az) and the time stamp, and reports the amount of strides, time of each stride, and stride length as the output data.
For details on the algorithm implementation, refer to the [corresponding unit test in GitHub](https://github.com/Navigine/Indoor-navigation-algorithms/blob/master/navigation/pedometer/src/test_pedometer.cpp). 
The considered unit test reads acceleration data from file, fills in the data via the updateAccMeasurements method, and then processes it via the detectSteps method.

Switch to the [corresponding sub-folder](https://github.com/Navigine/Indoor-navigation-algorithms/tree/master/navigation/pedometer) for downloads, or just to review the repository contents.

### Trilateration

This is a method of determining coordinates of a device using measurements of pseudo distances to all visible beacons or signal transmitters. For navigation purposes you may also operate with RSSI readings. We use this algorithm for indoor navigation along with the distance/RSSI measurements from Bluetooth LE 4.0 Beacons.

Algorithm with all required functions and data structures is presented as class in trilateration.h, beacon.h files.

Example of the algorithm usage and filling of data structures you can find in a unit test for this source code in the test_trilateration.cpp file.

Switch to the [corresponding sub-folder](https://github.com/Navigine/Indoor-navigation-algorithms/tree/master/navigation/trilateteration) for downloads, or just to review the repository contents.

## Navigation SDK

Refer to the [navigation_sdk sub-folder](https://github.com/Navigine/Indoor-navigation-algorithms/tree/master/navigation_SDK/Navigine_iOS_SDK) for header files used for iOS indoor navigation.