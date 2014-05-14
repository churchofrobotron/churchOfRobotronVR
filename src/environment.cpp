//
//  environment.cpp
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 1/19/14.
//
//

#include "environment.h"
#include "cinder/gl/gl.h"
#include "cinder/ObjLoader.h"
#include "cinder/ip/Flip.h"
#include "LiveAssetManager.h"
#include "MeshHelper.h"

using namespace std;
using namespace ci;
using namespace ci::gl;
using namespace ci::app;

void Environment::init(cinder::params::InterfaceGl* params)
{
  initGroundMesh();
  mGroundPos = Vec3f::zero();
  initSkyMesh();

  for (int i = 0; i < 3; i++)
  {
    mBuildings.push_back(SimpleModel());
  }

  initStation();

  mRoadTexture = loadImage(loadAsset("road/road.png"));
  mRoadTexture.setMinFilter(GL_LINEAR);
  mRoadTexture.setMagFilter(GL_LINEAR);
  
  params->addSeparator("Station");
  params->addParam("Station: Render", &mRenderStation);
  mStationRotate = Quatf(Vec3f(1, 0, 0), M_PI / 2.0f);
  mStationRotate *= Quatf(Vec3f(0, 0, 1), -M_PI / 2.0f);
  params->addParam("Station: Rotate", &mStationRotate);
  mStationPos = Vec3f(0.0f, 5.0f, -1.25f);
  params->addParam("Station: Position", &mStationPos);
  mStationScale = Vec3f(2.0f, 1.5f, 1.5f);
  params->addParam("Station: Scale", &mStationScale);
  params->addParam("Road: Position", &mGroundPos);
  
  mBuildings[0].mTransform.mRotate = Quatf(Vec3f(1,0,0), M_PI / 2.0f);
  mBuildings[0].mTransform.mRotate *= Quatf(Vec3f(0, 0, 1), M_PI / 2.0f);
  mBuildings[0].mTransform.mPosition = Vec3f(-23.38f, 12.21f, 3.11f);
  mBuildings[0].mTransform.mScale = Vec3f(0.01f, 0.01f, 0.01f);
  mBuildings[0].mTransform.addParams("Building0", params);
  
  mBuildings[1].mTransform.mRotate = Quatf(Vec3f(1,0,0), M_PI / 2.0f);
  mBuildings[1].mTransform.mRotate *= Quatf(Vec3f(0, 0, 1), M_PI / 2.0f);
  mBuildings[1].mTransform.mPosition = Vec3f(45.158f, 14.96f, 4.85f);
  mBuildings[1].mTransform.mScale = Vec3f(0.03f, 0.02f, 0.02f);
  mBuildings[1].mTransform.addParams("Building1", params);
  
  mBuildings[2].mTransform.mRotate = Quatf(Vec3f(1,0,0), M_PI / 2.0f);
  mBuildings[2].mTransform.mRotate *= Quatf(Vec3f(0, 0, 1), M_PI / 2.0f);
  mBuildings[2].mTransform.mPosition = Vec3f(72.158f, -35.47f, 4.85f);
  mBuildings[2].mTransform.mScale = Vec3f(0.02f, 0.01f, 0.02f);
  mBuildings[2].mTransform.addParams("Building2", params);
  
  vector<string> files = {
    "sky/grimmnight_ft.tga",
    "sky/grimmnight_rt.tga",
    "sky/grimmnight_up.tga",
    "sky/grimmnight_bk.tga",
    "sky/grimmnight_lf.tga",
    "sky/grimmnight_dn.tga"
  };
  
  vector<Surface8u> faces;
  for (auto f : files)
  {
    Surface8u s(loadImage(loadAsset(f)));
    faces.push_back(s);
  }
  
  mSkyMap = std::make_shared<CubeMap>(1024, 1024,
                                      faces[0],
                                      faces[1],
                                      faces[2],
                                      faces[3],
                                      faces[4],
                                      faces[5]);

  LiveAssetManager::load("sky/skyboxVert.glsl", "sky/skyboxFrag.glsl",
                         [this](ci::DataSourceRef vert,ci::DataSourceRef frag)
                         {
                           try
                           {
                             mSkyShader = gl::GlslProg(vert, frag);
                           }
                           catch (std::exception& e)
                           {
                             app::console() << e.what() << std::endl;
                           }
                         });

}

void Environment::update()
{
  
}

void Environment::draw()
{
  gl::disable(GL_TEXTURE);
  gl::disable(GL_TEXTURE_2D);
  gl::color(Color::white());

  
  gl::enableDepthRead();
  gl::enableDepthWrite();
  
  //
  if (mRenderStation)
  {
    mStationTexture.enableAndBind();
    gl::pushMatrices();
    {
      gl::translate(mStationPos);
      gl::rotate(mStationRotate);
      gl::scale(mStationScale);
      gl::draw(mStationMesh);
    }
    gl::popMatrices();
    mStationTexture.disable();

    for (int i = 0; i < mBuildings.size(); i++)
    {
      SimpleModel* m = &mBuildings[i];
      m->mTexture.enableAndBind();
      gl::pushMatrices();
      {
        m->mTransform.apply();
        gl::draw(m->mMesh);
      }
      m->mTexture.disable();
      gl::popMatrices();
    }
  }
}

