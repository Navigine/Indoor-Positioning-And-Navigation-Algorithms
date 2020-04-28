/** position.h
 *
 * Author: Aleksandr Galov <asgalov@gmail.com>
 * Copyright (c) 2018 Navigine. All rights reserved.
 *
 */

#ifndef NAVIGINE_POSITION_H
#define NAVIGINE_POSITION_H

#include <string>
#include "navigation_output.h"

namespace navigine {
namespace navigation_core {

struct Position
{
  Position() { }
  
  Position(double _x, double _y, int _sublocId, 
           double _deviationM, long long _ts, bool _isEmpty, 
           const Provider& _provider, std::string _err_message = "")
    : x            ( _x   )
    , y            ( _y   )
    , sublocId     ( _sublocId   )
    , deviationM   ( _deviationM )
    , ts           ( _ts  )
    , isEmpty      ( _isEmpty )
    , provider     ( _provider )
    , errorMessage ( _err_message )
  { }
  
  Position(const std::string& _errorMessage)
    : Position()
  {
    x = -1.0;
    y = -1.0;
    errorMessage = _errorMessage;
  }
  
  double        x             = 0;
  double        y             = 0;
  int           sublocId      = -1;
  double        deviationM    = 0;
  long long     ts            = 0;
  bool          isEmpty       = true;
  Provider      provider      = Provider::NONE;
  std::string   errorMessage  = "";
};

} } // namespace navigine::navigation_core

#endif // NAVIGINE_POSITION_H
