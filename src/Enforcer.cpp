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
    Area(1, 62, 1+9, 62+11)
  };
  //loadFrames(enforcer);
	setFPS(4.0);
  mPosition = Vec3f(0.0f, 14.0f, 2.0f);
  mTarget = mPosition;
}

void Enforcer::update()
{
  //PixelModel::update(0);

  Vec3f diff = mTarget - mPosition;
  if (diff.length() <= mSpeed * 2)
  {
    mTarget.x = randFloat(-13.0f, 13.0f) * 2.0;
    mTarget.y = randFloat(-24.0f, 5.0f) * 2.0;
    mTarget.z = randFloat(-0.95f, 0.48f);
  } else {
    diff.normalize();
    diff *= mSpeed;
    mPosition += diff;
  }
}
