//
//  PixelModelDirector.h
//  churchOfRobotronVR
//
//  Created by Zach Archer on 8/9/14.
//
//

#ifndef __churchOfRobotronVR__PixelModelDirector__
#define __churchOfRobotronVR__PixelModelDirector__

#include "cinder/TriMesh.h"
#include "cinder/params/Params.h"
#include "cinder/gl/Vbo.h"
#include "PixelModel.h"

class PixelModelDirector
{

public:
	PixelModelDirector(){}
	
	void init( cinder::params::InterfaceGl* params );
	void update();
	void draw();

private:
	std::vector<PixelModel*> mModels;
};

#endif /* defined(__churchOfRobotronVR__PixelModelDirector__) */
