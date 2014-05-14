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
#include "cinder/gl/GlslProg.h"
#include "cinder/params/Params.h"
#include "CubeMap.h"

struct Transform
{
  cinder::Quatf mRotate;
  cinder::Vec3f mPosition;
  cinder::Vec3f mScale;
  
  void addParams(const std::string& prefix, cinder::params::InterfaceGl* params);
  void apply();
};

struct SimpleModel
{
  Transform mTransform;
  cinder::gl::VboMesh mMesh;
  cinder::gl::Texture mTexture;
};

class Environment
{
public:
  void init(cinder::params::InterfaceGl* params);
  void update();
  void draw();
  void drawLast();
private:
  cinder::gl::Texture mRoadTexture;
  cinder::TriMesh mGroundMesh;
  cinder::Vec3f mGroundPos;
  
  cinder::TriMesh mSkyMesh;
  std::shared_ptr<CubeMap> mSkyMap;
  cinder::gl::GlslProg mSkyShader;
  
  bool mRenderStation = true;
  cinder::gl::VboMesh mStationMesh;
  cinder::gl::Texture mStationTexture;
  cinder::Quatf mStationRotate;
  cinder::Vec3f mStationPos;
  cinder::Vec3f mStationScale;

  std::vector<SimpleModel> mBuildings;
  SimpleModel mBuilding1;
  SimpleModel mBuilding2;
  SimpleModel mBuilding3;
  
  void initGroundMesh();
  void initSkyMesh();
  void initStation();
  
  void drawSky();
  
};

#endif /* defined(__churchOfRobotronVR__environment__) */
