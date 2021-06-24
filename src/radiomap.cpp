/** radiomap.cpp
 *
 * Copyright (c) 2019 Navigine.
 *
 */

#include <navigine/navigation-core/radiomap.h>

namespace navigine {
namespace navigation_core {

Radiomap::Radiomap(const XYReferencePoints& referencePoints)
{
  mReferencePoints = referencePoints;
  for (std::size_t i = 0; i < referencePoints.size(); i++)
    mReferencePointsIndex.insert({referencePoints[i].id, i});
}

const XYReferencePoints& Radiomap::referencePoints() const
{
  return mReferencePoints;
}

bool Radiomap::hasReferencePoint(const ReferencePointId& refPointId) const
{
  return mReferencePointsIndex.find(refPointId) != mReferencePointsIndex.end();
}

ReferencePoint<XYPoint> Radiomap::getReferencePoint(const ReferencePointId& refPointId) const
{
  return mReferencePoints.at(mReferencePointsIndex.at(refPointId));
}

} } // namespace navigine::navigation_core

