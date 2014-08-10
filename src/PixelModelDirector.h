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
	
	static std::vector<cinder::Area> walkAreas( cinder::Area home, int offsetX, BOOL dipHomeFrame );
	
	void init( cinder::params::InterfaceGl* params );
	void update();
	void draw();
	
	std::vector<cinder::gl::VboMeshRef> getMeshesWithAnimKey( std::string key );

private:
	std::vector< PixelModel* > mModels;
	std::map< std::string, std::vector<cinder::gl::VboMeshRef> > mAnimations;
	
	void cacheAnimation( cinder::Surface8u allSprites, std::string key, std::vector<cinder::Area> areas );
	
	cinder::Timer mTimer;
	double mPrevSeconds;
};

#endif /* defined(__churchOfRobotronVR__PixelModelDirector__) */
