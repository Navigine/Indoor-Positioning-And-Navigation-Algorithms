#pragma once

#include <optional>
#include <string>
#include <vector>

#include <extlibs/nlohmann/json.hpp>
#include <navigine/navigation-core/utils.h>

namespace navigine::navigation_core::serialization {

struct KeyValue
{
    std::string key;
    std::string value;
};

using KVStorage = std::vector<KeyValue>;

void addKeyValue(KVStorage& kv, const std::string& key, const std::string& value);

template <typename T, typename TD = std::decay_t<T>, typename = void>
struct is_optional : std::false_type {};

template <typename T, typename TD>
struct is_optional<
    T,
    TD,
    std::enable_if_t<std::is_same_v<TD,std::optional<typename TD::value_type>>>
    > : std::true_type {};

template <typename T>
inline constexpr bool is_optional_v = is_optional<T>::value;

template <typename T>
T fromString(const std::string& str)
{
    if constexpr (is_optional_v<T>) {
        return str.empty() ? std::nullopt : std::make_optional(fromString<typename T::value_type>(str));
    } else {
        T result;
        std::istringstream ss(str);
        ss >> result;
        if (!ss.fail() && ss.eof()) {
            return result;
        } else {
            throw std::runtime_error("error");
        }
    }
}

template <typename T>
std::string toString(T value, int precision)
{
    std::ostringstream ss;

    if constexpr (is_optional_v<T>) {
        ss << (value ? toString<typename T::value_type>(*value, precision) : "");
    } else {
        if (precision >= 0) {
            ss << std::fixed << std::setprecision(precision);
        }
        ss << value;
    }
    return ss.str();
}

template <>
inline bool fromString(const std::string& str)
{
    if (str == "0" || isEqualCaseInsensitive(str, "false")) {
        return false;
    } else if (str == "1" || isEqualCaseInsensitive(str, "true")) {
        return true;
    } else {
        throw std::runtime_error("error");
    }
}

template <>
inline std::string toString(bool value, int /*precision*/)
{
    return value ? "true" : "false";
}

template <typename T>
bool isNullOpt(T value)
{
    if constexpr (is_optional_v<T>) {
        return value ? false : true;
    }

    return false;
}

template <typename T>
bool tryFindSetting(const std::string& key, const T& settings) = delete;

template <typename T>
bool tryReadSetting(const std::string& key, const std::string& value, T& settings, std::vector<std::string>& errors) = delete;

template <typename T>
T readFromKV(const KVStorage& kv, std::vector<std::string>& errors) = delete;

template <typename T>
KVStorage writeToKV(const T& t1) = delete;

template <typename T>
T joinSettings(const T& t1, const T& t2) = delete;

template <typename T>
std::vector<std::string> listOfKeys(const T&) = delete;

#define NC_DECLARE_SETTINGS_SERIALIZER(SETTINGS_TYPE, VALUE_LIST) \
    template<> \
    inline bool tryFindSetting(const std::string& key, const SETTINGS_TYPE&) { \
        using Reader = std::function<void(SETTINGS_TYPE&, const std::string& str)>; \
        using Writer = std::function<std::string(const SETTINGS_TYPE&)>; \
        using NullOptCheck = std::function<bool(const SETTINGS_TYPE&)>; \
        using Joiner = std::function<void(const SETTINGS_TYPE&, const SETTINGS_TYPE&, SETTINGS_TYPE&)>; \
        using ValueType = std::tuple<Reader, Writer, NullOptCheck, Joiner>; \
        static const auto convert = \
            [](const auto& serializers) { \
                std::map<std::string, std::pair<ValueType, std::string>> result; \
                for(const auto& [keys, serializer] : serializers) { \
                    NC_ASSERT(!keys.empty(), "empty keys are not allowed (" << typeid(SETTINGS_TYPE).name() << ")"); \
                    for(const auto& key : keys) { \
                        NC_ASSERT(result.find(key) == result.end(), "duplicating keys are not allowed: " << key << " " \
                            "(" << typeid(SETTINGS_TYPE).name() << ")"); \
                        result[key] = std::make_pair(serializer, keys.front()); \
                    } \
                } \
                return result; \
            }; \
        static const auto serializers = std::vector<std::pair<std::vector<std::string>,ValueType>>VALUE_LIST; \
        static const auto serializersMap = convert(serializers); \
        const auto lowerKey = toLowerCase(key); \
        return serializersMap.contains(lowerKey); \
    } \
    template<> \
    inline bool tryReadSetting(const std::string& key, const std::string& value, SETTINGS_TYPE& settings, std::vector<std::string>& errors) { \
        using Reader = std::function<void(SETTINGS_TYPE&, const std::string& str)>; \
        using Writer = std::function<std::string(const SETTINGS_TYPE&)>; \
        using NullOptCheck = std::function<bool(const SETTINGS_TYPE&)>; \
        using Joiner = std::function<void(const SETTINGS_TYPE&, const SETTINGS_TYPE&, SETTINGS_TYPE&)>; \
        using ValueType = std::tuple<Reader, Writer, NullOptCheck, Joiner>; \
        static const auto convert = \
            [](const auto& serializers) { \
                std::map<std::string, std::pair<ValueType, std::string>> result; \
                for(const auto& [keys, serializer] : serializers) { \
                    NC_ASSERT(!keys.empty(), "empty keys are not allowed (" << typeid(SETTINGS_TYPE).name() << ")"); \
                    for(const auto& key : keys) { \
                        NC_ASSERT(result.find(key) == result.end(), "duplicating keys are not allowed: " << key << " " \
                            "(" << typeid(SETTINGS_TYPE).name() << ")"); \
                        result[key] = std::make_pair(serializer, keys.front()); \
                    } \
                } \
                return result; \
            }; \
        static const auto serializers = std::vector<std::pair<std::vector<std::string>,ValueType>>VALUE_LIST; \
        static const auto serializersMap = convert(serializers); \
        const auto lowerKey = toLowerCase(key); \
        auto iter = serializersMap.find(lowerKey); \
        if (iter == serializersMap.end()) { \
            return false; \
        } \
        const auto& reader = std::get<0>(iter->second.first); \
        const auto& defaultKey = iter->second.second; \
        if (defaultKey != lowerKey) { \
            std::stringstream ss; \
            ss << "Warning: key '" << key << "' is deprecated, use key '" << defaultKey << "'"; \
            errors.push_back(ss.str()); \
        } \
        try { \
            reader(settings, value); \
        } catch (std::runtime_error& e) { \
            std::stringstream ss; \
            ss << "Error: invalid value for key '" << key << "': '" << value << "'"; \
            errors.push_back(ss.str()); \
            return false; \
        } \
        return true; \
    } \
    template<> \
    inline SETTINGS_TYPE readFromKV(const KVStorage& kvStorage, std::vector<std::string>& errors) { \
        using Reader = std::function<void(SETTINGS_TYPE&, const std::string& str)>; \
        using Writer = std::function<std::string(const SETTINGS_TYPE&)>; \
        using NullOptCheck = std::function<bool(const SETTINGS_TYPE&)>; \
        using Joiner = std::function<void(const SETTINGS_TYPE&, const SETTINGS_TYPE&, SETTINGS_TYPE&)>; \
        using ValueType = std::tuple<Reader, Writer, NullOptCheck, Joiner>; \
        static const auto convert = \
            [](const auto& serializers) { \
                std::map<std::string, std::pair<ValueType, std::string>> result; \
                for(const auto& [keys, serializer] : serializers) { \
                    NC_ASSERT(!keys.empty(), "empty keys are not allowed (" << typeid(SETTINGS_TYPE).name() << ")"); \
                    for(const auto& key : keys) { \
                        NC_ASSERT(result.find(key) == result.end(), "duplicating keys are not allowed: " << key << " " \
                            "(" << typeid(SETTINGS_TYPE).name() << ")"); \
                        result[key] = std::make_pair(serializer, keys.front()); \
                    } \
                } \
                return result; \
            }; \
        static const auto serializers = std::vector<std::pair<std::vector<std::string>,ValueType>>VALUE_LIST; \
        static const auto serializersMap = convert(serializers); \
        SETTINGS_TYPE settings; \
        for(const auto& kv : kvStorage) { \
            const auto& key = kv.key; \
            const auto& value = kv.value; \
            const auto lowerKey = toLowerCase(key); \
            auto iter = serializersMap.find(lowerKey); \
            if (iter == serializersMap.end()) { \
                std::stringstream ss; \
                ss << "Error: invalid key '" << key << "'"; \
                errors.push_back(ss.str()); \
            } else { \
                const auto& reader = std::get<0>(iter->second.first); \
                const auto& defaultKey = iter->second.second; \
                if (defaultKey != lowerKey) { \
                    std::stringstream ss; \
                    ss << "Warning: deprecated key '" << key << "', use key '" << defaultKey << "'"; \
                    errors.push_back(ss.str()); \
                } \
                try { \
                    reader(settings, value); \
                } catch (std::runtime_error& e) { \
                    std::stringstream ss; \
                    ss << "Error: invalid value for key '" << key << "': '" << value << "'"; \
                    errors.push_back(ss.str()); \
                } \
            } \
        } \
        return settings; \
    } \
    template<> \
    inline KVStorage writeToKV(const SETTINGS_TYPE& settings) { \
        using Reader = std::function<void(SETTINGS_TYPE&, const std::string& str)>; \
        using Writer = std::function<std::string(const SETTINGS_TYPE&)>; \
        using NullOptCheck = std::function<bool(const SETTINGS_TYPE&)>; \
        using Joiner = std::function<void(const SETTINGS_TYPE&, const SETTINGS_TYPE&, SETTINGS_TYPE&)>; \
        using ValueType = std::tuple<Reader, Writer, NullOptCheck, Joiner>; \
        static const auto serializers = std::vector<std::pair<std::vector<std::string>,ValueType>>VALUE_LIST; \
        KVStorage kv; \
        for(const auto& [keys, serializer] : serializers) { \
            NC_ASSERT(!keys.empty(), "empty keys are not allowed (" << typeid(SETTINGS_TYPE).name() << ")"); \
            const auto& nullOptCheck = std::get<2>(serializer); \
            if (nullOptCheck(settings)) { \
                continue; \
            } \
            const auto& writer = std::get<1>(serializer); \
            kv.emplace_back(KeyValue{.key = keys[0], .value = writer(settings)}); \
        } \
        return kv; \
    } \
    template<> \
    inline SETTINGS_TYPE joinSettings(const SETTINGS_TYPE& s1, const SETTINGS_TYPE& s2) { \
        using Reader = std::function<void(SETTINGS_TYPE&, const std::string& str)>; \
        using Writer = std::function<std::string(const SETTINGS_TYPE&)>; \
        using NullOptCheck = std::function<bool(const SETTINGS_TYPE&)>; \
        using Joiner = std::function<void(const SETTINGS_TYPE&, const SETTINGS_TYPE&, SETTINGS_TYPE&)>; \
        using ValueType = std::tuple<Reader, Writer, NullOptCheck, Joiner>; \
        static const auto serializers = std::vector<std::pair<std::vector<std::string>,ValueType>>VALUE_LIST; \
        SETTINGS_TYPE result; \
        for(const auto& [_, serializer] : serializers) { \
            const auto& joiner = std::get<3>(serializer); \
            joiner(s1, s2, result); \
        } \
        return result; \
    } \
    template<> \
    inline std::vector<std::string> listOfKeys(const SETTINGS_TYPE&) { \
        using Reader = std::function<void(SETTINGS_TYPE&, const std::string& str)>; \
        using Writer = std::function<std::string(const SETTINGS_TYPE&)>; \
        using NullOptCheck = std::function<bool(const SETTINGS_TYPE&)>; \
        using Joiner = std::function<void(const SETTINGS_TYPE&, const SETTINGS_TYPE&, SETTINGS_TYPE&)>; \
        using ValueType = std::tuple<Reader, Writer, NullOptCheck, Joiner>; \
        static const auto serializers = std::vector<std::pair<std::vector<std::string>,ValueType>>VALUE_LIST; \
        std::vector<std::string> result; \
        for(const auto& [keys, _] : serializers) { \
            NC_ASSERT(!keys.empty(), "empty keys are not allowed (" << typeid(SETTINGS_TYPE).name() << ")"); \
            result.push_back(keys[0]); \
        } \
        return result; \
    } \

#define NC_DECLARE_SETTING_WITH_CUSTOM_PRECISION(PARAM, NAMES, PRECISION) \
    {std::vector<std::string>NAMES, { \
        /* Reader */ \
        [](auto& s, const std::string& str) { \
            s.PARAM = fromString<decltype(s.PARAM)>(str); \
        }, \
        /* Writer */ \
        [](const auto& s) -> std::string { \
            return toString<decltype(s.PARAM)>(s.PARAM, PRECISION); \
        }, \
        /* NullOptCheck */ \
        [](const auto& s) -> bool { \
            return isNullOpt<decltype(s.PARAM)>(s.PARAM); \
        }, \
        /* Joiner */ \
        [](const auto& s1, const auto& s2, auto& s3) { \
            if (!isNullOpt<decltype(s1.PARAM)>(s1.PARAM)) { \
                s3.PARAM = s1.PARAM; \
            } else if (!isNullOpt<decltype(s2.PARAM)>(s2.PARAM)) { \
                s3.PARAM = s2.PARAM; \
            } \
        }}}

#define NC_DECLARE_SETTING_WITH_DEFAULT_PRECISION(PARAM, NAMES) \
    NC_DECLARE_SETTING_WITH_CUSTOM_PRECISION(PARAM, NAMES, -1)

#define NC_DECLARE_SETTING_SELECT_4TH(ARG1, ARG2, ARG3, ARG4, ...) ARG4

#define NC_DECLARE_SETTING_MACRO_CHOOSER(...) \
    NC_DECLARE_SETTING_SELECT_4TH(__VA_ARGS__, \
            NC_DECLARE_SETTING_WITH_CUSTOM_PRECISION, \
            NC_DECLARE_SETTING_WITH_DEFAULT_PRECISION, \
            DUMMY)

#define NC_DECLARE_SETTING(...) \
    NC_DECLARE_SETTING_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

// Parsing plain JSON data to KVStorage
KVStorage parseJsonToKV(const nlohmann::json& j, std::vector<std::string>& errors);
KVStorage parseJsonDataToKV(const std::string& data, std::vector<std::string>& errors);
KVStorage parseJsonFileToKV(const std::string& filename, std::vector<std::string>& errors);
std::string dumpKVToJsonData(const KVStorage& kv, int indent = -1, bool ordered = true);

template <typename T1, typename T2>
bool readFromKV(const KVStorage& kvStorage, T1& t1, T2& t2, std::vector<std::string>& errors) {
    bool ok = true;
    for(const auto& kv : kvStorage) {
        if (tryReadSetting<T1>(kv.key, kv.value, t1, errors) ||
            tryReadSetting<T2>(kv.key, kv.value, t2, errors)) {
            continue;
        }
        ok = false;
        if (tryFindSetting<T1>(kv.key, t1) ||
            tryFindSetting<T2>(kv.key, t2)) {
            continue;
        }
        std::stringstream ss;
        ss << "Error: invalid key '" << kv.key << "'";
        errors.push_back(ss.str());
    }
    return ok;
}

} // namespace navigine::navigation_core::serialization
