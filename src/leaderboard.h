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
#include "cinder/TriMesh.h"
#include "cinder/Timer.h"

class Leaderboard
{
public:
  void init();
  void update();
  void draw();  
private:
  struct Score
  {
    std::string initials;
    std::string score;
    std::string gif;
  };
  std::deque<Score> mScores;
  int mCurrIndex;
  cinder::Timer mTimer;
  cinder::TriMesh mMesh;
  
  void loadScores();
};

#endif /* defined(__churchOfRobotronVR__leaderboard__) */
