Indoor-navigation-algorithms  
[![Build](https://github.com/Navigine/Indoor-Navigation-Algorithms/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/Navigine/Indoor-Navigation-Algorithms/actions/workflows/ubuntu.yml) [![Build](https://github.com/Navigine/Indoor-Navigation-Algorithms/actions/workflows/macos.yml/badge.svg)](https://github.com/Navigine/Indoor-Navigation-Algorithms/actions/workflows/macos.yml)
============================

## Useful Links

- Refer to the [Navigine official website](https://navigine.com/) for complete list of downloads, useful materials, information about the company, and so on.
- [Get started](http://client.navigine.com/login) with Navigine to get full access to Navigation services, SDKs, and applications.
- Refer to the Navigine [User Manual](http://docs.navigine.com/) for complete product usage guidelines.
- Find company contact information at the official website under [Contacts](https://navigine.com/contacts/) tab.

## Contents

This repository consists of the following components

* [src](/navigation-core/src/) - the source code of Navigine positioning algorithms

* [standalone_algorithms](/navigation-core/standalone_algorithms) - set of algorithms used in navigation

* [tests](/navigation-core/tests/) - test and utilities for the evaluation of source code quality

* [tools](/navigation-core/tools/verification/) - utilities for data extraction from map and settings

## Creating [Navigation Client](/navigation-core/include/navigation_client.h)

Here are some examples to give you an idea how to use Navigation Client

First, create variables that will store your data.
```c++
    // variables for storing location elements
    navigine::navigation_core::GeoLevel geoLevel;
    std::shared_ptr<navigine::navigation_core::LevelCollector> levelCollector;
    navigine::navigation_core::NavigationSettings navigationSettings;
    std::shared_ptr<navigine::navigation_core::NavigationClient> navClient;
```

Add transmitters (Beacon, Eddystone, WIFI, WIFI-RTT, etc.) as `GeoTransmitters` from your location.

Then they will be converted to `XYTransmitters` for internal evaluations
    
    Parameters:
        - id - identifier of transmitter like (major,minor,uuid) for beacon, (namespaceId,instanceId) for eddystone, mac for WIFI;
        - point - latitude and longitude as GeoPoint;
        - pathlossModel - A, B and power of transmitter as `PathlossModel` struct variable;
        - type - type of transmitter, like Beacon, Eddystone, WIFI, WIFI-RTT, etc..

Inside for loop add all the transmitters from your location. Here is an example of adding one element

```c++
    geoLevel.transmitters.emplace_back(transmitterId,
                              navigine::navigation_core::GeoPoint(latitude, longitude),
                              navigine::navigation_core::PathlossModel{A, B, power},
                              navigine::navigation_core::TransmitterType::BEACON);

```

Create geometry of the level using method `getGeometry` from [barriers_geometry_builder.h](/navigation-core/include/navigine/navigation-core/barriers_geometry_builder.h) file. Geometry could be created using the barriers and the size of the level
    
    Parameters:
        - barrierList - list of polygons, where each polygon describes the barrier of the level;
        - allowedArea - polygon, which is created using width and height of the map;

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

Quality of navigation can be evaluated via calculation of different navigational errors.

list of errors with description


## Navigation test

The example of navigation is presented in a file [Navigation Test](/navigation-core/tests/navigation_test.cpp).
