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
  
  // Just draw texture
  void drawSimple();
  
  // Animated cubes
  cinder::gl::GlslProg mCubeShader;
  cinder::TriMesh mCubeMesh;
  cinder::gl::VboMeshRef mVboMesh;
  cinder::Quatf mRotate;
  
  void initCubes();
  void drawCubes();
};


#endif /* defined(__churchOfRobotronVR__robotronScreen__) */
