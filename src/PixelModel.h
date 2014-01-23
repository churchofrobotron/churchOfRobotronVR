//
//  PixelModel.h
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 1/22/14.
//
//

#ifndef __churchOfRobotronVR__PixelModel__
#define __churchOfRobotronVR__PixelModel__

#include "cinder/TriMesh.h"
#include "cinder/params/Params.h"

class PixelModel
{
public:
  void init(cinder::params::InterfaceGl* params);
  void update();
  void draw();
private:
  std::deque<cinder::TriMesh> mFrames;
  int mCurrFrame;
  cinder::Timer mTimer;
  cinder::Vec3f mPosition;
};

#endif /* defined(__churchOfRobotronVR__PixelModel__) */
