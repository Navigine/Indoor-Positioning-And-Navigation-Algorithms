The performance of indoor positioning system depends on correct description of building structure that is used for navigation and
correct installation of infrastructure such as various signals transmitters, beacons and base stations.

To describe the indoor environment we propose the API of indoor positioning algorithms described below.

## Navigation structures

Any indoor space, such as building or warehouse is further referred to as `Location` and is represented by a set of 2D (plane)
areas called `Level`'s. A typical example of `Location` is a multistored building, where each `Level`s corresponds to one of the floors of the building.

[TODO figure with the scheme of location with levels]

Each `Level` is specified by all the data, necessary to calculate position on this `Level`. This data icludes: geometry of the `Level` (walls, open/closed areas, bounding box), route graph (information about available routes on this `Level`), list of istalled devices, called `Transmitters` (bluetooth beacons, wifi base stations, etc) and collected radio map (if present) that is called RSSI fingerprints (TODO reference to RSSI positioning papers).
All these pieces of data will be described below.

[TODO figure with the scheme of the level]

All these infrastructure is described in the following struct:

```cpp
struct Level
{
  Id id;
  ...
  std::vector<Transmitter> transmitters;
  std::map<ReferencePoint::Id, ReferencePoint> referencePoints;
  Geometry geometry;
  Graph graph;
};
```


### Geometry

The physical structure of the `Level` is described by struct `Geometry` which includes boundind box `Box` of the `Level` area and the set of polygons of free space joint together in `Multipolygon` class. 
Bounding box is basically a rectangle in physical space defining the boundaries of the area, selected for positioning.
Allowed Area (`Multipolygon`) is necessary to introduce a natural restriction on possible movements inside the area of this `Level`. These restrictions maybe introduced by closed rooms, walls, holes, and etc. Each of such obstacle is presented by a `Polygon` in physical space. And altogether these obstacle are joined in a `Multipolygon` structure. During positioning, only the area, that is not covered by this `Multipolygon` is available for positioning.

 TODO: More detailed description of how to compose Multipolygon.

```cpp
  struct Geometry {
    Multipolygon allowedArea;
    Box boundingBox;
  };
```

`Multipolygon` here is just an alias for boost multi_polygon: `using Multipolygon = boost::geometry::model::multi_polygon<Polygon>;`

Most of positioning engines must provide the functionality of building Routes (between points A and B, or from current position to some destination point B) on top of positioning. To make it possible, `Level` should contain routing information. 
This information is presented by a set of physical points - `Vertices`, and a set of links (`Edged`) between them. A route can be built from point `A` to point `B` only if these points are connencted by a multi-link of `Edges`. Altogether, `Vertices` and `Edges` form a mathmetical structure called `Graph` of routes.

```cpp
struct Graph
{
  struct Vertex
  {
    double x;
    double y;
  };

  std::vector<Vertex> vertices;
  std::set<std::pair<int, int>> edges;
};
```


### Transmitters

The indoor positioning system can use different technologies to calculate position of a mobile object. One of the most popular technology is based on measurements of received signal strength from base stations or beacons with known positions to a mobile tag. The signal strength can then be transformed to distance to an object. The set of such distances with corresponding coordinates of base stations is used to perform thrilateration and to calculate the position.
The struct `Transmitter` is used to describe the type, the position and other properties of a base station or a beacon.

The essential parameters of each transmitter are: its position on Level (`x`,`y`) and its unique identificator `id`.
`Position` of device is essential as all solvers are based on measurements of distance (some of device can measure distanc explicitly, while other provide some other types of signal, that will be converted to distance using existing physical formulas) to the transmitter. Thus to get correct position estimate, one has to correctly specify the positions of all transmitters.
`ID` of device is essential as it is used to match the signal, received on the device side, to the specific transmitter from which this signal was emitted.

Other parameters include the type `Type` of the device and parameters, specific to different types of devices.

```cpp
struct Transmitter 
{
  enum class Type { WIFI, BEACON, BLUETOOTH, RTT, UNKNOWN };

  Id id;
  double  x;
  double  y;
  Type    type;
  ... //other parameters describing transmition power, etc ...
};

```
 

### RSS Fingerprints
Another way of calculation of position of a mobile type is to use radiomap of received signal strength of available stationary devices such as wifi hotspots and ble devices. This approach is used even when position of base stations are not known. The idea is based on ... TODO.
The set of all available signals is stored in struct `ReferencePoint`
To describe radiomap on certain `Level` the map of `ReferencePoint`  with corresponding ids is used 

