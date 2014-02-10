//
//  MovieCubes.cpp
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 2/9/14.
//
//

#include "MovieCubes.h"

#include "MeshHelper.h"
#include "LiveAssetManager.h"
#include "churchUtil.h"

using namespace cinder;

void MovieCubes::init(cinder::params::InterfaceGl* params)
{
  MovieObject::init(params);
  initCubes();
  
  mPos = Vec3f(-1.43, 10.0f, 2.98f);
  params->addSeparator("MovieCubes");
  params->addParam("MovieCubes: Position", &mPos);
}

void MovieCubes::render()
{
  if (!mTexture)
    return;

  gl::enable(GL_TEXTURE);
  gl::color(Color::white());
  gl::pushMatrices();
  gl::translate(mPos);
  gl::rotate(mRotate);
  
  mTexture.enableAndBind();
  mTexture.setMinFilter(GL_NEAREST);
  mTexture.setMagFilter(GL_NEAREST);
  
  mTexture.enableAndBind();
  mCubeShader.bind();
  mCubeShader.uniform("tex0", 0);
  gl::draw(mVboMesh);
  mCubeShader.unbind();
  mTexture.unbind();
  mTexture.disable();
  gl::popMatrices();
}

void MovieCubes::initCubes()
{
  TriMesh cube = MeshHelper::createCube();

  const int skipAmt = 1;
  float boardWidth = 426;
  float boardHeight = 472;
  float cellSize = 0.025f * skipAmt;
  float cubeSize = cellSize * 0.76f;
  float yStart = -(boardHeight / 2.0f) * cellSize;
  float xStart = -(boardWidth / 2.0f) * cellSize;
  float currY = yStart;
  for (int y = boardHeight * 0.2; y < boardHeight; y+=skipAmt)
  {
    float currX = xStart;
    for (int x = 0; x < boardWidth; x+=skipAmt)
    {
      Vec3f meshOffset(currX, 0.0f, currY);
      Vec2f tcOffset(x, y);
      cor::appendMeshWithTexCoords(mCubeMesh, cube, cubeSize, meshOffset, 0.0, tcOffset);
      currX += cellSize;
    }
    currY += cellSize;
  }
  
  mVboMesh = ci::gl::VboMesh::create(mCubeMesh);
  
  LiveAssetManager::load("movieCubeVert.glsl", "cubeFrag.glsl",
                         [this](ci::DataSourceRef vert,ci::DataSourceRef frag)
                         {
                           try
                           {
                             mCubeShader = gl::GlslProg(vert, frag);
                           }
                           catch (std::exception& e)
                           {
                             app::console() << e.what() << std::endl;
                           }
                         });
}