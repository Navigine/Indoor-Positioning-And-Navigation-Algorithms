/** logger.h
*
* Copyright (c) 2019 Navigine.
*
*/

#ifndef NAVIGINE_LOGGER_H
#define NAVIGINE_LOGGER_H

#include <string>
#include <map>

#define LOG_MESSAGE(lvl, fmt, ...)                                \
    do {                                                          \
      navigine::navigation_core::_Logger.print(lvl, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
    } while (false)

namespace navigine {
namespace navigation_core {

class Logger
{
public:
  Logger() = default;

  // Function is used for specifying a log file.
  // If @file parameter is null, function sets a default log file.
  // Otherwise, function sets a separate log file for the corresponding module.
  void setLogFile(const std::string& logFile, const char* file = 0);

  // Function is used for specifying a debug level.
  // If @file parameter is null, function sets a default debug level.
  // Otherwise, function sets a separate debug level for the corresponding module.
  void setDebugLevel(int level, const char* file = 0);

  void printDebugLevels(FILE* fp = stdout);
  void printLogFiles(FILE* fp = stdout);
  void setOutStream(FILE* os);

  // Function prints a format message in printf-like way.
  // @level - message debug level. If @level is greater than the debug level
  //  specified earlier, the message will be ignored.
  // @file  - module name, from where Logger has been called.
  // @line  - line number where Logger has been called.
  // @fmt   - message format string
  void print(int level, const char* file, int line, const char* fmt, ...) const;

private:
  FILE * mOutStream = stdout;
  std::map<std::string, int>          mDebugLevels = {};
  std::map<std::string, std::string>  mLogFiles = {};
};

extern Logger _Logger;

extern int GetLocalTime (int unixTime,
                         int* year, int* month,  int* day,
                         int* hour, int* minute, int* second);

extern long long GetCurrentTimeMillis();

} } // namespace navigine::navigation_core

#endif