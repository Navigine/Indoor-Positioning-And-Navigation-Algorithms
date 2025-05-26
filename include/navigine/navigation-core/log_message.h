#pragma once

#include <optional>

#include <navigine/navigation-core/geometry.h>
#include <navigine/navigation-core/measurement.h>
#include <navigine/navigation-core/position.h>

namespace navigine::navigation_core {

using LogMessage = std::variant<Measurement, Position>;
using LogMessages = std::vector<LogMessage>;

static constexpr int LOG_MESSAGE_MEASUREMENT_INDEX = 0;
static constexpr int LOG_MESSAGE_CHECKPOINT_INDEX = 1;

// Parse log message from json data.
// Throws std::runtime_error on parse error.
LogMessage parseLogMessageFromData(const std::string& data);

// Parse log messages from jsonl-file.
// Parse errors will be appended to the errors-list.
// Invalid log messages will be skipped.
std::vector<LogMessage> parseLogMessagesFromFile(
    const std::string& filename,
    std::vector<std::string>& errors);

void normalizeLogMessages(std::vector<LogMessage>& messages);

Timestamp getTimestamp(const LogMessage& logMsg);

std::string dumpLogMessage(const LogMessage& logMsg);

void dumpLogToFile(
    const std::vector<LogMessage>& logMessages,
    const std::string& filename);

using LogReader = std::function<LogMessages()>;

LogReader createLogFileReader(const std::string& filename);

} // namespace navigine::navigation_core
