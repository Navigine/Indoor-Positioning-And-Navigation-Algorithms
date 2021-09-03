Indoor-navigation-algorithms  
[![Build](https://github.com/Navigine/Indoor-Navigation-Algorithms/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/Navigine/Indoor-Navigation-Algorithms/actions/workflows/ubuntu.yml) [![Build](https://github.com/Navigine/Indoor-Navigation-Algorithms/actions/workflows/macos.yml/badge.svg)](https://github.com/Navigine/Indoor-Navigation-Algorithms/actions/workflows/macos.yml)
============================

* extlibs/ - the source code of third-party libraries on which NavigineSDK depends

* src/ - the source code of Navigine positioning algorithms

* test/ - test and utilities to check the code source quality

## Creating [Navigation Client](/navigation-core/include/navigation_client.h)

Create variables first, which will store your data.
```c++
    /*
    ..
        Create variables, which will hold your location elements
    ..
    */ 
    navigine::navigation_core::GeoLevel geoLevel;
    std::shared_ptr<navigine::navigation_core::LevelCollector> levelCollector;
    navigine::navigation_core::NavigationSettings navigationSettings;
    std::shared_ptr<navigine::navigation_core::NavigationClient> navClient;
```

Add transmitters(Beacon, Eddystone, WIFI, WIFI-RTT, etc.) as `GeoTransmitters` from your location (then they will be converted to `XYTransmitters` for internal evaluations)
    
    Parameters:
        - id - identifier of transmitter like (major,minor,uuid) for beacon, (namespaceId,instanceId) for eddystone, mac for WIFI;
        - point - latitude and longitude as GeoPoint;
        - pathlossModel - A, B and power of transmitter as `PathlossModel` struct variable;
        - type - type of transmitter, like Beacon, Eddystone, WIFI, WIFI-RTT, etc..

```c++
    /*
    ..
        Inside for loop add all the transmitters from your location
        Here is the example of adding one element
    ..
    */
    geoLevel.transmitters.emplace_back(transmitterId,
                              navigine::navigation_core::GeoPoint(latitude, longitude),
                              navigine::navigation_core::PathlossModel{A, B, power},
                              navigine::navigation_core::TransmitterType::BEACON);

```

Create geometry of the level using method getGeometry from `barriers_geometry_builder.h` file. Geometry could be created using the barriers and the size of the level
    
    Parameters:
        - barrierList - list of polygons, where each polygon describes the barrier of the level;
        - allowedArea - polygon, which created using width and height of the map;

```c++

    // create the list of Polygons which will describe all the barriers
    // example from navigate.cpp in Android repository
    std::list<navigine::navigation_core::Polygon> barriersList;
    for (size_t i = 0; i < barriers.size(); ++i)
    {
        auto coords = barriers.at(i);
        navigine::navigation_core::Polygon barrier;
        for (const auto& coord : coords)
            boost::geometry::append(barrier, navigine::navigation_core::Point(coord.first, coord.second));

        barriersList.push_back(barrier);
    }

    // create the polygon of allowed area
    // example from navigate.cpp in Android repository
    navigine::navigation_core::Polygon levelArea;
    auto boundingBox = navigation_core::Box(navigation_core::Point(leftMin.latitude, leftMin.longitude), navigation_core::Point(rightMax.latitude, rightMax.longitude));
    boost::geometry::convert(boundingBox, allowedArea);

    geoLevel.geometry = navigine::navigation_core::getGeometry(barriersList, levelArea);
```

Create `LevelCollector` using method `createLevelCollector` and add all your geo levels.

```c++
    levelCollector = navigine::navigation_core::createLevelCollector();
    levelCollector->addGeoLevel(geoLevel);
```

Create `NavigationSettings`, with two parameters - level settings and common settings. (You can find them in navigation.xml i.e.)

Create `NavigationClient` using method `createNavigationClient` which will take as arguments level collector and navigation settings.

```c++
    navClient = navigine::navigation_core::createNavigationClient(levleCollector, navigationSettings);
```

## Evaluate the navigation

Will be added.
