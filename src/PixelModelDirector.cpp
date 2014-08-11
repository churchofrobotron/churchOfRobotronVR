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

const int MAX_PIXEL_MODELS = 50;

void PixelModelDirector::init( cinder::params::InterfaceGl* params )
{
	// Prepare the timer
	mTimer.start();
	
	// Prepare animations
	Surface8u allSprites = loadImage(loadAsset("robotron_sprites_complete.png"));
	
	this->cacheAnimation( allSprites, "enforcer", { Area(1, 62, 1+9, 62+11) } );
	
	this->cacheAnimation( allSprites, "grunt", PixelModelDirector::walkAreas( Area(76, 118, 76+9, 118+13), (91-76), true ) );
	

	for( int i=0; i<MAX_PIXEL_MODELS; i++ ) {
		PixelModel* model = new PixelModel("model"+std::to_string(i));
		model->init(params);
		mModels.push_back(model);
	}
				
	mPrevSeconds = 0;
	mModelIdx = 0;
	mSequenceTimeRemaining = 0;	// This will trigger a new sequence right away
}

#pragma mark - Animation building

std::vector<cinder::gl::VboMeshRef> PixelModelDirector::animationMeshesForKey( std::string key ) {
	return mAnimations[key];
}

void PixelModelDirector::cacheAnimation( cinder::Surface8u allSprites, std::string key, std::vector<cinder::Area> areas )
{
	std::vector<cinder::gl::VboMeshRef> meshVec;
	
	for( auto area : areas ) {
		Surface8u s = allSprites.clone(area);
		meshVec.push_back( VboMesh::create(cor::spriteToMesh(s)));
	}
	
	mAnimations[key] = meshVec;
}

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

#pragma mark - per frame

void PixelModelDirector::update()
{
	double seconds = mTimer.getSeconds();
	float elapsed = (float)(seconds - mPrevSeconds);
	mPrevSeconds = seconds;
	
	for( auto model : mModels ) {
		model->update( elapsed );
	}
	
	mSequenceTimeRemaining -= elapsed;
	if( mSequenceTimeRemaining <= 0.0f ) {
		this->startSequenceHerdOfGrunts();
	}
}

void PixelModelDirector::draw()
{
	for( auto model : mModels ) {
		model->draw();
	}
}

#pragma mark - Animated sequences

// 		move.loc = Vec3f( randFloat(-26,26), randFloat(-48,10), randFloat(-0.95,0.48) );

void PixelModelDirector::startSequenceHerdOfGrunts() {
	int herdCount = (MAX_PIXEL_MODELS-10) + randInt()%10;
	mSequenceTimeRemaining = 0.0f;
	
	for( int m=0; m<herdCount; m++ ) {
		PixelModel* model = mModels[mModelIdx];
		mModelIdx = (mModelIdx+1) % mModels.size();
		
		model->clearMovements();

		float totalWalkTime = randFloat(10.0,15.0);
		mSequenceTimeRemaining = MAX( mSequenceTimeRemaining, totalWalkTime );
		
		int steps = 3 + randInt()%10;
		for( int s=0; s<steps; s++ ) {
			ModelMovement move;// = new ModelMovement();
			move.loc = Vec3f( randFloat(-26,26), lerp(10,-48,s/(float)steps), -1.27f );
			move.animKey = "grunt";
			move.fps = randFloat(2,6);
			move.duration = (m==0) ? 0 : totalWalkTime/steps;
			move.alwaysFaceAltar = false;
			
			model->appendMovement( move );
		}
	}
	

}
