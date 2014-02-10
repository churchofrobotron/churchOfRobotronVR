//
//  MovieCubes.h
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 2/9/14.
//
//

#ifndef __churchOfRobotronVR__MovieCubes__
#define __churchOfRobotronVR__MovieCubes__

#include "MovieObject.h"
#include "cinder/params/Params.h"
#include "cinder/TriMesh.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vbo.h"
#include "cinder/Quaternion.h"

class MovieCubes : public MovieObject
{
public:
  virtual void init(cinder::params::InterfaceGl* params);
  virtual void render();
private:
  // Animated cubes
  cinder::gl::GlslProg mCubeShader;
  cinder::TriMesh mCubeMesh;
  cinder::gl::VboMeshRef mVboMesh;
  cinder::Quatf mRotate;
  cinder::Vec3f mPos;

  void initCubes();
};

#endif /* defined(__churchOfRobotronVR__MovieCubes__) */
