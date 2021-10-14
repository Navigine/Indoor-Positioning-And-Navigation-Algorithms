/** level.h
*
* Copyright (c) 2019 Navigine.
*
*/

#ifndef NAVIGINE_LEVEL_GEOMETRY_H
#define NAVIGINE_LEVEL_GEOMETRY_H

#include "boost_geometry_adaptation.h"
#include <vector>

namespace navigine {
namespace navigation_core {

  /**
 * @brief level geometry container
 * @details geometry container defines a map and a method to check if path is in allowed area
 * 
 */
  class LevelGeometry
  {
  public:
    LevelGeometry(){};
    /**
     * @brief Construct a new Level Geometry object containing a map data for a single location
     * 
     * @param allowedArea - {type}
     * @param box - {type} outer bounding box for given allowedArea
     */
    LevelGeometry(const Multipolygon &allowedArea, const Box &box);

    /**
     * @brief check for intersection between path and level barriers
     * 
     * @param path - {type} input curve, analyzed for intersection
     * @return true - path is NOT inside allowed area
     * @return false - path is IN allowed area
     */
    bool isPathTouchesBarriers(const Linestring &path) const;

    const Multipolygon &allowedArea() const;
    const Box &boundingBox() const;

  private:
    Multipolygon mAllowedArea;
    Box mBoundingBox;
    RTree mTree;
    std::vector<Ring> mInnerBarriers;
};

} } // namespace navigine::navigation_core

#endif // NAVIGINE_LEVEL_GEOMETRY_H
