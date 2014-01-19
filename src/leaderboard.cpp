//
//  leaderboard.cpp
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 1/14/14.
//
//

#include "leaderboard.h"
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include "cinder/gl/gl.h"
#include "churchUtil.h"

using namespace cinder;
using namespace cinder::app;

void Leaderboard::init()
{
  mMesh = cor::textToMesh("C O R");
  loadScores();
  mTimer.start();
}

void Leaderboard::update()
{
  if (mTimer.getSeconds() > 5)
  {
    mTimer.stop();
    mTimer.start();
    
    mCurrIndex++;
    if (mCurrIndex >= mScores.size())
      mCurrIndex = 0;
    
    const Score& score = mScores[mCurrIndex];
    mMesh = cor::textToMesh(score.initials + " " + score.score);
  }
}

void Leaderboard::draw()
{
  gl::pushMatrices();
  gl::translate(Vec3f(-6.0f, 18.0f, -3.0));
  gl::draw(mMesh);
  gl::popMatrices();
}

void Leaderboard::loadScores()
{
  std::ifstream scores("/Users/bzztbomb/projects/churchOfRobotron/scores/end/leaderboard.txt");
  // Oh the slopoverflow!
  std::string line;
  while (!scores.eof())
  {
    std::getline(scores, line);
    std::stringstream lineStream(line);
    Score score;
    int offset = 0;
    std::string cell;
    while(std::getline(lineStream,cell,','))
    {
      switch (offset)
      {
        case 0:
          score.initials = cell;
          offset++;
          break;
        case 1:
          score.score = cell;
          offset++;
          break;
        case 2:
          score.gif = cell;
          mScores.push_back(score);
          offset = 0;
          score = Score();
          break;
      }
    }
  }
  mCurrIndex = 0; // Maybe we shouldn't do this?
}
