//
//  MovieObject.cpp
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 12/3/13.
//
//

#include "MovieObject.h"
#include "churchUtil.h"

using namespace ci;

MovieObject::MovieObject(const cinder::fs::path& path)
{
  mMovie = qtime::MovieGl(path);
  mMovie.setLoop();
  mMovie.play();
}

void MovieObject::update()
{
  mTexture = mMovie.getTexture();
}

void MovieObject::render()
{
  if (mTexture)
    drawTexRectBillboard(&mTexture, mTexture.getWidth(), mTexture.getHeight(),
                         Vec3f(0.0, 32.0, 0.0f), Vec2f(20.0f, 20.0f), 0.0f,
                         Vec3f::xAxis(), Vec3f::zAxis());
}

