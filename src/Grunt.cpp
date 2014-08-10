//
//  Grunt.cpp
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 1/25/14.
//
//

#include "Grunt.h"

using namespace ci;

Grunt::Grunt()
: PixelModel("Grunt")
{
  
}

void Grunt::init(cinder::params::InterfaceGl* params)
{
  PixelModel::init(params);
  mSpeed = 0.1f;
  params->addParam(mParamPrefix + ": Speed", &mSpeed);
  mState = msEast;

	const int w = 9;
	const int h = 13;
	const int y = 118;
  std::vector<Area> grunts =
  {
    Area(76, y-1, 76+w, y-1+h),
    Area(91, y, 91+w, y+h),
	  Area(76, y-1, 76+w, y-1+h),
	  Area(106, y, 106+w, y+h),
  };
  //loadFrames(grunts);
	setFPS(6.0);
}

void Grunt::update()
{
  PixelModel::update();

  std::map<MovementState, Vec3f> mapStateToPos =
  {
    { MovementState::msNorth, Vec3f(-8, 2, mPosition.z) },
    { MovementState::msEast, Vec3f(8, 2, mPosition.z) },
    { MovementState::msSouth, Vec3f(8, -24, mPosition.z) },
    { MovementState::msWest, Vec3f(-8, -24, mPosition.z) },
  };

  Vec3f targetPos = mapStateToPos[mState];
  Vec3f diff = targetPos - mPosition;
  if (diff.length() <= mSpeed*2)
  {
    mPosition = targetPos;
    switch (mState)
    {
      case msNorth :
        mState = msEast;
        break;
      case msEast :
        mState = msSouth;
        break;
      case msSouth :
        mState = msWest;
        break;
      case msWest :
        mState = msNorth;
        break;
    };
    if (mState > msWest)
      mState = msNorth;
  } else {
    diff.normalize();
    diff *= mSpeed;
    mPosition += diff;
  }
}
