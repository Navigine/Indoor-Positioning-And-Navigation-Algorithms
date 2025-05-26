#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <iomanip>
#include <limits>
#include <list>
#include <optional>
#include <map>
#include <memory>
#include <type_traits>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <utility>

//#define GEOLIB_DEBUG_ENABLED

#ifdef GEOLIB_DEBUG_ENABLED
# include <iostream>
#endif

#define GEOLIB_FORMAT(msg)                                  \
    [&]() {                                                 \
        std::stringstream ss;                               \
        ss << std::setprecision(16) << msg << std::endl;    \
        return ss.str();                                    \
    }()

namespace GEOLIB_NAMESPACE::detail {

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

} // namespace GEOLIB_NAMESPACE::detail
