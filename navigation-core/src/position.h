/** position.h
 *
 * Author: Aleksandr Galov <asgalov@gmail.com>
 * Copyright (c) 2018 Navigine. All rights reserved.
 *
 */

#ifndef NAVIGINE_POSITION_H
#define NAVIGINE_POSITION_H

#include <string>
#include "point.h"
#include "navigation_output.h"
#include "level.h"

namespace navigine {
namespace navigation_core {

struct Position : XYPoint
{
  Position()
    : XYPoint(0.0, 0.0)
    , levelId(LevelId(""))
    , deviationM(0)
    , ts(0)
    , isEmpty(true)
    , provider(Provider::NONE)
    , heading(0.0)
  { }
  
  Position(double _x, double _y, LevelId _levelId,
           double _deviationM, long long _ts, bool _isEmpty, 
           const Provider& _provider,
           double _heading)
    : XYPoint(_x, _y)
    , levelId(_levelId)
    , deviationM(_deviationM)
    , ts(_ts)
    , isEmpty(_isEmpty)
    , provider(_provider)
    , heading(_heading)
  { }
  
  LevelId levelId;
  double deviationM;
  long long ts;
  bool isEmpty = true; //TODO use boost optional for empty?
  Provider provider = Provider::NONE;
  double heading;
};

} } // namespace navigine::navigation_core

#endif // NAVIGINE_POSITION_H
