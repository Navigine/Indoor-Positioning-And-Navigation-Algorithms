/** radiomap.h
*
* Copyright (c) 2019 Navigine.
*
*/

#ifndef NAVIGINE_RADIOMAP_H
#define NAVIGINE_RADIOMAP_H

#include <map>

#include "reference_point.h"
#include "transmitter.h"

namespace navigine {
namespace navigation_core {

class Radiomap
{
public:
  Radiomap(const XYReferencePoints& referencePoints = {});

  const XYReferencePoints& referencePoints() const;

  ReferencePoint<XYPoint> getReferencePoint(const ReferencePointId& refPointId) const;
  bool hasReferencePoint(const ReferencePointId& refPointId) const;

private:
  XYReferencePoints mReferencePoints;
  std::map<ReferencePointId, size_t> mReferencePointsIndex;  // {id -> index}
};

} } // namespace navigine::navigation_core

#endif // NAVIGINE_RADIOMAP_H
