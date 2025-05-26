#pragma once

#include <list>
#include <memory>

#include <navigine/navigation-core/geolevel.h>
#include <navigine/navigation-core/level.h>

namespace navigine::navigation_core {

class LevelCollector
{
public:
    virtual ~LevelCollector() = default;

    void addGeoLevel(const GeoLevel& geoLevel);

    const Level* findLevel(const LevelId& levelId) const;

    using LevelVisitor = std::function<void(const std::shared_ptr<Level>&)>;
    using ConstLevelVisitor = std::function<void(const std::shared_ptr<const Level>&)>;

    // Apply function to all levels in level index
    void traverse(const ConstLevelVisitor& visitor) const;

private:
    std::map<LevelId, std::shared_ptr<Level>> levels_;
};

std::shared_ptr<LevelCollector> createLevelCollector();

} // namespace navigine::navigation_core
