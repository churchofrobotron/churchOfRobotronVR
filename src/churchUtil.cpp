//
//  churchUtil.cpp
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 12/15/13.
//
//

#include "churchUtil.h"

#include "cinder/font.h"
#include "cinder/text.h"

#include "MeshHelper.h"

namespace cor {

using namespace cinder;

void drawTexRectBillboard( ci::gl::Texture* tex, GLfloat w, GLfloat h, const Vec3f &pos, const Vec2f &scale, float rotationDegrees, const Vec3f &bbRight, const Vec3f &bbUp )
{
  gl::SaveTextureBindState saveBind(tex->getTarget());
  gl::BoolState saveEnabledState( tex->getTarget() );
  
  tex->enableAndBind();
	glEnableClientState( GL_VERTEX_ARRAY );
	Vec3f verts[4];
	glVertexPointer( 3, GL_FLOAT, 0, &verts[0].x );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	const GLfloat texCoords[8] = {
    0, tex->isFlipped() ? 0 : h,
    0, tex->isFlipped() ? h : 0,
    w, tex->isFlipped() ? 0 : h,
    w, tex->isFlipped() ? h : 0 };
	glTexCoordPointer( 2, GL_FLOAT, 0, texCoords );
  
	float sinA = math<float>::sin( toRadians( rotationDegrees ) );
	float cosA = math<float>::cos( toRadians( rotationDegrees ) );
  
	verts[0] = pos + bbRight * ( -0.5f * scale.x * cosA - 0.5f * sinA * scale.y ) + bbUp * ( -0.5f * scale.x * sinA + 0.5f * cosA * scale.y );
	verts[1] = pos + bbRight * ( -0.5f * scale.x * cosA - -0.5f * sinA * scale.y ) + bbUp * ( -0.5f * scale.x * sinA + -0.5f * cosA * scale.y );
	verts[2] = pos + bbRight * ( 0.5f * scale.x * cosA - 0.5f * sinA * scale.y ) + bbUp * ( 0.5f * scale.x * sinA + 0.5f * cosA * scale.y );
	verts[3] = pos + bbRight * ( 0.5f * scale.x * cosA - -0.5f * sinA * scale.y ) + bbUp * ( 0.5f * scale.x * sinA + -0.5f * cosA * scale.y );
  
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
  
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}

// Move to churchUtil at some point.
cinder::TriMesh textToMesh(const std::vector<std::string>& str)
{
  const float xSize = 0.0666;
  const float ySize = xSize;
  const float cubeSize = 0.666; // of xSize
  
  TriMesh cube = MeshHelper::createCube();
  
  TextLayout tl;
  tl.setFont(Font("Data70EF", 32));
  tl.setColor(Color::white());
  for (auto s : str)
    tl.addCenteredLine(s);
  Surface s = tl.render(true, true);
  TriMesh mesh;
  for (int y = 0; y < s.getHeight(); y++)
  {
    for (int x = 0; x < s.getWidth(); x++)
    {
      uint8_t* red = s.getData(Vec2i(x, y));
      int value = *red > 0 ? 1 : 0;
      if (value)
      {
        size_t lastIndex = mesh.getNumIndices();
        Vec3f offset(x * xSize, 0.0, (s.getHeight() - y) * ySize);
        for (auto v : cube.getVertices())
          mesh.appendVertex((v * xSize * cubeSize) + offset);
        mesh.appendNormals(&cube.getNormals()[0], cube.getNormals().size());
        const auto& indices = cube.getIndices();
        for (int i = 0; i < indices.size(); i+=3)
        {
          mesh.appendTriangle(indices[i+0] + lastIndex,
                               indices[i+1] + lastIndex,
                               indices[i+2] + lastIndex);
        }
      }
    }
  }
  return mesh;
}
  
cinder::TriMesh spriteToMesh(const cinder::Surface8u& s)
{
  const float xSize = 0.5f;
  const float ySize = xSize;
  const float cubeSize = 0.666; // of xSize
  
  TriMesh cube = MeshHelper::createCube();
  
  TriMesh mesh;
  for (int y = 0; y < s.getHeight(); y++)
  {
    for (int x = 0; x < s.getWidth(); x++)
    {
      const uint8_t* pixel = s.getData(Vec2i(x, y));
      const uint8_t red = *pixel;
      const uint8_t green = *(pixel+1);
      const uint8_t blue = *(pixel+2);
      int value = (red+green+blue > 0) ? 1 : 0;
      if (value)
      {
        Color c(red, green, blue);
        size_t lastIndex = mesh.getNumIndices();
        Vec3f offset(x * xSize, 0.0, (s.getHeight() - y) * ySize);
        for (auto v : cube.getVertices())
        {
          mesh.appendVertex((v * xSize * cubeSize) + offset);
          mesh.appendColorRgb(c);
        }
        mesh.appendNormals(&cube.getNormals()[0], cube.getNormals().size());
        const auto& indices = cube.getIndices();
        for (int i = 0; i < indices.size(); i+=3)
        {
          mesh.appendTriangle(indices[i+0] + lastIndex,
                              indices[i+1] + lastIndex,
                              indices[i+2] + lastIndex);
        }
      }
    }
  }
  return mesh;
}
  
}