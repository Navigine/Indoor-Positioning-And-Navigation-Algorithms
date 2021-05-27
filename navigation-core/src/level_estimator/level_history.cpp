/** level_history.cpp
 *
 * Author: Vasily Kosyanchuk <v.kosyanchuk@navigine.com>
 * Copyright (c) 2017 Navigine. All rights reserved.
 *
 */

#include "level_history.h"

namespace navigine {
namespace navigation_core {

static const int MAX_LEVEL_ID_OCCUR = 4;

LevelHistory::LevelHistory()
  : mMaxLevelIdOccurrence(MAX_LEVEL_ID_OCCUR)
  , mIsInitialized(false)
  , mCurrentLevelId(LevelId(""))
{
}

LevelId LevelHistory::detectLevelUsingHistory(const LevelId& detectedLevelId)
{
  //TODO change to boost optional!!!
  if (detectedLevelId.value == "")
    return mCurrentLevelId;

  if (!mIsInitialized)
  {
    mIsInitialized = true;
    mCurrentLevelId = detectedLevelId;
  }

  if (++mFloorCnt[detectedLevelId] > MAX_LEVEL_ID_OCCUR)
  {
    mFloorCnt.clear();
    mFloorCnt[detectedLevelId] = MAX_LEVEL_ID_OCCUR;
    mCurrentLevelId = detectedLevelId;
  }

  return mCurrentLevelId;
}

} } // namespace navigine::navigation_core
