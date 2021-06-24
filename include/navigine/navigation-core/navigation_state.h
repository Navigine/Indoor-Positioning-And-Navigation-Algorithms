/** navigation_state.h
 *
 * Copyright (c) 2017 Navigine.
 *
 */

#ifndef NAVIGINE_NAVIGATION_STATE_H
#define NAVIGINE_NAVIGATION_STATE_H

#include <vector>
#include "navigation_input.h"
#include <navigine/navigation-core/point.h>
#include "xy_particle.h"

namespace navigine {
namespace navigation_core {

class NavigationState
{
  public:
    NavigationState() { }
    
    NavigationState(const std::vector<XYParticle>& particles,
                    const std::vector<double>& weights,
                    const RadioMeasurementsData& radioMsr,
                    const std::vector<XYParticle>& likelihoodPoints,
                    const std::vector<double>& likelihoodWeights,
                    double heading = 0,
                    double stepLen = 0,
                    int stepCounter = 0)
      : mParticles            ( particles      )
      , mParticleWeights      ( weights        )
      , mMeasurements         ( radioMsr       )
      , mLikelihoodMapPoints  ( likelihoodPoints  )
      , mLikelihoodMapWeights ( likelihoodWeights )
      , mHeading              ( heading        )
      , mStepLen              ( stepLen        )
      , mStepCounter          ( stepCounter    )
    { }
    
    void setStepCounter      (double stepCounter)  { mStepCounter = stepCounter; }
    void setStepLen          (double stepLen)      { mStepLen = stepLen; }
    void setIndoorPosition   (const XYPoint& pos)  { mIndoorPosition = pos; }
    void setOutdoorPosition  (const XYPoint& pos)  { mOutdoorPosition = pos; }

    XYPoint                  getIndoorPosition         ( void ) const { return mIndoorPosition; }
    XYPoint                  getOutdoorPosition        ( void ) const { return mOutdoorPosition; }
    int                      getStepCounter            ( void ) const { return mStepCounter; }
    double                   getHeading                ( void ) const { return mHeading;   }
    double                   getStepLen                ( void ) const { return mStepLen;   }
    std::vector<XYParticle>  getParticles              ( void ) const { return mParticles; }
    std::vector<double>      getParticleWeights        ( void ) const { return mParticleWeights; }
    RadioMeasurementsData        getLevelRadioMeasurements ( void ) const { return mMeasurements;    }
    std::vector<XYParticle>  getLikelihoodMapPoints       ( void ) const { return mLikelihoodMapPoints; }
    std::vector<double>      getLikelihoodMapWeights      ( void ) const { return mLikelihoodMapWeights; }
    
  private:
    std::vector<XYParticle>  mParticles;
    std::vector<double>      mParticleWeights;
    RadioMeasurementsData        mMeasurements;
    std::vector<XYParticle>  mLikelihoodMapPoints;
    std::vector<double>      mLikelihoodMapWeights;
    double                   mHeading;
    double                   mStepLen;
    int                      mStepCounter;
    XYPoint                  mIndoorPosition;
    XYPoint                  mOutdoorPosition;
};

} } // namespace navigine::navigation_core

#endif // NAVIGINE_NAVIGATION_STATE_H
