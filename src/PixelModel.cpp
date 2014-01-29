//
//  PixelModel.cpp
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 1/22/14.
//
//

#include "PixelModel.h"
#include "cinder/gl/gl.h"
#include "cinder/app/App.h"
#include "churchUtil.h"

using namespace ci;
using namespace ci::gl;
using namespace ci::app;

void PixelModel::init(cinder::params::InterfaceGl* params)
{
  mPosition = Vec3f(0.0f, 14.0f, -5.0f);
  params->addSeparator("PixelModel");
  params->addParam("Position", &mPosition);
  params->addParam("Animate", &mAnimate);

  mCurrFrame = 0;
  mAnimate = true;
  mTimer.start();
}

void PixelModel::loadFrames(const std::vector<cinder::Area>& frames)
{
  Surface8u allSprites = loadImage(loadAsset("robotron_sprites.png"));

  for (auto f : frames)
  {
    Surface8u s = allSprites.clone(f);
    mFrames.push_back(cor::spriteToMesh(s));
  }
}


void PixelModel::update()
{
  const float fps = 4.0f;
  const float spf = 1.0 / fps;

  int totalFrames = mTimer.getSeconds() / spf;
  mCurrFrame = totalFrames % mFrames.size();
}

void PixelModel::draw()
{
  gl::disable(GL_TEXTURE);
  gl::disable(GL_TEXTURE_2D);
  gl::color(Color::white());
  gl::pushMatrices();
  gl::translate(mPosition);
  gl::draw(mFrames[mCurrFrame]);
  gl::popMatrices();
}
