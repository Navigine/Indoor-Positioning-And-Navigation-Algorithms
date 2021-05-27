#pragma once

#include <string>
#include <vector>

namespace navigine {
namespace navigation_core {

struct Transmitter 
{
    enum class Type { WIFI, BEACON };

    std::string  id;
    double  x;
    double  y;
    Type  type;

    bool operator<  (const Transmitter& tx)const { return id < tx.id; }
};

typedef std::vector<Transmitter> Transmitters;

} } // namespace navigine::navigation_core
