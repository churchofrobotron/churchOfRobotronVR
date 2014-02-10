//
//  robotronScreen.cpp
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 11/26/13.
//
//

#include "robotronScreen.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "churchUtil.h"

#include "MeshHelper.h"
#include "LiveAssetManager.h"

using namespace cinder;

namespace {
  const int boardWidth = 292;
  const int boardHeight = 240;
}

void RobotronScreen::init(cinder::params::InterfaceGl* params)
{
  mClient.setup();
  mClient.set("Robotron2084", "mamerobotron64");
  
  initCubes();

  mRotate = Quatf(cinder::Vec3f(-1.0f, 0.0f, 0.0f), M_PI / 4.0f);
  
  params->addSeparator("RobotronScreen");
  params->addParam("RobotronScreen:  Rotate", &mRotate);
}

void RobotronScreen::draw()
{
  drawCubes();
}

void RobotronScreen::drawSimple()
{
  mClient.bind();
  
  gl::TextureRef tex = mClient.getTexture();
  Vec3f up(Vec3f::zAxis());
  up.rotateX(M_PI / 1.5f);
  cor::drawTexRectBillboard(tex.get(), tex->getWidth(), tex->getHeight(),
                       Vec3f(0.0, 1.0f, -0.50f),
                       Vec2f(0.75f, 0.75f), 0.0f,
                       Vec3f::xAxis(), up);
  
  mClient.unbind();
}

void RobotronScreen::initCubes()
{
  TriMesh cube = MeshHelper::createCube();
  
  float cellSize = 0.0025f;
  float cubeSize = cellSize * 0.76f;
  float yStart = -(boardHeight / 2.0f) * cellSize;
  float xStart = -(boardWidth / 2.0f) * cellSize;
  float currY = yStart;
  for (int y = 0; y < boardHeight; y++)
  {
    float currX = xStart;
    for (int x = 0; x < boardWidth; x++)
    {
      Vec3f meshOffset(currX, 0.0f, currY);
      Vec2f tcOffset(x, y);
      cor::appendMeshWithTexCoords(mCubeMesh, cube, cubeSize, meshOffset, 0.0, tcOffset);
      currX += cellSize;
    }
    currY += cellSize;
  }
  
  mVboMesh = gl::VboMesh::create(mCubeMesh);
  
  LiveAssetManager::load("cubeVert.glsl", "cubeFrag.glsl",
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

void RobotronScreen::drawCubes()
{
  gl::enable(GL_TEXTURE);
  gl::color(Color::white());
  gl::pushMatrices();
  gl::translate(Vec3f(0.0, 1.0f, -0.50f));
  gl::rotate(mRotate);
  
  mClient.bind();
  gl::TextureRef tex = mClient.getTexture();
  tex->setMinFilter(GL_NEAREST);
  tex->setMagFilter(GL_NEAREST);
  
  tex->enableAndBind();
  mCubeShader.bind();
  mCubeShader.uniform("tex0", 0);
  gl::draw(mVboMesh);
  mCubeShader.unbind();
  tex->unbind();
  tex->disable();
  mClient.unbind();
  gl::popMatrices();
}