void Environment::drawLast()
{
  gl::pushMatrices();
  {
    gl::translate(mGroundPos);
    mRoadTexture.enableAndBind();
    gl::draw(mGroundMesh);
    mRoadTexture.unbind();
  }
  gl::popMatrices();
  
  drawSky();
}

void Environment::initGroundMesh()
{
  mGroundMesh.clear();
  
  // Ground mesh
  
  std::function<void(const Vec3f&)> placeRoad = [&](const Vec3f& offset)
  {
    float minv = -5.0f;
    float maxv = 5.0f;
    float yVal = -2.0f;
    float yStretch = 4.0f;
    
    Vec3f road_verts[] = {
      Vec3f(minv, minv * yStretch, yVal), Vec3f(minv, maxv * yStretch, yVal),
      Vec3f(maxv, maxv * yStretch, yVal), Vec3f(maxv, minv * yStretch, yVal)
    };
    
    Vec2f tex_verts[] = {
      Vec2f(0.0f, 0.0f), Vec2f(0.0f, 1.0f),
      Vec2f(1.0f, 1.0f), Vec2f(1.0f, 0.0f)
    };
    
    for (int j = 0; j < 4; j++)
    {
      mGroundMesh.appendVertex(road_verts[j]+offset);
      mGroundMesh.appendTexCoord(tex_verts[j]);
    }
    
    // get the index of the vertex. not necessary with this example, but good practice
    int vIdx0 = mGroundMesh.getNumVertices() - 4;
    int vIdx1 = mGroundMesh.getNumVertices() - 3;
    int vIdx2 = mGroundMesh.getNumVertices() - 2;
    int vIdx3 = mGroundMesh.getNumVertices() - 1;
    
    // now create the triangles from the vertices
    mGroundMesh.appendTriangle( vIdx0, vIdx1, vIdx2 );
    mGroundMesh.appendTriangle( vIdx0, vIdx2, vIdx3 );
  };
  
  placeRoad(Vec3f(-16.0f, 0.0f, 0.0f));
  placeRoad(Vec3f(-16.0f, 40.0f, 0.0f));
  placeRoad(Vec3f(-16.0f, -40.0f, 0.0f));
  placeRoad(Vec3f(16.0f, 0.0f, 0.0f));
  placeRoad(Vec3f(16.0f, 40.0f, 0.0f));
  placeRoad(Vec3f(16.0f, -40.0f, 0.0f));
  placeRoad(Vec3f(-64.0f, 0.0f, 0.0f));
  placeRoad(Vec3f(-64.0f, 40.0f, 0.0f));
  placeRoad(Vec3f(-64.0f, -40.0f, 0.0f));
  placeRoad(Vec3f(64.0f, 0.0f, 0.0f));
  placeRoad(Vec3f(64.0f, 40.0f, 0.0f));
  placeRoad(Vec3f(64.0f, -40.0f, 0.0f));
}

void Environment::initSkyMesh()
{
  mSkyMesh = MeshHelper::createCube();
  for (auto& v : mSkyMesh.getVertices())
    v *= 2.0f;
}

void Environment::drawSky()
{
  gl::enable(GL_TEXTURE_CUBE_MAP_ARB);
  mSkyMap->bindMulti(0);
  mSkyShader.bind();
  mSkyShader.uniform("skyTexture", 0);
  gl::draw(mSkyMesh);
  mSkyMap->unbind();
  mSkyShader.unbind();
  gl::disable(GL_TEXTURE_CUBE_MAP_ARB);
}

void Environment::initStation()
{
  ObjLoader loader( ci::app::loadAsset("station/gasStation.obj") );
  cinder::TriMesh mesh;
	loader.load( &mesh );
	mStationMesh = gl::VboMesh( mesh );
  mStationTexture = gl::Texture(loadImage(ci::app::loadAsset("station/gas_station.tga")));
  
  {
    ObjLoader building_loader(ci::app::loadAsset("industrial/industrial_building4.obj"));
    cinder::TriMesh bmesh;
    building_loader.load(&bmesh);
    mBuildings[0].mMesh = gl::VboMesh(bmesh);
    mBuildings[0].mTexture = gl::Texture(loadImage(ci::app::loadAsset("industrial/industrial_building4.jpg")));
  }
  
  {
    ObjLoader building_loader(ci::app::loadAsset("abandoned/abandoned1.obj"));
    cinder::TriMesh bmesh;
    building_loader.load(&bmesh);
    mBuildings[1].mMesh = gl::VboMesh(bmesh);
    mBuildings[1].mTexture = gl::Texture(loadImage(ci::app::loadAsset("abandoned/abandoned1_texture.jpg")));
  }
  
  {
    ObjLoader building_loader(ci::app::loadAsset("building11/building11.obj"));
    cinder::TriMesh bmesh;
    building_loader.load(&bmesh);
    mBuildings[2].mMesh = gl::VboMesh(bmesh);
    mBuildings[2].mTexture = gl::Texture(loadImage(ci::app::loadAsset("building11/building11_texture.jpg")));
  }
}

void Transform::addParams(const std::string& prefix, cinder::params::InterfaceGl* params)
{
  params->addParam(prefix + ": Rotate", &mRotate);
  params->addParam(prefix + ": Position", &mPosition);
  params->addParam(prefix + ": Scale", &mScale);
}

void Transform::apply()
{
  gl::translate(mPosition);
  gl::rotate(mRotate);
  gl::scale(mScale);
}
