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
  virtual void init(cinder::params::InterfaceGl* params);
  virtual void update();
  virtual void draw();
protected:
  cinder::Vec3f mPosition;
  bool mAnimate;

  void loadFrames(const std::vector<cinder::Area>& frames);
private:
  std::deque<cinder::TriMesh> mFrames;
  int mCurrFrame;
  cinder::Timer mTimer;
};

#endif /* defined(__churchOfRobotronVR__PixelModel__) */
