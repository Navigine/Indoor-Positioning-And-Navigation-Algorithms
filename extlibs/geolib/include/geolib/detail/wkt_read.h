#pragma once

#include "type_checks.h"
#include "util.h"

namespace GEOLIB_NAMESPACE::detail {

inline int skipWhitespaces(std::istream& is)
{
    int count = 0;
    while (is.peek() == ' ') {
        is.get();
        ++count;
    }
    return count;
}

inline double readDouble(std::istream& is)
{
    double result = 0.0;
    is >> result;
    if (is.fail()) {
        throw Exception("readDouble: parse error at " + std::to_string(is.tellg()));
    }
    return result;
}

inline void readSymbol(std::istream& is, char c)
{
    if (is.peek() != c) {
        throw Exception("readSymbol: parse error at " + std::to_string(is.tellg()));
    }
    is.get();
}

inline void readSymbols(std::istream& is, const std::string& symbols)
{
    for(auto c : symbols) {
        readSymbol(is, c);
    }
}

inline void checkEof(std::istream& is)
{
    skipWhitespaces(is);
    if (!is.eof()) {
        throw Exception("checkEof: End of string expected");
    }
}

template <typename Point>
Point readPoint(std::istream& is)
{
    static_assert(isPoint<Point>());

    skipWhitespaces(is);
    double x = readDouble(is);
    readSymbol(is, ' ');
    skipWhitespaces(is);
    double y = readDouble(is);

    Point result;
    util::getX(result) = x;
    util::getY(result) = y;
    return result;
}

template <typename Point>
Segment<Point> readSegment(std::istream& is)
{
    static_assert(isPoint<Point>());

    Segment<Point> result;

    skipWhitespaces(is);

    readSymbol(is, '(');

    result.first = readPoint<Point>(is);
    skipWhitespaces(is);
    readSymbol(is, ',');
    result.second = readPoint<Point>(is);

    skipWhitespaces(is);
    readSymbol(is, ')');
    return result;
}

template <typename Point>
Box<Point> readBox(std::istream& is)
{
    static_assert(isPoint<Point>());

    Box<Point> result;

    skipWhitespaces(is);

    readSymbol(is, '(');

    result.minCorner = readPoint<Point>(is);
    skipWhitespaces(is);
    readSymbol(is, ',');
    result.maxCorner = readPoint<Point>(is);

    skipWhitespaces(is);
    readSymbol(is, ')');
    return result;
}

template <typename Point>
LineString<Point> readLineString(std::istream& is)
{
    static_assert(isPoint<Point>());

    LineString<Point> result;

    skipWhitespaces(is);

    readSymbol(is, '(');
    result.points.push_back(readPoint<Point>(is));

    while (true) {
        const auto pos = is.tellg();
        try {
            skipWhitespaces(is);
            readSymbol(is, ',');
            result.points.push_back(readPoint<Point>(is));
        } catch (Exception& e) {
            is.seekg(pos);
            break;
        }
    }

    skipWhitespaces(is);
    readSymbol(is, ')');
    return result;
}

template <typename Point>
Ring<Point> readRing(std::istream& is)
{
    static_assert(isPoint<Point>());

    Ring<Point> result;

    skipWhitespaces(is);

    readSymbol(is, '(');
    result.points.push_back(readPoint<Point>(is));

    while (true) {
        const auto pos = is.tellg();
        try {
            skipWhitespaces(is);
            readSymbol(is, ',');
            result.points.push_back(readPoint<Point>(is));
        } catch (Exception& e) {
            is.seekg(pos);
            break;
        }
    }

    skipWhitespaces(is);
    readSymbol(is, ')');
    return result;
}

template <typename Point>
Polygon<Point> readPolygon(std::istream& is)
{
    static_assert(isPoint<Point>());

    Polygon<Point> result;

    skipWhitespaces(is);

    readSymbol(is, '(');
    result.outer = readRing<Point>(is);

    while (true) {
        const auto pos = is.tellg();
        try {
            skipWhitespaces(is);
            readSymbol(is, ',');
            result.inners.push_back(readRing<Point>(is));
        } catch (Exception& e) {
            is.seekg(pos);
            break;
        }
    }

    skipWhitespaces(is);
    readSymbol(is, ')');
    return result;
}

template <typename Point>
MultiPolygon<Point> readMultiPolygon(std::istream& is)
{
    static_assert(isPoint<Point>());

    MultiPolygon<Point> result;

    skipWhitespaces(is);
    readSymbol(is, '(');

    while (true) {
        const auto pos = is.tellg();
        try {
            if (!result.parts.empty()) {
                skipWhitespaces(is);
                readSymbol(is, ',');
            }
            result.parts.push_back(readPolygon<Point>(is));
        } catch (Exception& e) {
            is.seekg(pos);
            break;
        }
    }

    skipWhitespaces(is);
    readSymbol(is, ')');
    return result;
}

template <typename Point>
Point fromWktPoint(const std::string& wkt)
{
    static_assert(isPoint<Point>());

    std::istringstream is(wkt);
    skipWhitespaces(is);
    readSymbols(is, "POINT");
    skipWhitespaces(is);
    readSymbol(is, '(');
    auto result = readPoint<Point>(is);
    skipWhitespaces(is);
    readSymbol(is, ')');
    checkEof(is);
    return result;
}

template <typename Point>
Segment<Point> fromWktSegment(const std::string& wkt)
{
    static_assert(isPoint<Point>());

    std::istringstream is(wkt);
    skipWhitespaces(is);
    readSymbols(is, Segment<Point>::geometry_type);
    auto result = readSegment<Point>(is);
    checkEof(is);
    return result;
}

template <typename Point>
Box<Point> fromWktBox(const std::string& wkt)
{
    static_assert(isPoint<Point>());

    std::istringstream is(wkt);
    skipWhitespaces(is);
    readSymbols(is, Box<Point>::geometry_type);
    auto result = readBox<Point>(is);
    checkEof(is);
    return result;
}

template <typename Point>
LineString<Point> fromWktLineString(const std::string& wkt)
{
    static_assert(isPoint<Point>());

    std::istringstream is(wkt);
    skipWhitespaces(is);
    readSymbols(is, LineString<Point>::geometry_type);
    auto result = readLineString<Point>(is);
    checkEof(is);
    return result;
}

template <typename Point>
Ring<Point> fromWktRing(const std::string& wkt)
{
    static_assert(isPoint<Point>());

    std::istringstream is(wkt);
    skipWhitespaces(is);
    readSymbols(is, Ring<Point>::geometry_type);
    auto result = readRing<Point>(is);
    checkEof(is);
    return result;
}

template <typename Point>
Polygon<Point> fromWktPolygon(const std::string& wkt)
{
    static_assert(isPoint<Point>());

    std::istringstream is(wkt);
    skipWhitespaces(is);
    readSymbols(is, Polygon<Point>::geometry_type);
    auto result = readPolygon<Point>(is);
    checkEof(is);
    return result;
}

template <typename Point>
MultiPolygon<Point> fromWktMultiPolygon(const std::string& wkt)
{
    static_assert(isPoint<Point>());

    std::istringstream is(wkt);
    skipWhitespaces(is);
    readSymbols(is, MultiPolygon<Point>::geometry_type);
    auto result = readMultiPolygon<Point>(is);
    checkEof(is);
    return result;
}

template <typename Point>
Geometry<Point> fromWktGeometry(const std::string& wkt)
{
    static_assert(isPoint<Point>());

    Geometry<Point> result;

    if (wkt.starts_with("POINT")) {
        result.geometry = fromWktPoint<Point>(wkt);
    } else if (wkt.starts_with(Segment<Point>::geometry_type)) {
        result.geometry = fromWktSegment<Point>(wkt);
    } else if (wkt.starts_with(Box<Point>::geometry_type)) {
        result.geometry = fromWktBox<Point>(wkt);
    } else if (wkt.starts_with(LineString<Point>::geometry_type)) {
        result.geometry = fromWktLineString<Point>(wkt);
    } else if (wkt.starts_with(Ring<Point>::geometry_type)) {
        result.geometry = fromWktRing<Point>(wkt);
    } else if (wkt.starts_with(Polygon<Point>::geometry_type)) {
        result.geometry = fromWktPolygon<Point>(wkt);
    } else if (wkt.starts_with(MultiPolygon<Point>::geometry_type)) {
        result.geometry = fromWktMultiPolygon<Point>(wkt);
    } else {
        throw Exception("fromWkt: Unexpected geometry type.");
    }

    return result;
}

template <typename Geometry>
Geometry fromWkt(const std::string& wkt)
{
    static_assert(
        isPoint<Geometry>() ||
        isSegment<Geometry>() ||
        isBox<Geometry>() ||
        isLineString<Geometry>() ||
        isRing<Geometry>() ||
        isPolygon<Geometry>() ||
        isMultiPolygon<Geometry>() ||
        isGeometry<Geometry>());

    if constexpr (isPoint<Geometry>()) {
        return fromWktPoint<Geometry>(wkt);
    } else if constexpr (isSegment<Geometry>()) {
        return fromWktSegment<typename Geometry::point_type>(wkt);
    } else if constexpr (isBox<Geometry>()) {
        return fromWktBox<typename Geometry::point_type>(wkt);
    } else if constexpr (isLineString<Geometry>()) {
        return fromWktLineString<typename Geometry::point_type>(wkt);
    } else if constexpr (isRing<Geometry>()) {
        return fromWktRing<typename Geometry::point_type>(wkt);
    } else if constexpr (isPolygon<Geometry>()) {
        return fromWktPolygon<typename Geometry::point_type>(wkt);
    } else if constexpr (isMultiPolygon<Geometry>()) {
        return fromWktMultiPolygon<typename Geometry::point_type>(wkt);
    } else if constexpr (isGeometry<Geometry>()) {
        return fromWktGeometry<typename Geometry::point_type>(wkt);
    }
}

} // namespace GEOLIB_NAMESPACE::detail
