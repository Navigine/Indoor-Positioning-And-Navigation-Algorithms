
### Pedometer

The [pedometer algorithm](https://github.com/Navigine/Indoor-navigation-algorithms/tree/master/navigation/pedometer) implements a method of measuring the current positioning based on the information collected from the device's integrated modules such as:

- accelerometer
- magnetometer
- gyroscope

The algorithm analyses three input acceleration components (ax, ay, az) and the time stamp, and reports the amount of strides, time of each stride, and stride length as the output data.
For details on the algorithm implementation, refer to the [corresponding unit test in GitHub](https://github.com/Navigine/Indoor-navigation-algorithms/blob/master/navigation/pedometer/src/test_pedometer.cpp). 
The considered unit test reads acceleration data from file, fills in the data via the updateAccMeasurements method, and then processes it via the detectSteps method.
