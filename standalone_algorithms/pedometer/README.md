### Pedometer

The pedometer estimates steps based on accelerometer measurements collected from the user device.

The algorithm analyses three input acceleration components $(a_x, \:a_y, \:a_z)$ and the time stamp.
For details on the algorithm implementation, please refer to the corresponding unit test in GitHub.

### Algorithm

On each line of a log file there are timestamp and three input acceleration components $(a_x, \:a_y, \:a_z)$.
The process starts with reading raw acceleration data. Since the data could contain noise, it is processed to get rid of any noise. Magnitudes are calculated with low-passed filter and the average magnitude values are also computed according to the time interval for average value estimation. Once filtered, the noise reduced acceleration data is analyzed to detect whether a person is walking. Step counting is only performed if a walk is detected. Otherwise, the system goes back to reading the next window of acceleration data.

If walking is detected, step counting is performed using a zero crossing technique. Signal, crossing the zero mark once in the negative direction followed by the same action but in the positive direction, can be observed. This phenomena is called zero crossing, and it can be used for step counting.

Even after filtering there is still some unnecessary noise in the measurements. Zero crossing is considered
only if the signal has come from outside this range, otherwise it is ignored.

### Build

To build the project for running tests make sure that `BUILD_TESTS` option in CMakeLists.txt is turned on.

To build the project for running example turn `BUILD_EXAMPLES` on.

```sh
cd /standalone_algorithms/pedometer && rm -rf build
cmake -Bbuild -H.
cmake --build build
```

Run tests:

```sh
./build/test-filter
```

Run example:

```sh
./build/pedometer logs/HuaweiLong1.log
```
