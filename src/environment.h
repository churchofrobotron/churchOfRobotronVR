//
//  environment.h
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 1/19/14.
//
//

#ifndef __churchOfRobotronVR__environment__
#define __churchOfRobotronVR__environment__

#include "cinder/TriMesh.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Vbo.h"
#include "cinder/params/Params.h"

class Environment
{
public:
  void init(cinder::params::InterfaceGl* params);
  void update();
  void draw();
private:
  cinder::TriMesh mGroundMesh;
  cinder::TriMesh mSkyMesh;
  
  bool mRenderStation = true;
  cinder::gl::VboMesh mStationMesh;
  cinder::gl::Texture mStationTexture;
  cinder::Quatf mStationRotate;
  cinder::Vec3f mStationPos;
  cinder::Vec3f mStationScale;
  
  void initGroundMesh();
  void initSkyMesh();
  void initStation();
};

#endif /* defined(__churchOfRobotronVR__environment__) */
