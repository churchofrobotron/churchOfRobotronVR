//
//  environment.cpp
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 1/19/14.
//
//

#include "environment.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::gl;

void Environment::init()
{
  initGroundMesh();
  initSkyMesh();
}

void Environment::update()
{
  
}

void Environment::draw()
{
  gl::disable(GL_TEXTURE);
  gl::disable(GL_TEXTURE_2D);
  gl::color(Color::white());
  gl::draw(mSkyMesh);
  gl::draw(mGroundMesh);
}

void Environment::initGroundMesh()
{
  mGroundMesh.clear();
  // Ground mesh
  float minv = -10000.0f;
  float maxv = 10000.0f;
  float yVal = -1.0;
  Color groundColor(0.2, 0.1, 0.2);
  
  mGroundMesh.appendVertex(Vec3f(minv, minv, yVal));
  mGroundMesh.appendColorRgb(groundColor);
  mGroundMesh.appendTexCoord(Vec2f(0.0f, 0.0f));
  
  mGroundMesh.appendVertex( Vec3f(minv, maxv, yVal) );
  mGroundMesh.appendColorRgb(groundColor);
  mGroundMesh.appendTexCoord(Vec2f(0.0f, 0.0f));
  
  mGroundMesh.appendVertex( Vec3f(maxv, maxv, yVal) );
  mGroundMesh.appendColorRgb(groundColor);
  mGroundMesh.appendTexCoord(Vec2f(0.0f, 0.0f));
  
  mGroundMesh.appendVertex( Vec3f(maxv, minv, yVal));
  mGroundMesh.appendColorRgb(groundColor);
  mGroundMesh.appendTexCoord(Vec2f(0.0f, 0.0f));
  
  // get the index of the vertex. not necessary with this example, but good practice
  int vIdx0 = mGroundMesh.getNumVertices() - 4;
  int vIdx1 = mGroundMesh.getNumVertices() - 3;
  int vIdx2 = mGroundMesh.getNumVertices() - 2;
  int vIdx3 = mGroundMesh.getNumVertices() - 1;
  
  // now create the triangles from the vertices
  mGroundMesh.appendTriangle( vIdx0, vIdx1, vIdx2 );
  mGroundMesh.appendTriangle( vIdx0, vIdx2, vIdx3 );
}

void Environment::initSkyMesh()
{
  mSkyMesh.clear();
  // Ground mesh
  float minv = -10000.0f;
  float maxv = 10000.0f;
  float yVal = 10.0;
  Color groundColor(0.5, 0.5, 0.7);
  groundColor *= 0.5f;
  
  mSkyMesh.appendVertex(Vec3f(minv, minv, yVal));
  mSkyMesh.appendColorRgb(groundColor);
  mSkyMesh.appendTexCoord(Vec2f(0.0f, 0.0f));
  
  mSkyMesh.appendVertex( Vec3f(minv, maxv, yVal) );
  mSkyMesh.appendColorRgb(groundColor);
  mSkyMesh.appendTexCoord(Vec2f(0.0f, 0.0f));
  
  mSkyMesh.appendVertex( Vec3f(maxv, maxv, yVal) );
  mSkyMesh.appendColorRgb(groundColor);
  mSkyMesh.appendTexCoord(Vec2f(0.0f, 0.0f));
  
  mSkyMesh.appendVertex( Vec3f(maxv, minv, yVal));
  mSkyMesh.appendColorRgb(groundColor);
  mSkyMesh.appendTexCoord(Vec2f(0.0f, 0.0f));
  
  // get the index of the vertex. not necessary with this example, but good practice
  int vIdx0 = mSkyMesh.getNumVertices() - 4;
  int vIdx1 = mSkyMesh.getNumVertices() - 3;
  int vIdx2 = mSkyMesh.getNumVertices() - 2;
  int vIdx3 = mSkyMesh.getNumVertices() - 1;
  
  // now create the triangles from the vertices
  mSkyMesh.appendTriangle( vIdx0, vIdx1, vIdx2 );
  mSkyMesh.appendTriangle( vIdx0, vIdx2, vIdx3 );
}