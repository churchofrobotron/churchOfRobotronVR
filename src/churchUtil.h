//
//  churchUtil.h
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 12/15/13.
//
//

#ifndef __churchOfRobotronVR__churchUtil__
#define __churchOfRobotronVR__churchUtil__

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

namespace cor {
  
void drawTexRectBillboard( ci::gl::Texture* tex, GLfloat w, GLfloat h, const ci::Vec3f &pos, const ci::Vec2f &scale, float rotationDegrees, const ci::Vec3f &bbRight, const ci::Vec3f &bbUp );

cinder::TriMesh textToMesh(const std::vector<std::string>& str);
  
cinder::TriMesh spriteToMesh(const cinder::Surface8u& s);
  
void appendMesh(cinder::TriMesh& dest, const cinder::TriMesh& src, float scaleFactor, cinder::Vec3f offset, cinder::Color c);

void appendMeshWithTexCoords(cinder::TriMesh& dest, const cinder::TriMesh& src,
                             float meshScaleFactor, cinder::Vec3f meshOffset,
                             float tcScaleFactor, cinder::Vec2f tcOffset);
}

#endif /* defined(__churchOfRobotronVR__churchUtil__) */
