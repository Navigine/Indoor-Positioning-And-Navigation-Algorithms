# K-Nearest Neighbours Positioning Algorithm


Note:: The algorithm requires special radiomap data measurement that can beperformed using procedure called Radiomap Measurement.
For detailed information of data collection for algorithm usage refer to [https://docs.navigine.com/Measuring_Radiomap](https://docs.navigine.com/Measuring_Radiomap)

Radiomap Measurement:: measuring the target location's radiomap on foot via an Android device with the [Navigine Application](https://github.com/Navigine/Indoor-Navigation-Android-Mobile-SDK-2.0) installed.

*Requirements*:
* infrastructure deployed
* maps (locations) implemented
* [radiomap data](https://docs.navigine.com/Measuring_Radiomap) collected
*Our notation*:
* RSSI
* KNN
* transmismitter
* beacon
* BLE
* radio map
## Measurements & context

Algorithm is based on an RSS mapping technique. For the set of locations in an observation area with an apriory known positions we record RSSI measurements.

All observations from multiple positions we call a radio map. From radio map we can estimate user position by measuring similarity between observations and observations measured from a known prior location.

<!-- The positions of beacons are not known to the program. -->
<!--
Radio Map can give us information on our position based on measurements similarity. -->

<!-- weighted centroid approach -->

<!-- The measuring process was repeated 30 times for each of
the 68 test point locations resulting in a total amount of
24 480 RSS values collected-->

<!-- /The achieved measurement re-
peatability -->

The algorithms implies averaging on a radio map data, measuring and processing incoming RSSI signals from transmiters to receiver, and a signal propagation model, used for localization.

### What means **access point** is KNN positioning context

We measure incoming RSSI in several points of building for a long enough time.
After several minutes of collecting the signal, we perform averaging of signals. After that we have a precise measurements for a single point. Decent radio-map consists of several access points mapped to a building map.

The "access point" is the landmark, point with known coordinates and a history of observations from each beacon in visible range.

### KNN logfile and map contents

![radio-map example](img/3-10-2021-21-50-31-PM.png)

On figure above we see GeoJSON visualization containing some access points (circles).

The structure of data used for this algorithm is as follows:

```json
{
    "type": "Feature",
    "properties": {
        "name": null,
        "type": "reference_point",
        "level": "5179",
        "id": "1",
        "uuid": "eff14dbc-b33a-11e8-945d-7c7a91af7e27",
        "entries": [
            {
                "value": [
                    {
                        "rssi": 83,
                        "count": 6
                    },
                    {
                        ...
                    },
                ],
                "bssid": "20:18:00:00:04:32",
                "type": "WIFI"
            },
            {
                ...
            }
        ]
    },
    "geometry": {
        "type": "Point",
        "coordinates": [
            -1.6307104042,
            47.225793733499998
        ]
    }
}
```

For each entity of "type": "reference_point" we have many observations for each detected beacon in area.

Approximately we can say:
The example log will consist of 20-100 access point locations, 10000 RSSI observations from 20-50 detected beacons for a single location.

We have set of reference points, for each of them RSSI data from closest beacons in area are measured. This is the starting point of an algorithm.

## KNN algorithm setup

The settings we use for program configuration:

```json
"Use_algorithm": "KNN",
"Use_triangles": true,
"Min_num_of_measurements_for_position_calculation": 6,
"Use_diff_mode": true,
"K": 3,
```

We have two mode options for this algorithm called tringle and differential modes respectively.

<!-- Averaging signal for a long time we reduce measurement noise. -->

<!--
we skip RPs with 0 weights because they are most improbable

but if all signals coincide to map, we should add small additive

TODO:: Coefficient to find weight of AP (See Accuracy of RSS-Based Centroid Localization -->

## The algorithm

### KNN algorithm math and general concept

KNN approach is based on a radio-channel propagation model:

<p align="center"><img alt="RSS(d) = RSS(d_0)-10\mu \log \frac{d}{d_0} + w " src="https://render.githubusercontent.com/render/math?math=RSS%28d%29%20%3D%20RSS%28d_0%29-10%5Cmu%20%5Clog%20%5Cfrac%7Bd%7D%7Bd_0%7D%20%2B%20w%20"/></p>

The goal of an RSS-based localization algorithm is to provide estimate <img alt="\hat p = (\hat x, \hat y)" src="https://render.githubusercontent.com/render/math?math=%5Chat%20p%20%3D%20%28%5Chat%20x%2C%20%5Chat%20y%29" style="transform: translateY(20%);" />
of position <img alt="p" src="https://render.githubusercontent.com/render/math?math=p" style="transform: translateY(20%);" /> given the vector <img alt="[RSS_1, RSS_2, . . . ,  RSS_n]" src="https://render.githubusercontent.com/render/math?math=%5BRSS_1%2C%20RSS_2%2C%20.%20.%20.%20%2C%20%20RSS_n%5D" style="transform: translateY(20%);" />.

There exist several algorithms that can be used to determine
the position of a target through RSS measurements: Some
of them are geometric methods, such as lateration or a
minimum—maximum method (min—max), whereas some
others are based on statistical approaches, such as maximum
likelihood.

The algorithms for localization can be classified as either statistical of geometrical.

When the target node communicates with all the anchors, i.e., all anchors are visible, the centroid results the center of the anchors coordinates.

If we assume that all anchors we record were in the line of sight, or equally we asseme that there is no signal reflections in the model. If we have to consider noise from reflections and reflections model, the algorithm will be much more complicated. For simplicity, we work with a classical log-linear signal propagation model.

Anchors - beacons transmitting the signal.

<p align="center"><img alt="\hat p = \frac{1}{m}\cdot \sum_{i=1}^m {a_i} " src="https://render.githubusercontent.com/render/math?math=%5Chat%20p%20%3D%20%5Cfrac%7B1%7D%7Bm%7D%5Ccdot%20%5Csum_%7Bi%3D1%7D%5Em%20%7Ba_i%7D%20"/></p>

If we have <img alt="m" src="https://render.githubusercontent.com/render/math?math=m" style="transform: translateY(20%);" /> different anchor points in our collected data - cardinality of visible subset. (under assumption that all the visible anchors are equally close to target node).

### Implementation

#### Initialization step
* collect measurements
* construct building radio map
Firstly we organise the recorded data, write set of detected anchors to their respective sublocations.

We map radiomap data to each detected beacon(*reverse mapping* of radiomap data). Thus later during localization process, we may get closest reference points from an observed set of beacons at a zero computational cost.

#### RP weights estimation step
* get measurements
* get reference points for the respective location
* filter outliers
* stop until enough measurements collected
* calculate weights of RPs based on RSS measurements and building radio map
* calculate position based on known RPs weights and positions (see below)
We select the level / sublocation for localization.
For sublocation we select: map, known reference points and a set of detected beacons. From beacon to radio data map we obtain positions of most similar reference points.

We assume that reference points with the most similar signal are also the closest one, because of our signal prpagation model.

#### Position calculation

The pose estimation from weighted set of closest reference points can be performed in different ways: geometrically, statistically, etc.

Based on our settings, we have two options:

##### Weighted centroid approach

The position is the weighted mean of closest reference points with respect to their likelihood / signal similarity.

<p align="center"><img alt="\hat x = \sum_{i=1}^{n}{x_i \cdot w_i};\\
 \hat y = \sum_{i=1}^{n}{y_i \cdot w_i}
" src="https://render.githubusercontent.com/render/math?math=%5Chat%20x%20%3D%20%5Csum_%7Bi%3D1%7D%5E%7Bn%7D%7Bx_i%20%5Ccdot%20w_i%7D%3B%5C%5C%0A%20%5Chat%20y%20%3D%20%5Csum_%7Bi%3D1%7D%5E%7Bn%7D%7By_i%20%5Ccdot%20w_i%7D%0A"/></p>

Where <img alt="n" src="https://render.githubusercontent.com/render/math?math=n" style="transform: translateY(20%);" /> is the cardinality of closest reference points set.

##### Triangulation
* Triangulate our reference points set.
* Select triange with maximum cumulative weight: <img alt="w_i = w_1 + w_2 + w_3" src="https://render.githubusercontent.com/render/math?math=w_i%20%3D%20w_1%20%2B%20w_2%20%2B%20w_3" style="transform: translateY(20%);" />
From 3 closest reference points we select the triangle; Based on our signal model we calculate the position.

<p align="center"><img alt="\hat x = \sum_{i=1}^{3}{x_i \cdot w_i};\\
 \hat y = \sum_{i=1}^{3}{y_i \cdot w_i}
" src="https://render.githubusercontent.com/render/math?math=%5Chat%20x%20%3D%20%5Csum_%7Bi%3D1%7D%5E%7B3%7D%7Bx_i%20%5Ccdot%20w_i%7D%3B%5C%5C%0A%20%5Chat%20y%20%3D%20%5Csum_%7Bi%3D1%7D%5E%7B3%7D%7By_i%20%5Ccdot%20w_i%7D%0A"/></p>
* Pros:: direct calclation with low number of reference points required. Robust solution.
* Cons:: we don't consider information from all other reference points, our solution can be geometrically wrong and we have no options to tune the solution.
##### Discussion

The effective number of nearest neighbours needed for lacation estimation can be different. For some cases we want to choose from 3 optimal closest points, for some cases we prefer to have averaging. All logic comes from observations, noise, location properties and optimization procedure.

If during the measurement procedure, average visible number of beacons for each access points was high, we can possibly improve our accuracy. And if there's multiple signal reflections, we may want to localize only on 3 most strong signals and rejecting all noisy information.

## Preview & examples


Demo video for KNN in triangulation mode:

https://user-images.githubusercontent.com/68880242/139697439-7cffb9a0-e8cf-4f52-8ba3-f40350acbd14.mov

