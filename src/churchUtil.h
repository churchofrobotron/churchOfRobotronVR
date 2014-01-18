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

cinder::TriMesh textToMesh(const std::string& str);
  
}

#endif /* defined(__churchOfRobotronVR__churchUtil__) */
