//
//  robotronScreen.cpp
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 11/26/13.
//
//

#include "robotronScreen.h"

void RobotronScreen::init()
{
  mClient.setup();
  mClient.set("Robotron2084", "mamerobotron64");
}

void RobotronScreen::draw()
{
  mClient.bind();
  mClient.draw(0,0);
  mClient.unbind();
}
