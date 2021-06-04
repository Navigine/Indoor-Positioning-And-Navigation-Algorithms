/** navigation_error_codes.h
 *
 * Copyright (c) 2017 Navigine.
 *
 */

#ifndef NAVIGINE_NAVIGATION_ERROR_CODES_H
#define NAVIGINE_NAVIGATION_ERROR_CODES_H

namespace navigine {
namespace navigation_core {

static const int ERROR_NO_SOLUTION              = 4;
static const int MISSING_STRING_CONFIG          = 5;
static const int MISSING_LEVELS                 = 6;
static const int NO_PARAMETERS_IN_JSON_FILE     = 21;
static const int ERROR_OPEN_FILE                = 25;
static const int INVALID_LOC_ID_IN_JSON_FILE    = 26;
static const int INVALID_PARAMETER_IN_JSON_FILE = 27;

} } // namespace navigine::navigation_core

#endif // NAVIGINE_NAVIGATION_ERROR_CODES_H
