//
//  leaderboard.h
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 1/14/14.
//
//

#ifndef __churchOfRobotronVR__leaderboard__
#define __churchOfRobotronVR__leaderboard__

#include "cinder/TriMesh.h"
#include "cinder/Timer.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Vbo.h"
#include "cinder/params/Params.h"
#include "cinder/qtime/QuickTime.h"

class Leaderboard
{
public:
  void init(cinder::params::InterfaceGl* params);
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
  int mScoreIndex;
  cinder::gl::Texture mTexture;
  cinder::qtime::MovieGl mMovie;
  cinder::Timer mTimer;
  cinder::TriMesh mMesh;
  cinder::gl::VboMeshRef mVbo;
  cinder::Vec3f mPosition;
  cinder::Vec3f mScale;
  cinder::Vec3f mTextOffset;
  
  void loadScores();
  void loadNextScore();
};

#endif /* defined(__churchOfRobotronVR__leaderboard__) */
