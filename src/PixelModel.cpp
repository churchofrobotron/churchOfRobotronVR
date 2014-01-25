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
  Surface8u allSprites = loadImage(loadAsset("robotron_sprites.png"));
  
  std::vector<Area> grunts =
  {
    Area(2, 18, 2+9, 18+13),
    Area(14, 18, 14+9, 18+12)
  };
  
  for (auto grunt : grunts)
  {
    Surface8u s = allSprites.clone(grunt);
    mFrames.push_back(cor::spriteToMesh(s));
  }
  
  mPosition = Vec3f(0.0f, 14.0f, -5.0f);
  params->addSeparator("PixelModel");
  params->addParam("Position", &mPosition);
  
  mCurrFrame = 0;
  mTimer.start();
}

void PixelModel::update()
{
  const float fps = 2.0f;
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
