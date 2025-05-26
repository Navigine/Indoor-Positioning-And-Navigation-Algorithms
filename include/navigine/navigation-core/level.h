#pragma once

#include <map>
#include <string>
#include <vector>

#include <navigine/navigation-core/geometry.h>
#include <navigine/navigation-core/graph.h>
#include <navigine/navigation-core/measurement.h>
#include <navigine/navigation-core/radiomap.h>
#include <navigine/navigation-core/random.h>
#include <navigine/navigation-core/reference_point.h>
#include <navigine/navigation-core/transmitter.h>

namespace navigine::navigation_core {

class Level
{
public:
    Level(const LevelId& id, const GeoPoint& bindingPoint);

    void addTransmitters(const XYTransmitters& transmitters);
    void setReferencePoints(const XYReferencePoints& referencePoints);
    void setGraph(const XYGraph& graph);
    void setGeometry(const XYMultiPolygon& geometry);
    void setAltitude(const std::optional<double>& altitude);

    const LevelId& id() const;
    const GeoPoint& bindingPoint() const;
    const XYMultiPolygon& geometry() const;
    const XYGraph& graph() const;
    const Radiomap& radiomap() const;
    const std::optional<double>& altitude() const;

    const XYTransmitter* findTransmitter(const TransmitterId& txId) const;

    bool containsPoint(const XYPoint& point) const;
    bool containsSegment(const XYSegment& segment) const;
    bool containsGeoPoint(const GeoPoint& point) const;
    bool containsGeoSegment(const GeoSegment& segment) const;

    RadioMeasurementsData getRadioMeasurements(const RadioMeasurementsData& radioMsr) const;

    std::vector<XYPoint> samplePoints(unsigned int count, Rng* rng) const;

private:
    void buildTriangulation();

    const LevelId id_;
    const GeoPoint bindingPoint_;

    std::map<TransmitterId, XYTransmitter> transmitters_;
    XYMultiPolygon geometry_;
    XYGraph graph_;
    Radiomap radiomap_;
    std::optional<double> altitude_;

    std::unique_ptr<geolib::SpatialSearchTree<XYPoint>> tree_;

    std::vector<XYTriangle> triangulation_;
    std::vector<double> areaDistribution_;
};

} // namespace navigine::navigation_core
