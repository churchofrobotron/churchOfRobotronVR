//
//  MovieObject.h
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 12/3/13.
//
//

#ifndef __churchOfRobotronVR__MovieObject__
#define __churchOfRobotronVR__MovieObject__

#include "cinder/qtime/QuickTime.h"
#include "cinder/gl/Texture.h"

class MovieObject
{
public:
  MovieObject();
  
  void setMovieList(const std::vector<std::string>& movies);
  void render();
  void update();
  
  void setPosition(const cinder::Vec3f& pos) { mPosition = pos; }
  void setScale(const cinder::Vec2f& scale) { mScale = scale; }
  void setRightUp(const cinder::Vec3f& r, const cinder::Vec3f& u) { mRight = r; mUp = u; }
  void setMute(bool mute) { mMute = mute; }
private:
  std::vector<std::string> mMovies;
  int mCurrentMovie;
  cinder::qtime::MovieGl	mMovie;
  cinder::gl::Texture mTexture;
  
  cinder::Vec3f mPosition;
  cinder::Vec2f mScale;
  cinder::Vec3f mRight;
  cinder::Vec3f mUp;
               
  bool mMute;
  
  void loadCurrentMovie();
};

typedef std::shared_ptr<MovieObject> MovieObjectPtr;

#endif /* defined(__churchOfRobotronVR__MovieObject__) */
