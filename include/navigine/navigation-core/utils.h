#pragma once

#include <errno.h>

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <stdexcept>
#include <type_traits>
#include <tuple>
#include <variant>
#include <vector>

namespace navigine::navigation_core {

const double EPSILON = 1e-10;

using Timestamp = int64_t;

using LevelId = std::string;

std::string toLowerCase(const std::string& str);
std::string toUpperCase(const std::string& str);

bool isEqualCaseInsensitive(const std::string& str1, const std::string& str2);

double GetDeltaTimeSec(const Timestamp& t1, const Timestamp& t2);

#define NC_ASSERT_IMPL(cond)                                                        \
    do {                                                                            \
        if (!(cond)) {                                                              \
            std::stringstream ss;                                                   \
            ss << "**** **** **** **** **** **** **** **** **** **** **** ****\n";  \
            ss << "Assertion failed at " << __FILE__ << ":" << __LINE__ << ":\n";   \
            ss << ">>> " << #cond << "\n\n";                                        \
            std::cerr << ss.str();                                                  \
            ::std::abort();                                                         \
        }                                                                           \
    } while (false)

#define NC_ASSERT_MSG_IMPL(cond, message)                                           \
    do {                                                                            \
        if (!(cond)) {                                                              \
            std::stringstream ss;                                                   \
            ss << "**** **** **** **** **** **** **** **** **** **** **** ****\n";  \
            ss << "Fatal error at " << __FILE__ << ":" << __LINE__ << ":\n";        \
            ss << ">>> " << message << "\n\n";                                      \
            std::cerr << ss.str();                                                  \
            ::std::abort();                                                         \
        }                                                                           \
    } while (false)

#define NC_ASSERT_SELECT_3RD(x, y, z, ...) z
#define NC_ASSERT_IMPL_HELPER(...) \
    NC_ASSERT_SELECT_3RD(__VA_ARGS__, NC_ASSERT_MSG_IMPL, NC_ASSERT_IMPL)

#define NC_ASSERT(...) NC_ASSERT_IMPL_HELPER(__VA_ARGS__)(__VA_ARGS__)

#define NC_FORMAT(msg)                                      \
    [&]() {                                                 \
        std::stringstream ss;                               \
        ss << std::setprecision(16) << msg;                 \
        return ss.str();                                    \
    }()

} // namespace navigine::navigation_core
