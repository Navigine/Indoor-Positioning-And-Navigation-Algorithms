### Complementary filter

Complementary filter fuses data from IMU (Intertial Measurement Units) sensors such as accelerometer, magnetometer, gyroscope and pedometer. Configuration of sensors that are in use during the position estimation process may vary depending on the quality of signal from sensors. For instance, it is common to work with accelerometer and gyroscope, rejecting other sensors if signal from corresponding sensors is noisy.

The example directory contains code that simulates IMU measuremetns of pendulum and estimates pendulum orientation with the complementary filter. It is possible to plot the result of orientation estimation with python script located at helpers directory.

### What is a complementary filter

The basic complementary filter is shown in a picture below.

<img src="../illustrations/complementary_filter.png"
     alt="Markdown Monster icon"/>

Where z is an input signal, x and y are noisy measurements of this signal. <img src="https://render.githubusercontent.com/render/math?math=\hat{z}%20\:%20is" title="\hat{z} \: is" /> an estimation of the output signal produced buy the filter. Assume that
the noise in y is mostly high frequency, and the noise in x is mostly low frequency. Then G(s) can be made a low-pass filter to filter out the high-frequency noise in y. If G(s) is low-pass, [1 - G(s)] is the complement, i.e., a high-pass filter which filters out the low-frequency noise in x. The complementary filter can be reconfigured as in figure B. In this case the input to G(s) is

<p align="center"><img src="https://render.githubusercontent.com/render/math?math=y%20-%20x%20=%20n_2%20-%20n_1%20\:%20," title="y - x = n_2 - n_1 \: ," /> 
 
 so that the filter G(s) just operates on the noise or error in the measurements x and y. In the case of noise less or error-free measurements

<p align="center"><img src="https://render.githubusercontent.com/render/math?math=\hat{z}%20=%20z[1%20-%20G(s)]%20%2b%20zG(s)%20=%20z," title="\hat{z} = z[1 - G(s)] + zG(s) = z," />


i.e., the signal is estimated perfectly.

Gyroscope obtains the angular position of the object by integrating the angular velocity over time. Gyroscope is a very precise sensor and not susceptible to external forces. But because of integration, the measurement tends to drift, not returning to zero when the system goes back to its original position. Accelerometer is a sensor which takes into consideration every small forse therefore it can be easily disturbed. That is why the accelerometer data is reliable only on a long term. The advantage of accelerometer is that it does not drift. A magnetometer is a device that measures magnetic field or magnetic dipole moment.

In case if the measurements from different sources give precise information in different frequency regions, it is convenient to use a weighted sum of measurements from these sources.

