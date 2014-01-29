//
//  Enforcer.cpp
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 1/28/14.
//
//
#include "Enforcer.h"
#include "cinder/Rand.h"

using namespace ci;

Enforcer::Enforcer()
: PixelModel("Enforcer")
{
  
}

void Enforcer::init(cinder::params::InterfaceGl* params)
{
  PixelModel::init(params);
  mSpeed = 0.2f;
  params->addParam(mParamPrefix + ": Speed", &mSpeed);
  
  std::vector<Area> enforcer =
  {
    Area(4, 94, 4+9, 94+11)
  };
  loadFrames(enforcer);
  mPosition = Vec3f(0.0f, 14.0f, 2.0f);
  mTarget = mPosition;
}

void Enforcer::update()
{
  PixelModel::update();
  if (!mAnimate)
    return;

  Vec3f diff = mTarget - mPosition;
  if (diff.length() <= mSpeed * 2)
  {
    mTarget.x = randFloat(-10.0f, 10.0f) * 2.0;
    mTarget.y = randFloat(-14.0f, 14.0f) * 2.0;
    mTarget.z = randFloat(-1.0f, 2.0f);
  } else {
    diff.normalize();
    diff *= mSpeed;
    mPosition += diff;
  }
}
