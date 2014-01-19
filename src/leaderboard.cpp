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

namespace  {
  const std::string basePath = "/Users/bzztbomb/projects/churchOfRobotron/scores/end/";
}

void Leaderboard::init()
{
  loadScores();
  loadNextScore();
  mTimer.start();
}

void Leaderboard::update()
{
  if (mTimer.getSeconds() > 5)
  {
    mTimer.stop();
    mTimer.start();

    loadNextScore();
  }
  mTexture = mMovie.getTexture();
  if (mTexture)
    mTexture.setFlipped(true);
}

void Leaderboard::draw()
{
  if (mTexture)
    cor::drawTexRectBillboard(&mTexture, mTexture.getWidth(), mTexture.getHeight(),
                              Vec3f(0.0, 22.0, 0.0f), Vec2f(5.0f, 5.0f), 0.0f,
                              Vec3f::xAxis(), -Vec3f::zAxis());

  gl::pushMatrices();
  gl::translate(Vec3f(-6.0f, 18.0f, -3.0));
  gl::draw(mMesh);
  gl::popMatrices();
}

void Leaderboard::loadScores()
{
  std::ifstream scores(basePath + "leaderboard.txt");
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
  mScoreIndex = 0; // Maybe we shouldn't do this?
}


void Leaderboard::loadNextScore()
{
  mScoreIndex++;
  if (mScoreIndex >= mScores.size())
    mScoreIndex = 0;
  
  const Score& score = mScores[mScoreIndex];
  mMesh = cor::textToMesh(score.initials + " " + score.score);
  mTexture.reset();
  mMovie = qtime::MovieGl(basePath + score.gif);
  mMovie.setLoop();
  mMovie.play();
}