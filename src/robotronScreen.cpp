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
#include "cinder/Matrix44.h"

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

//  mRotate = Quatf(cinder::Vec3f(-1.0f, 0.0f, 0.0f), M_PI / 4.0f);
  mPos = Vec3f(0.0, 1.0f, -0.950f);

  mRotate = Quatf(cinder::Vec3f(-1.0f, 0.0f, 0.0f), M_PI / 4.5f);
  mPos = Vec3f(0.0, 1.1f, -0.50f);
  mStartPos = Vec3f(0.0f, 1.50f, 2.25f);
  mStartScale = Vec3f(1.5f, 1.0f, 1.0f);
  
  mOffsetRange = -0.015;
  mOffsetBase = -0.04;
  
  initCubes();

  params->addSeparator("RobotronScreen");
  params->addParam("RobotronScreen:  Rotate", &mRotate);
  params->addParam("RobotronScreen:  Pos", &mPos);
  params->addParam("RobotronScreen:  StartPos", &mStartPos);
  params->addParam("RobotronScreen:  StartScale", &mStartScale);
  params->addParam("RobotronScreen:  OffsetBase", &mOffsetBase);
  params->addParam("RobotronScreen:  OffsetRange", &mOffsetRange);
  
  mStart = loadImage(ci::app::loadAsset("start_instructions.png"));
}

void RobotronScreen::draw()
{
  drawCubes();
//  drawSimple();
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

  float cellSize = 0.006f;
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

  // Init cabinet
  Vec3f top[4] = {
    { xStart, 0.0f, yStart }, { -xStart, 0.0f, yStart }, { -xStart, 0.0f, -yStart }, { xStart, 0.0f, -yStart },
  };
  Vec3f bottom[4];
  cinder::Matrix44f xform(mRotate);
  for (int i = 0; i < 4; i++)
  {
    top[i] = xform.transformPoint(top[i]);
    bottom[i] = top[i];
    bottom[i].z = -1.5;
  }
  mCabinet.appendVertices(top, 4);
  mCabinet.appendVertices(bottom, 4);
  for (int i = 0; i < 4; i++)
    mCabinet.appendColorRgb(Color::black());
  for (int i = 0; i < 4; i++)
  {
    float val = 0.25 + i * 0.15;
    mCabinet.appendColorRgb(Color(val,val,val));
  }
  // Top two triangles
  mCabinet.appendTriangle(0, 1, 2);
  mCabinet.appendTriangle(0, 2, 3);
  // Bottom two triangles
  mCabinet.appendTriangle(4, 5, 6);
  mCabinet.appendTriangle(4, 6, 7);
  // Front two
  mCabinet.appendTriangle(4, 5, 0);
  mCabinet.appendTriangle(5, 1, 0);
  // Back two
  mCabinet.appendTriangle(6, 7, 2);
  mCabinet.appendTriangle(3, 2, 7);
  // Left two
  mCabinet.appendTriangle(4, 0, 7);
  mCabinet.appendTriangle(0, 3, 7);
  // Right two
  mCabinet.appendTriangle(6, 1, 5);
  mCabinet.appendTriangle(6, 2, 1);
  
  mStartMesh = MeshHelper::createSquare();
}

void RobotronScreen::drawCubes()
{
  gl::enable(GL_TEXTURE);
  gl::color(Color::white());
  gl::pushMatrices();
  gl::translate(mPos);
  gl::color(Color::white());
 
  gl::pushMatrices();
  {
    gl::translate(mStartPos);
    Quatf rot(Vec3f(1,0,0), -M_PI / 2.0);
    gl::rotate(rot);
    gl::scale(mStartScale);
    mStart.enableAndBind();
    gl::draw(mStartMesh);
    mStart.unbind();
  }
  gl::popMatrices();
  
  gl::draw(mCabinet);
  gl::rotate(mRotate);

  mClient.bind();
  gl::TextureRef tex = mClient.getTexture();
  tex->setMinFilter(GL_NEAREST);
  tex->setMagFilter(GL_NEAREST);

  tex->enableAndBind();
  mCubeShader.bind();
  mCubeShader.uniform("tex0", 0);
  mCubeShader.uniform("offsetRange", mOffsetRange);
  mCubeShader.uniform("offsetBase", mOffsetBase);
  gl::draw(mVboMesh);
  mCubeShader.unbind();
  tex->unbind();
  tex->disable();
  mClient.unbind();
  gl::popMatrices();
}