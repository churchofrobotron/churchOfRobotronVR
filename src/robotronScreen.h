//
//  robotronScreen.h
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 11/26/13.
//
//

#ifndef __churchOfRobotronVR__robotronScreen__
#define __churchOfRobotronVR__robotronScreen__

#include "syphonClient.h"
#include "cinder/TriMesh.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vbo.h"
#include "cinder/params/Params.h"
#include "cinder/Quaternion.h"

class RobotronScreen
{
public:
  void init(cinder::params::InterfaceGl* params);
  void draw();
private:
  // Common
  syphonClient mClient;
  cinder::gl::Texture mStart;

  // Just draw texture
  void drawSimple();

  // Animated cubes
  cinder::gl::GlslProg mCubeShader;
  cinder::TriMesh mCubeMesh;
  cinder::gl::VboMeshRef mVboMesh;
  cinder::TriMesh mCabinet;
  
  cinder::Vec3f mStartPos;
  cinder::Vec3f mStartScale;
  cinder::TriMesh mStartMesh;
  
  cinder::Quatf mRotate;
  cinder::Vec3f mPos;
  
  float mOffsetRange;
  float mOffsetBase;

  void initCubes();
  void drawCubes();
};


#endif /* defined(__churchOfRobotronVR__robotronScreen__) */
