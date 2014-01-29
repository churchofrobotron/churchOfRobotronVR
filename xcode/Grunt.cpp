//
//  Grunt.cpp
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 1/25/14.
//
//

#include "Grunt.h"

using namespace ci;

void Grunt::init(cinder::params::InterfaceGl* params)
{
  PixelModel::init(params);
  mSpeed = 0.1f;
  params->addParam("Speed", &mSpeed);
  mState = msEast;

  std::vector<Area> grunts =
  {
    Area(2, 18, 2+9, 18+13),
    Area(14, 18, 14+9, 18+12)
  };
  loadFrames(grunts);
}

void Grunt::update()
{
  PixelModel::update();

  if (!mAnimate)
    return;

  std::map<MovementState, Vec3f> mapStateToPos =
  {
    { MovementState::msNorth, Vec3f(-10, 14, mPosition.z) },
    { MovementState::msEast, Vec3f(10, 14, mPosition.z) },
    { MovementState::msSouth, Vec3f(10, -14, mPosition.z) },
    { MovementState::msWest, Vec3f(-10, -14, mPosition.z) },
  };

  Vec3f targetPos = mapStateToPos[mState];
  Vec3f diff = targetPos - mPosition;
  if (diff.length() <= mSpeed*2)
  {
    mPosition = targetPos;
    mState++;
    if (mState > msWest)
      mState = msNorth;
  } else {
    diff.normalize();
    diff *= mSpeed;
    mPosition += diff;
  }
}
