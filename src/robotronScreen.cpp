//
//  robotronScreen.cpp
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 11/26/13.
//
//

#include "robotronScreen.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "churchUtil.h"

using namespace cinder;

void RobotronScreen::init()
{
  mClient.setup();
  mClient.set("Robotron2084", "mamerobotron64");
}

void RobotronScreen::draw()
{
  mClient.bind();
  
  gl::TextureRef tex = mClient.getTexture();
  Vec3f up(Vec3f::zAxis());
  up.rotateX(M_PI / 1.5f);
  cor::drawTexRectBillboard(tex.get(), tex->getWidth(), tex->getHeight(),
                       Vec3f(0.0, 1.0f, -0.50f),
                       Vec2f(0.75f, 0.75f), 0.0f,
                       Vec3f::xAxis(), up);
  
  mClient.unbind();
}
