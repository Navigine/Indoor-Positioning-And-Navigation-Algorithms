### Complementary filter

The complementary filter estimates orientation based on accelerometer, magnetometer and gyroscope measurements collected from the user device.

The example directory contains code that simulates IMU measuremetns of pendulum and estimates pendulum orientation with the complementary filter. It is possible to plot the result of orientation estimation with python script located at helpers directory.

### What is a complementary filter

Gyroscope obtains the angular position of the object by integrating the angular velocity over time. Gyroscope is a very precise sensor and not susceptible to external forces. But because of integration, the measurement tends to drift, not returning to zero when the system goes back to its original position. Accelerometer is a sensor which takes into consideration every small forse therefore it can be easily disturbed. That is why the accelerometer data is reliable only on a long term. The advantage of accelerometer is that it does not drift. A magnetometer is a device that measures magnetic field or magnetic dipole moment.

In case if the measurements from different sources give precise information in different frequency regions, it is convenient to use a weighted sum of measurements from these sources.

[//]: # "angle = alpha * (angle + gyrData * dt) + beta * accData + gamma * magnData)"

<p align="center"><img alt="angle = \alpha -10\mu \log \frac{d}{d_0} + w " src="https://latex.codecogs.com/svg.image?\phi&space;=&space;\alpha&space;*&space;(\phi&space;&plus;&space;gyrData&space;*&space;dt)&space;&plus;&space;\beta&space;*&space;accData&space;&plus;&space;\gamma&space;*&space;magnData" title="\phi = \alpha * (\phi + gyrData * dt) + \beta * accData + \gamma * magnData" /></p>

where alpha, beta, gamma are constant weights of different measurement sources.

### Low and high pass filters

Low-pass filter is a filter that passes signals with a frequency lower than a selected cutoff frequency.
Transfer function of low-pass filter

[//]: # "https://en.wikipedia.org/wiki/Low-pass_filter"

<p align="center"><img src="https://latex.codecogs.com/svg.image?H(s)&space;=&space;\frac{\omega_0}{s&space;&plus;&space;\omega_0}" title="H(s) = \frac{\omega_0}{s + \omega_0}" />

where <img src="https://latex.codecogs.com/svg.image?\omega_0" title="\omega_0" /> is a cutoff frequency of a filter

Low-pass filter is the complement of a high-pass filter. High-pass filter is a filter that passes signals with a frequency higher than a certain cutoff frequency.
Transffer function of high-pass filter

<p align="center"><img src="https://latex.codecogs.com/svg.image?H(s)&space;=&space;\frac{s}{s&space;&plus;&space;\omega_0}" title="H(s) = \frac{s}{s + \omega_0}" />

Consider a first-order integrator

<p align="center"><img src="https://latex.codecogs.com/svg.image?\dot{x}&space;=&space;u" title="\dot{x} = u" />

with the folowing measurement characteristics

<p align="center"><img src="https://latex.codecogs.com/svg.image?y_x&space;=&space;L(s)x&space;&plus;&space;\mu_x,&space;y_u&space;=&space;u&space;&plus;&space;\mu_u&space;&plus;&space;b(t)" title="y_x = L(s)x + \mu_x, y_u = u + \mu_u + b(t)" />

Measurements <img src="https://latex.codecogs.com/svg.image?y_x&space;\hspace{1mm}&space;and&space;\hspace{1mm}&space;y_u" title="y_x \hspace{1mm} and \hspace{1mm} y_u" /> can be fused into an estimate <img src="https://latex.codecogs.com/svg.image?x_{est}" title="x_{est}" /> of a state <img src="https://latex.codecogs.com/svg.image?x" title="x" /> via the filter

<p align="center"><img src="https://latex.codecogs.com/svg.image?x_{est}&space;=&space;F_1(s)y_x&space;&plus;&space;F_2(s)\frac{y_u}{s}" title="x_{est} = F_1(s)y_x + F_2(s)\frac{y_u}{s}" />

A filter is called a complementary filter if 
<p align="center"><img src="https://latex.codecogs.com/svg.image?F_1(s)&space;&plus;&space;F_2(s)&space;=&space;1" title="F_1(s) + F_2(s) = 1" />

### Calculations

Acceleration is stored in quaternion form. The weight part of it is a numeric value of the acceleration, the vector part -- vector components.

The goal of developing an estimator is to provide a smooth estimate <img src="https://latex.codecogs.com/svg.image?\hat{R(t)}&space;\in&space;SO(3)" title="\hat{R(t)} \in SO(3)" /> of a state R(t) that is evolving due
to some external input based on a set of measurements.

Frame of reference allows to determine the error. It can be calculated as follows

<p align="center"><img src="https://latex.codecogs.com/svg.image?\dot{\hat{R}}&space;=&space;R^{*}&space;*&space;R" title="\dot{\hat{R}} = R^{*} * R" />

<p align="center"><img src="https://latex.codecogs.com/svg.image?err&space;=&space;acc&space;\times&space;\dot{\hat{R}" title="err = acc \times \dot{\hat{R}" />

The direct complementary ﬁlter dynamics are speciﬁed by

<p align="center"><img src="https://latex.codecogs.com/svg.image?\dot{\hat{R}}&space;=&space;(R\Omega&space;*&space;R\omega)\times&space;\hat{R}" title="\dot{\hat{R}} = (R\Omega * R\omega)\times \hat{R}" />

The R* operation is an inverse operation on SO(3)

The kinematics can be written directly in terms of the quaternion representation of SO(3) by

<p align="center"><img src="https://latex.codecogs.com/svg.image?\dot{q}&space;=&space;q&space;\times&space;p(\Omega)" title="\dot{q} = q \times p(\Omega)" />

Quaternion is multiplied by the pure rotation quaternion <img src="https://latex.codecogs.com/svg.image?p(\Omega)" title="p(\Omega)" />, where real part is equal to zero.
In the source code provided 'updateQuaternion(mQ, rotation)' function which does the same calculations.

The gyroscope data is integrated every timestep with the current angle value:

`omega = gyro * dt + mIntegralError * dt;`

### Components

The following constant coefficients are the weights applied to accelerometer, gyroscope and magnetometer components.

```
    double     mKaccelerometer;
    double     mKmagnetometer;
    double     mKintegralGain;
```

### Build

To build the project for running tests make sure that `BUILD_TESTS` option is turned on.

```
cd /standalone_algorithms/complementary_filter
cmake -Bbuild -H.
cmake --build build
```

Run tests:

```
./build/test-filter
```

### Results of orientation

<img src="../illustrations/orientation_estimation.png"
     alt="Markdown Monster icon"/>
