#pragma once

#include "util.h"

namespace GEOLIB_NAMESPACE::detail {

template <class T, template <typename> class U>
concept IsSameGeometryImpl = std::is_same_v<T, U<typename T::point_type>>;

template <class T, template <typename> class U>
using IsSameGeometry =
    std::conditional_t<IsSameGeometryImpl<T, U>, std::true_type, std::false_type>;

template <typename T>
constexpr bool isPoint()
{
    return false;
}

template <typename T>
constexpr bool isPoint() requires requires { util::getX(T{}) && util::getY(T{}); }
{
    return true;
}

template <class T>
concept IsPoint = isPoint<T>();

template <typename T>
constexpr bool isSegment() {
    return IsSameGeometry<T, Segment>::value;
}

template <typename T>
constexpr bool isBox() {
    return IsSameGeometry<T, Box>::value;
}

template <typename T>
constexpr bool isLineString() {
    return IsSameGeometry<T, LineString>::value;
}

template <typename T>
constexpr bool isRing() {
    return IsSameGeometry<T, Ring>::value;
}

template <typename T>
constexpr bool isPolygon() {
    return IsSameGeometry<T, Polygon>::value;
}

template <typename T>
constexpr bool isMultiPolygon() {
    return IsSameGeometry<T, MultiPolygon>::value;
}

template <typename T>
constexpr bool isGeometry() {
    return IsSameGeometry<T, Geometry>::value;
}

} // namespace GEOLIB_NAMESPACE::detail
