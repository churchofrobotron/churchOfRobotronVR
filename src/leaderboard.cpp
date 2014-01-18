//
//  leaderboard.cpp
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 1/14/14.
//
//

#include "leaderboard.h"
#include "cinder/gl/gl.h"

#include "churchUtil.h"

using namespace cinder;

void Leaderboard::init()
{
  mMesh = cor::textToMesh("B T R");
}

void Leaderboard::draw()
{
  gl::pushMatrices();
  gl::translate(Vec3f(-6.0f, 18.0f, -3.0));
  gl::draw(mMesh);
  gl::popMatrices();
}