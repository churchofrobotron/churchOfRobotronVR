//
//  Grunt.h
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 1/25/14.
//
//

#ifndef __churchOfRobotronVR__Grunt__
#define __churchOfRobotronVR__Grunt__

#include "PixelModel.h"

class Grunt : public PixelModel
{
public:
  Grunt();
  virtual void init(cinder::params::InterfaceGl* params);
  virtual void update();
protected:
  enum MovementState
  {
    msNorth,
    msEast,
    msSouth,
    msWest,
  };
  MovementState mState;
  float mSpeed;
};

#endif /* defined(__churchOfRobotronVR__Grunt__) */