[//]: # "angle = alpha * (angle + gyrData * dt) + beta * accData + gamma * magnData)"

<p align="center"><img alt="angle = \alpha -10\mu \log \frac{d}{d_0} + w " src="https://render.githubusercontent.com/render/math?math=\phi%20=%20\alpha%20*%20(\phi%20%2b%20gyrData%20*%20dt)%20%2b%20\beta%20*%20accData%20%2b%20\gamma%20*%20magnData" title="\phi = \alpha * (\phi + gyrData * dt) + \beta * accData + \gamma * magnData" /></p>

where alpha, beta, gamma are constant weights of different measurement sources.

### Low and high pass filters

Low-pass filter is a filter that passes signals with a frequency lower than a selected cutoff frequency.
Transfer function of low-pass filter

[//]: # "https://en.wikipedia.org/wiki/Low-pass_filter"

<p align="center"><img src="https://render.githubusercontent.com/render/math?math=H(s)%20=%20\frac{\omega_0}{s%20%2b%20\omega_0}" title="H(s) = \frac{\omega_0}{s + \omega_0}" />

where <img src="https://render.githubusercontent.com/render/math?math=\omega_0" title="\omega_0" /> is a cutoff frequency of a filter

Low-pass filter is the complement of a high-pass filter. High-pass filter is a filter that passes signals with a frequency higher than a certain cutoff frequency.
Transffer function of high-pass filter

<p align="center"><img src="https://render.githubusercontent.com/render/math?math=H(s)%20=%20\frac{s}{s%20%2b%20\omega_0}" title="H(s) = \frac{s}{s + \omega_0}" />

Consider a first-order integrator

<p align="center"><img src="https://render.githubusercontent.com/render/math?math=\dot{x}%20=%20u" title="\dot{x} = u" />

with the folowing measurement characteristics

<p align="center"><img src="https://render.githubusercontent.com/render/math?math=y_x%20=%20L(s)x%20%2b%20\mu_x,%20y_u%20=%20u%20%2b%20\mu_u%20%2b%20b(t)" title="y_x = L(s)x + \mu_x, y_u = u + \mu_u + b(t)" />

Measurements <img src="https://render.githubusercontent.com/render/math?math=y_x%20\hspace{1mm}%20and%20\hspace{1mm}%20y_u" title="y_x \hspace{1mm} and \hspace{1mm} y_u" /> can be fused into an estimate <img src="https://render.githubusercontent.com/render/math?math=x_{est}" title="x_{est}" /> of a state <img src="https://render.githubusercontent.com/render/math?math=x" title="x" /> via the filter

<p align="center"><img src="https://render.githubusercontent.com/render/math?math=x_{est}%20=%20F_1(s)y_x%20%2b%20F_2(s)\frac{y_u}{s}" title="x_{est} = F_1(s)y_x + F_2(s)\frac{y_u}{s}" />

A filter is called a complementary filter if 
<p align="center"><img src="https://render.githubusercontent.com/render/math?math=F_1(s)%20%2b%20F_2(s)%20=%201" title="F_1(s) + F_2(s) = 1" />

### Comparison with Kalman filter

Complementary filter outperforms Kalman filter significantly by using less computational and processing power and providing more accuracy. The Complementary filter can be applied by having only vector and quaternion mathematical operators. On other hand, the traditional Kalman filter needs an enormous number of matrix operations, including multiplications and taking inverses of these matrices, which, besides the complexity, also results in high computational and processing costs.

### Calculations

Acceleration is stored in quaternion form. The weight part of it is a numeric value of the acceleration, the vector part -- vector components.

The goal of developing an estimator is to provide a smooth estimate <img src="https://render.githubusercontent.com/render/math?math=\hat{R(t)}%20\in%20SO(3)" title="\hat{R(t)} \in SO(3)" /> of a state R(t) that is evolving due
to some external input based on a set of measurements.

Frame of reference allows to determine the error. It can be calculated as follows

<p align="center"><img src="https://render.githubusercontent.com/render/math?math=\dot{\hat{R}}%20=%20R^{*}%20*%20R" title="\dot{\hat{R}} = R^{*} * R" />

<p align="center"><img src="https://render.githubusercontent.com/render/math?math=err%20=%20acc%20\times%20\dot{\hat{R}}" title="err = acc \times \dot{\hat{R}}" />

The direct complementary ﬁlter dynamics are speciﬁed by

<p align="center"><img src="https://render.githubusercontent.com/render/math?math=\dot{\hat{R}}%20=%20(R\Omega%20*%20R\omega)\times%20\hat{R}" title="\dot{\hat{R}} = (R\Omega * R\omega)\times \hat{R}" />

The R* operation is an inverse operation on SO(3)

The kinematics can be written directly in terms of the quaternion representation of SO(3) by

<p align="center"><img src="https://render.githubusercontent.com/render/math?math=\dot{q}%20=%20q%20\times%20p(\Omega)" title="\dot{q} = q \times p(\Omega)" />

Quaternion is multiplied by the pure rotation quaternion <img src="https://render.githubusercontent.com/render/math?math=p(\Omega)" title="p(\Omega)" />, where real part is equal to zero.
In the source code provided 'updateQuaternion(mQ, rotation)' function which does the same calculations.

The gyroscope data is integrated every timestep with the current angle value:

```cpp
omega = gyro * dt + mIntegralError * dt;
```

### Components

The following constant coefficients are the weights applied to accelerometer, gyroscope and magnetometer components.

```cpp
double     mKaccelerometer;
double     mKmagnetometer;
double     mKintegralGain;
```

### Build

To build the project for running tests make sure that `BUILD_TESTS` option is turned on.

```sh
cd /standalone_algorithms/complementary_filter
cmake -Bbuild -H.
cmake --build build
```

Run tests:

```sh
./build/test-filter
```

### Results of orientation

<img src="../illustrations/orientation_estimation.png"
     alt="Markdown Monster icon"/>
