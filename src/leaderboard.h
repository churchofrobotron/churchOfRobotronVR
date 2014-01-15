//
//  leaderboard.h
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 1/14/14.
//
//

#ifndef __churchOfRobotronVR__leaderboard__
#define __churchOfRobotronVR__leaderboard__

#include "cinder/gl/Texture.h"

class Leaderboard
{
public:
  void init();
  void draw();
private:
  cinder::gl::Texture mTexture;
};

#endif /* defined(__churchOfRobotronVR__leaderboard__) */
