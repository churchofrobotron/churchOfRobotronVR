//
//  leaderboard.cpp
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 1/14/14.
//
//

#include "leaderboard.h"
#include "cinder/text.h"
#include "cinder/gl/gl.h"
#include "cinder/font.h"

#include "churchUtil.h"

using namespace cinder;

void Leaderboard::init()
{
  TextLayout tl;
  tl.setFont(Font("Data70EF", 128));
  tl.setColor(Color::white());
  tl.addLine("BTR");
  mTexture = tl.render(true, true);
}

void Leaderboard::draw()
{
  Vec3f up(Vec3f::zAxis());
  up.rotateX(M_PI / 1.5f);
  mTexture.enableAndBind();
  mTexture.setFlipped(false);
  gl::drawBillboard(
//  drawTexRectBillboard(&mTexture, mTexture.getWidth(), mTexture.getHeight(),
                       Vec3f(0.0, 1.0f, -0.50f),
                       Vec2f(0.75f, 0.75f), 0.0f,
                       Vec3f::xAxis(), up);
  mTexture.unbind();
}
