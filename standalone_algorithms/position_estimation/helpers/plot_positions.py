import pandas as pd
import matplotlib.pylab as plt

df_calculated = pd.read_csv('../logs/output.log', sep=' ', names=['ts', 'x', 'y', 'r'])
df_benchmark = pd.read_csv('../logs/benchmarks.log', sep=' ', names=['x', 'y', 'ts', 'ori', 'subloc'])

plt.gca().set_aspect('equal')
plt.plot(df_calculated.x, df_calculated.y, '.-', label='calculated positions')
plt.plot(df_benchmark.x, df_benchmark.y, '-', label='reference trace')

plt.legend(bbox_to_anchor=(1.0, -0.25))
plt.show()

