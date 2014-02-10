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
  gl::pushMatrices();
  Quatf rotY(Vec3f(0.0, 0.0, 1.0), M_PI / 6);
  gl::rotate(rotY);

  gl::translate(Vec3f(-2.0f, 0.0f, 0.0));
  
  if (mTexture)
    cor::drawTexRectBillboard(&mTexture, mTexture.getWidth(), mTexture.getHeight(),
                              Vec3f(0.0, 22.0, 0.0f), Vec2f(5.0f, 5.0f), 0.0f,
                              Vec3f::xAxis(), -Vec3f::zAxis());

  gl::pushMatrices();
  gl::translate(Vec3f(-2.5f, 18.0f, -6.0));
  gl::draw(mVbo);
  gl::popMatrices();
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
  std::vector<std::string> text;
  text.push_back(score.initials);
  text.push_back(score.score);
  mMesh = cor::textToMesh(text);
  mVbo = cinder::gl::VboMesh::create(mMesh);
  mTexture.reset();
  mMovie = qtime::MovieGl(basePath + score.gif);
  mMovie.setLoop();
  mMovie.play();
}