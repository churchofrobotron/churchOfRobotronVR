//
//  MovieObject.cpp
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 12/3/13.
//
//

#include "MovieObject.h"
#include "churchUtil.h"

using namespace ci;

MovieObject::MovieObject()
: mPosition(0.0f, 32.0f, 0.0f)
, mScale(20.0f, 20.0f)
, mRight(Vec3f::xAxis())
, mUp(Vec3f::zAxis())
, mCurrentMovie(-1)
{
}

void MovieObject::setMovieList(const std::vector<std::string>& movies)
{
  mMovies = movies;
}

void MovieObject::update()
{
  if (!mMovie || mMovie.isDone())
    loadCurrentMovie();

  mTexture = mMovie.getTexture();
  if (mTexture)
    mTexture.setFlipped(true);
}

void MovieObject::render()
{
  if (mTexture)
    cor::drawTexRectBillboard(&mTexture, mTexture.getWidth(), mTexture.getHeight(),
                              mPosition, mScale, 0.0f,
                              mRight, mUp);
}

void MovieObject::loadCurrentMovie()
{
  mTexture.reset();
  if (mMovies.size() == 0)
    return;
  mCurrentMovie++;
  if (mCurrentMovie > mMovies.size())
    mCurrentMovie = 0;
  mMovie = qtime::MovieGl(mMovies[mCurrentMovie]);
  mMovie.play();
}