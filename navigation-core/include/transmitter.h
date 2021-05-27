#pragma once

#include <string>
#include <vector>
#include <cmath>
#include "declare_identifier.h"
#include "point.h"

namespace navigine {
namespace navigation_core {

struct PathlossModel
{
  double A;
  double B;
  double power;

  inline double operator()(double r) const {
    static constexpr double MIN_DISTANCE = 0.01;
    return A - B * std::log(std::max(r, MIN_DISTANCE));
  }
};

enum class TransmitterType { WIFI, BEACON, BLUETOOTH, UNKNOWN, EDDYSTONE, LOCATOR };

DECLARE_IDENTIFIER(TransmitterId)

template <typename Point>
struct Transmitter 
{
  Transmitter() = default;

  Transmitter(TransmitterId _id, Point _p, PathlossModel _model, TransmitterType _type)
    : id            ( _id    )
    , point         ( _p     )
    , pathlossModel ( _model )
    , type          ( _type  )
  { }

  TransmitterId id;
  Point point;
  PathlossModel pathlossModel;
  TransmitterType type;

  bool operator== (const Transmitter& tx)const { return id == tx.id; }
  bool operator<  (const Transmitter& tx)const { return id < tx.id; }
};

typedef std::vector<Transmitter<XYZPoint>> XYZTransmitters;
typedef std::vector<Transmitter<GeoPoint3D>> Geo3DTransmitters;

} } // namespace navigine::navigation_core
