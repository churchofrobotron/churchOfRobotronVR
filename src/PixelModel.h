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
#include "cinder/gl/Vbo.h"

class PixelModel
{
public:
  PixelModel(const std::string& prefix)
  {
    mParamPrefix = prefix;
  }
  
  virtual void init(cinder::params::InterfaceGl* params);
  virtual void update();
  virtual void draw();

protected:
	std::string mParamPrefix;
	cinder::Vec3f mPosition;
	cinder::Vec3f mScale;
	float mRotationRads;	// Apparent rotation on Y axis. (It's actually the model's Z axis)
	BOOL mAlwaysFaceAltar;	// perFrame: mRotation will be set to face the player.

	bool mAnimate;

  void loadFrames(const std::vector<cinder::Area>& frames);
private:
  std::deque<cinder::gl::VboMeshRef> mFrames;
  int mCurrFrame;
  cinder::Timer mTimer;
};

#endif /* defined(__churchOfRobotronVR__PixelModel__) */
