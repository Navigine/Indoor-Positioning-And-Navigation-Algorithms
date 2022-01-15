import pandas as pd
import matplotlib.pylab as plt
import numpy as np

df1 = pd.read_csv('./../test_data/simulation.log', sep=' ', index_col=None, names=['ts', 'angle', 'ax', 'ay', 'az', 'wx', 'wy', 'wz', 'mx', 'my', 'mz'])

df2 = pd.read_csv('./../test_data/calculated_angles.log', sep=' ', names=['ts', 'roll', 'pitch', 'yaw'])
t = (df1['ts'] - df1['ts'][0]) / 1000.0

plt.plot(t, 180 * df1.angle / 3.14, '-', label='simulated')
plt.plot(t, 180 * df2.roll / 3.14, '-', label='roll')
plt.plot(t, 180 * df2.pitch / 3.14, '-', label='pitch')
plt.plot(t, 180 * df2.yaw / 3.14, '-', label='yaw')
plt.legend()
plt.show()

