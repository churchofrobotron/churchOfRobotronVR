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

using namespace cinder;

void drawTexRectBillboard( ci::gl::TextureRef tex, GLfloat w, GLfloat h, const Vec3f &pos, const Vec2f &scale, float rotationDegrees, const Vec3f &bbRight, const Vec3f &bbUp )
{
  tex->enableAndBind();
  
	glEnableClientState( GL_VERTEX_ARRAY );
	Vec3f verts[4];
	glVertexPointer( 3, GL_FLOAT, 0, &verts[0].x );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	const GLfloat texCoords[8] = {
    0, h,
    0, 0,
    w, h,
    w, 0 };
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


void RobotronScreen::init()
{
  mClient.setup();
  mClient.set("Robotron2084", "mamerobotron64");
}

void RobotronScreen::draw()
{
  mClient.bind();
  
  gl::TextureRef tex = mClient.getTexture();
  drawTexRectBillboard(tex, tex->getWidth(), tex->getHeight(),
                       Vec3f(0.0, 2.0f, 0.0f),
                       Vec2f(1.0f, 1.0f), 0.0f,
                       Vec3f::xAxis(), Vec3f::zAxis());
  
  mClient.unbind();
}
