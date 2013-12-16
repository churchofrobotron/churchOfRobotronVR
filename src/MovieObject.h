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
  MovieObject(const cinder::fs::path& path);
  
  virtual void render();
  virtual void update();
private:
  cinder::qtime::MovieGl	mMovie;
  cinder::gl::Texture mTexture;
};

typedef std::shared_ptr<MovieObject> MovieObjectPtr;

#endif /* defined(__churchOfRobotronVR__MovieObject__) */
