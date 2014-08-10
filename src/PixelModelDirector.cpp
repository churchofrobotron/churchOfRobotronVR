//
//  PixelModelDirector.cpp
//  churchOfRobotronVR
//
//  Created by Zach Archer on 8/9/14.
//
//

#include "PixelModelDirector.h"
#include "PixelModel.h"
#include "cinder/gl/gl.h"
#include "cinder/app/App.h"
#include "churchUtil.h"
#include "cinder/Rand.h"

// tEMP
#include "Enforcer.h"

using namespace ci;
using namespace ci::gl;
using namespace ci::app;

std::vector<cinder::Area> PixelModelDirector::walkAreas( cinder::Area home, int offsetX, BOOL dipHomeFrame )
{
	// Some animations look better if the neutral stance is lowered 1 pixel
	int dipY = (dipHomeFrame?-1:0);
	
	return {
			Area( home.x1, home.y1+dipY, home.x2, home.y2+dipY ),
			Area( home.x1 + offsetX, home.y1, home.x2 + offsetX, home.y2),
			Area( home.x1, home.y1+dipY, home.x2, home.y2+dipY ),
			Area( home.x1 + offsetX*2, home.y1, home.x2 + offsetX*2, home.y2)
	};
}

void PixelModelDirector::cacheAnimation( cinder::Surface8u allSprites, std::string key, std::vector<cinder::Area> areas )
{
	std::vector< cinder::gl::VboMeshRef > meshVec;
	
	for( auto area : areas ) {
		Surface8u s = allSprites.clone(area);
		meshVec.push_back( VboMesh::create(cor::spriteToMesh(s)));
	}
	
	mAnimations[key] = meshVec;
}

void PixelModelDirector::init( cinder::params::InterfaceGl* params )
{
	// Prepare animations
	Surface8u allSprites = loadImage(loadAsset("robotron_sprites_complete.png"));
	
	this->cacheAnimation( allSprites, "enforcer", { Area(1, 62, 1+9, 62+11) } );
	
	this->cacheAnimation( allSprites, "grunt", PixelModelDirector::walkAreas( Area(76, 118, 76+9, 118+13), (91-76), true ) );
	

						 
	for( int i=0; i<8; i++ ) {
		PixelModel* model = new PixelModel("model"+std::to_string(i));
		model->init(params);
		std::string key = (randBool() ? "enforcer" : "grunt");
		model->setAnimation( mAnimations[key] );
		model->setFPS( 5.0 );
		mModels.push_back(model);
	}
}

void PixelModelDirector::update()
{
	for( auto &model : mModels ) {
		model->update();
	}
}

void PixelModelDirector::draw()
{
	for( auto &model : mModels ) {
		model->draw();
	}
}
