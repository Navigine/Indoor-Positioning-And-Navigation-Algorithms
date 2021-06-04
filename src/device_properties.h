#ifndef NAVIGINE_DEVICE_PROPERTIES_H
#define NAVIGINE_DEVICE_PROPERTIES_H

#include <string>

namespace navigine {
namespace navigation_core {

class DeviceProperties
{
  public:
    DeviceProperties() { }
    
    DeviceProperties(const std::string& _id, const std::string& _model,
                     bool _hasAcc  = false,
                     bool _hasGyro = false,
                     bool _hasMagn = false,
                     bool _hasBaro = false)
        : mId       ( _id      )
        , mModel    ( _model   )
        , mHasAccel ( _hasAcc  )
        , mHasGyro  ( _hasGyro )
        , mHasMagn  ( _hasMagn )
        , mHasBaro  ( _hasBaro )
    { }
    
  public:
    std::string  getId    ( void ) const { return mId;       }
    std::string  getModel ( void ) const { return mModel;    }
    bool         hasAccel ( void ) const { return mHasAccel; }
    bool         hasGyro  ( void ) const { return mHasGyro;  }
    bool         hasMagn  ( void ) const { return mHasMagn;  }
    bool         hasBaro  ( void ) const { return mHasBaro;  }
    
  private:
    std::string     mId       = "";     // Client ID
    std::string     mModel    = "";     // Telephone model
    bool            mHasAccel = false;
    bool            mHasGyro  = false;
    bool            mHasMagn  = false;
    bool            mHasBaro  = false;
};

} } // namespace navigine::navigation_core

#endif // NAVIGINE_DEVICE_PROPERTIES_H
