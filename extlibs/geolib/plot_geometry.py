#!/usr/bin/env python3

from shapely import wkt
import os, sys
import matplotlib.pyplot as plt
import geopandas as gpd
import shapely.validation

s = None

if len(sys.argv) >= 2:
    print("Reading wkt-string from file '" + sys.argv[1] + "'")
    with open(sys.argv[1], 'r') as f:
        s = f.read().rstrip()
else:
    s = input("Input wkt-string: ")

print("\nAnalysing wkt-string: " + s + "\n")

try:
    g1 = wkt.loads(s)
except:
    print("Invalid wkt-string")
    sys.exit(1)

print(shapely.validation.explain_validity(g1))

p1 = gpd.GeoSeries(g1)
p1.plot()

plt.show()
