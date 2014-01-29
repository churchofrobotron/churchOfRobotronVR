//
//  Enforcer.h
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 1/28/14.
//
//

#ifndef __churchOfRobotronVR__Enforcer__
#define __churchOfRobotronVR__Enforcer__

#include "PixelModel.h"

class Enforcer : public PixelModel
{
public:
  Enforcer();

  virtual void init(cinder::params::InterfaceGl* params);
  virtual void update();
private:
  float mSpeed;
  cinder::Vec3f mTarget;
};

#endif /* defined(__churchOfRobotronVR__Enforcer__) */
