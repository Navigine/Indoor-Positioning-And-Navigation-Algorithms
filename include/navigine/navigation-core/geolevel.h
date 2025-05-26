#pragma once

#include <string>
#include <vector>
#include <map>

#include <navigine/navigation-core/geometry.h>
#include <navigine/navigation-core/graph.h>
#include <navigine/navigation-core/level.h>
#include <navigine/navigation-core/reference_point.h>
#include <navigine/navigation-core/transmitter.h>
#include <navigine/navigation-core/utils.h>

namespace navigine::navigation_core {

struct GeoLevel
{
    LevelId id;
    GeoTransmitters transmitters;
    GeoReferencePoints referencePoints;
    GeoMultiPolygon geometry;
    GeoGraph graph;
    std::optional<double> altitude;
};

typedef std::vector<GeoLevel> GeoLevels;

// Parse geolevels from geojson data.
// Throws std::runtime_error on parse error.
GeoLevels parseGeoLevelsFromData(const std::string& data);

// Parse geolevels from geojson file.
// Throws std::runtime_error on parse error.
GeoLevels parseGeoLevelsFromFile(const std::string& filename);

// Serialize geolevels
std::string dumpGeoLevelsToData(const GeoLevels& geoLevels);

// Dump geolevels to file
void dumpGeoLevelsToFile(const GeoLevels& geoLevels, const std::string& filename);

} // namespace navigine::navigation_core
