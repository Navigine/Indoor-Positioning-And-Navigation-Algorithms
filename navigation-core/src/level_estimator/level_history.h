/** level_history.h
 *
 * Author: Aleksandr Galov <asgalov@gmail.com>
 * Copyright (c) 2017 Navigine. All rights reserved.
 *
 */

#ifndef LEVEL_HISTORY_H
#define LEVEL_HISTORY_H

#include <vector>
#include <map>
#include "level.h"

namespace navigine {
namespace navigation_core {

class LevelHistory
{
  public:
    LevelHistory();
    LevelId detectLevelUsingHistory(const LevelId &detectedLevelId);
  
  private:
    const int mMaxLevelIdOccurrence;
    LevelId mCurrentLevelId;
    bool mIsInitialized;
    std::map<LevelId, int> mFloorCnt;
};

} } // namespace navigine::navigation_core

#endif // LEVEL_HISTORY_H
