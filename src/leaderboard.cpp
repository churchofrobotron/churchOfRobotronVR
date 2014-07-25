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

extern bool hasOculus;

namespace  {
  const std::string basePath = "/Users/bzztbomb/projects/churchOfRobotron/mcor/leaderboard/data/";
}

void Leaderboard::init(cinder::params::InterfaceGl* params)
{
  mScoreIndex = 0; // Maybe we shouldn't do this?
  loadScores();
  loadNextScore();
  mTimer.start();
  mPosition = Vec3f(-5.03f, 7.01f, 0.53f);
  mScale = Vec3f(2.0f, 2.0f, 0.0f);
  mTextOffset = Vec3f(-1.78f, -0.250f, 1.21f);
  params->addSeparator("Leaderboard");
  params->addParam("Leaderboard: Position", &mPosition);
  params->addParam("Leaderboard: Scale", &mScale);
  params->addParam("Leaderboard: Text Offset", &mTextOffset);
}

void Leaderboard::update()
{
  if (mTimer.getSeconds() > 5)
  {
    mTimer.stop();
    mTimer.start();

    loadScores();
    loadNextScore();
  }
  if (mMovie && mMovie.isPlaying())
    mTexture = mMovie.getTexture();
  if (mTexture)
    mTexture.setFlipped(mFlipped);
}

void Leaderboard::draw()
{
  gl::pushMatrices();

  gl::translate(mPosition);

//  Quatf rotY(Vec3f(0.0, 0.0, 1.0), M_PI / 6);
//  gl::rotate(rotY);
  
  if (mTexture)
    cor::drawTexRectBillboard(&mTexture, mTexture.getWidth(), mTexture.getHeight(),
                              Vec3f(0.0, 0.0, 0.0f), mScale.xy(), 0.0f,
                              Vec3f::xAxis(), -Vec3f::zAxis());

  gl::pushMatrices();
  gl::translate(mTextOffset);
  if (mVbo)
    gl::draw(mVbo);
  gl::popMatrices();
  gl::popMatrices();
}

void Leaderboard::loadScores()
{
  std::ifstream scores(basePath + "leaderboard.txt");
  if (!scores.good())
  {
    std::cout << scores.rdstate() << std::endl;
    return;
  }
  mScores.clear();
  // Oh the slopoverflow!
  std::string line;
  while (!scores.eof())
  {
    std::getline(scores, line);
    if (line.empty())
      continue;
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
  std::sort(mScores.begin(), mScores.end(), [](const Score& a, const Score& b)
            {
              return std::stoi(a.score) > std::stoi(b.score);
            });
}


void Leaderboard::loadNextScore()
{
  if (mScores.size() == 0)
    return;
  mScoreIndex++;
  if (mScoreIndex >= mScores.size())
    mScoreIndex = 0;
  
  const Score& score = mScores[mScoreIndex];
  std::vector<std::string> text;
  text.push_back(score.initials + " " + score.score);
//  text.push_back(score.score);
  mMesh = cor::textToMesh(text);
  mVbo = cinder::gl::VboMesh::create(mMesh);
  mTexture.reset();
  fs::path p = basePath + score.gif;
  if (!fs::exists(p))
  {
    p = "/Users/bzztbomb/Downloads/Glitch-logo-02.mp4";
    mFlipped = hasOculus;
  } else {
    mFlipped = hasOculus;
  }
  if (fs::exists(p))
  {
    mMovie = qtime::MovieGl(p);
    mMovie.setLoop();
    mMovie.play();
  } else {
    if (mMovie)
      mMovie.stop();
    mTexture.reset();
  }
}