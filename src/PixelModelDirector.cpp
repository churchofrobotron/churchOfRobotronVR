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

// "Rare" sequences are longer, and happen in front of the altar, where they can distract the player.
const int MIN_SEQS_BETWEEN_RARE_SEQUENCES = 5;

void PixelModelDirector::init( cinder::params::InterfaceGl* params )
{
	// Prepare the timer
	mTimer.start();
	
	// Prepare animations
	Surface8u allSprites = loadImage(loadAsset("robotron_sprites_complete.png"));
	
	//
	// ROBOTRONS
	//
	
	this->cacheAnimation( allSprites, "enforcer", { Area(1, 62, 1+9, 62+11) } );
	this->cacheAnimation( allSprites, "enforcer_bullet", PixelModelDirector::fourFrameAreas( Area(91,62,91+7,62+7), 13 ));
	
	this->cacheAnimation( allSprites, "grunt", PixelModelDirector::walkAreas( Area(76, 118, 76+9, 118+13), 15, true ) );
	
	this->cacheAnimation( allSprites, "tank", PixelModelDirector::fourFrameAreas( Area(1, 138, 1+13, 138+16), 19) );
	
	this->cacheAnimation( allSprites, "hulk_left", PixelModelDirector::walkAreas( Area(57, 179, 57+12, 179+14), 19, false ));
	this->cacheAnimation( allSprites, "hulk_right", PixelModelDirector::walkAreas( Area(77, 41, 77+12, 41+14), 19, false ));
	this->cacheAnimation( allSprites, "hulk_down", PixelModelDirector::walkAreas( Area(20, 41, 20+13, 41+16), 19, true ));
	this->cacheAnimation( allSprites, "hulk_up", PixelModelDirector::walkAreas( Area(20, 41, 20+13, 41+16), 19, true ));

	this->cacheAnimation( allSprites, "brain_left", PixelModelDirector::walkAreas( Area(161, 62, 161+11, 62+15), 19, false ));
	this->cacheAnimation( allSprites, "brain_right", PixelModelDirector::walkAreas( Area(217, 62, 217+11, 62+15), 19, false ));
	this->cacheAnimation( allSprites, "brain_down", PixelModelDirector::walkAreas( Area(1, 179, 1+11, 179+15), 19, true ));
	this->cacheAnimation( allSprites, "brain_up", PixelModelDirector::walkAreas( Area(39, 83, 39+11, 83+15), 19, true ));
	
	//
	// HUMANITY
	//
	this->cacheAnimation( allSprites, "mommy_left", PixelModelDirector::walkAreas( Area(103, 1, 103+6, 1+13), 13, false ));
	this->cacheAnimation( allSprites, "mommy_right", PixelModelDirector::walkAreas( Area(143, 1, 143+6, 1+13), 13, false ));
	this->cacheAnimation( allSprites, "mommy_down", PixelModelDirector::walkAreas( Area(181, 1, 181+7, 1+14), 13, true ));
	this->cacheAnimation( allSprites, "mommy_up", PixelModelDirector::walkAreas( Area(220, 1, 220+7, 1+14), 13, true ));
	
	this->cacheAnimation( allSprites, "daddy_left", PixelModelDirector::walkAreas( Area(259, 1, 259+8, 1+12), 15, false ));
	this->cacheAnimation( allSprites, "daddy_right", PixelModelDirector::walkAreas( Area(1, 20, 1+8, 20+12), 15, false ));
	this->cacheAnimation( allSprites, "daddy_down", PixelModelDirector::walkAreas( Area(46, 20, 46+8, 20+12), 15, true ));
	this->cacheAnimation( allSprites, "daddy_up", PixelModelDirector::walkAreas( Area(92, 20, 92+8, 20+12), 15, true ));

	this->cacheAnimation( allSprites, "mikey_left", PixelModelDirector::walkAreas( Area(136, 20, 136+5, 20+10), 11, false ));
	this->cacheAnimation( allSprites, "mikey_right", PixelModelDirector::walkAreas( Area(169, 20, 169+5, 20+10), 11, false ));
	this->cacheAnimation( allSprites, "mikey_down", PixelModelDirector::walkAreas( Area(202, 20, 202+5, 20+11), 11, true ));
	this->cacheAnimation( allSprites, "mikey_up", PixelModelDirector::walkAreas( Area(235, 20, 235+5, 20+11), 11, true ));

	for( int i=0; i<MAX_PIXEL_MODELS; i++ ) {
		PixelModel* model = new PixelModel("model"+std::to_string(i));
		model->init(params);
		mModels.push_back(model);
	}
				
	mPrevSeconds = 0;
	mModelIdx = 0;
	mSequenceTimeRemaining = 0;	// This will trigger a new sequence right away
	mSeqsSinceRareSeq = 0;
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

std::vector<cinder::Area> PixelModelDirector::walkAreas( cinder::Area home, int offsetX, BOOL dipHomeFrame ) {
	// Some animations look better if the neutral stance is lowered 1 pixel
	int dipY = (dipHomeFrame?-1:0);
	
	return {
		Area( home.x1, home.y1+dipY, home.x2, home.y2+dipY ),
		Area( home.x1 + offsetX, home.y1, home.x2 + offsetX, home.y2),
		Area( home.x1, home.y1+dipY, home.x2, home.y2+dipY ),
		Area( home.x1 + offsetX*2, home.y1, home.x2 + offsetX*2, home.y2)
	};
}

// Tanks have 4 frames of animation that run in a loop
std::vector<cinder::Area> PixelModelDirector::fourFrameAreas( cinder::Area home, int offsetX ) {
	return {
		Area( home.x1+offsetX*0, home.y1, home.x2+offsetX*0, home.y2 ),
		Area( home.x1+offsetX*1, home.y1, home.x2+offsetX*1, home.y2 ),
		Area( home.x1+offsetX*2, home.y1, home.x2+offsetX*2, home.y2 ),
		Area( home.x1+offsetX*3, home.y1, home.x2+offsetX*3, home.y2 )
	};
}

#pragma mark - per frame

void PixelModelDirector::update()
{
	double seconds = mTimer.getSeconds();
	float elapsed = (float)(seconds - mPrevSeconds);
	mPrevSeconds = seconds;
		
	mSequenceTimeRemaining -= elapsed;
	if( mSequenceTimeRemaining <= 0.0f ) {
		// Choose a new sequence. Yay
		
		// Test sequences. Don't use these in production :P
		//this->startSequence_HerdOfGrunts();
		//this->startSequence_TestAllAnims();
		
		BOOL useRareSeq = FALSE;	// Default: use a non-rare common sequence which occurs off to the side.
		if( (mSeqsSinceRareSeq > MIN_SEQS_BETWEEN_RARE_SEQUENCES) && (randFloat(1.0)<0.333) ) {
			useRareSeq = TRUE;
		}
		
		useRareSeq = FALSE;	// FIXME: remove this line duh
		
		if( useRareSeq ) {	// Rare sequences
			
			
		} else {	// Common sequences
			this->startSequence_EnforcersFlyOver();
		}
	}
	
	for( auto model : mModels ) {
		model->update( elapsed );
	}

}

void PixelModelDirector::draw()
{
	gl::disable(GL_TEXTURE);
	gl::disable(GL_TEXTURE_2D);
	gl::color(Color::white());

	for( auto model : mModels ) {
		model->draw();
	}
}

PixelModel* PixelModelDirector::getNextModel()
{
	PixelModel* model = mModels[mModelIdx];
	mModelIdx = (mModelIdx+1) % mModels.size();
	model->clearMovements();
	return model;
}

#pragma mark - Animated sequences

// 		move.loc = Vec3f( randFloat(-26,26), randFloat(-48,10), randFloat(-0.95,0.48) );

#pragma mark - Animations: Common + short (off-to-the-side)
// These are the most common animations, and are off to the side (or overhead)
//   so it's easy for the player to ignore them. (Roomba flies overhead; Grunt walks around; etc.)

void PixelModelDirector::startSequence_EnforcersFlyOver() {
	const float MAX_Z = 1.5f;
	
	// These look better in herds. Do 1-4 or so
	int count = (arc4random() % 4) + 1;
	for( int c=0; c<count; c++ ) {
		PixelModel* model = this->getNextModel();
				
		float angle = randFloat(M_PI*2.0);
		float radius = 20.0f;
		float duration = randFloat(3.0f,6.0f);
		int steps = 1 + randInt()%3;
		
		// Initial delay, to stagger their entrance
		float stepStart = mSequenceTimeRemaining;
		model->appendMovementInvisible( stepStart, Vec3f( cosf(angle)*(-radius), sinf(angle)*(-radius), randFloat(0,2.0f) ), angle+M_PI*0.5 );
		
		for( int s=1; s<=steps; s++ ) {
			float jitteryAngle = angle + (M_PI*0.3)*randFloat(-1.0,1.0);
			float distance = radius * ((((float)s/steps)*2.0) - 1.0);	// radius * range [-1..1]
			Vec3f loc = Vec3f( cosf(jitteryAngle)*distance, sinf(jitteryAngle)*distance, randFloat(0,MAX_Z));
			model->appendMovementVars("enforcer", 0, duration/steps, loc, jitteryAngle+M_PI*0.5f);
						
			stepStart += (duration/steps);
			
			// Fire a bullet when Enforcer reaches this point?
			if( randFloat() < 0.5 ) {
				PixelModel* bullet = this->getNextModel();
				bullet->appendMovementInvisible( stepStart, loc );
				float bulletDuration = randFloat( 1.5, 2.0 );
				float bulletAngle = randFloat( M_PI*2.0 );
				const float BULLET_DISTANCE = 40.0f;
				bullet->appendMovementVarsFacingAltar("enforcer_bullet", 8, bulletDuration, Vec3f(loc.x+cosf(bulletAngle)*BULLET_DISTANCE, loc.y+sinf(bulletAngle)*BULLET_DISTANCE, loc.z) );
			}

		}
		
		// Some overlap with the next animation
		mSequenceTimeRemaining += duration * randFloat( 0.1f, 0.4f );
	}
	
}

#pragma mark - Animations: Rare + longer
// Longer animations that occur in front of the altar, where players will see them more easily.
// These are longer and more elaborate: A Brain prog's a Human, etc.

#pragma mark - Animations: Cruddy tests (not for production)

// Cruddy test animation where Grunts enter ahead of the altar, and strafe all over while moving towards the exit.
void PixelModelDirector::startSequence_HerdOfGrunts() {
	std::cout << "Hello! Here come the grunts...\n";
	
	int herdCount = (MAX_PIXEL_MODELS-10) + randInt()%10;
	mSequenceTimeRemaining = 0.0f;
	
	for( int m=0; m<herdCount; m++ ) {
		PixelModel* model = this->getNextModel();
		
		float totalWalkTime = randFloat(10.0,15.0);
		mSequenceTimeRemaining = MAX( mSequenceTimeRemaining, totalWalkTime );
		
		int steps = 3 + randInt()%10;
		for( int s=0; s<steps; s++ ) {
			ModelMovement move;// = new ModelMovement();
			move.isVisible = (s>0);
			move.loc = Vec3f( randFloat(-26,26), lerp(10,-48,s/(float)steps), -1.27f );
			move.animKey = "grunt";
			move.fps = randFloat(2,6);
			move.duration = (s==0) ? 0 : totalWalkTime/steps;
			move.alwaysFaceAltar = false;
			
			model->appendMovement( move );
		}
	}
}

// Builds a claustrophobic ring around the altar with every animation represented.
// It is ridiculous but might help debug specific animation frames. Well that's what I'm telling myself anyway.
void PixelModelDirector::startSequence_TestAllAnims() {
	std::cout << "Hi! test all anims begins...\n";
	
	float time = 5.0;
	mSequenceTimeRemaining = time;
	
	float angle = 0.0;
	
	for( auto keyValue:mAnimations ) {
		//std::cout << keyValue.first << "\n";	// Yep, the iteration works
		
		PixelModel* model = this->getNextModel();
		
		float radius = randFloat(4.0,8.0);
		
		for( int s=0; s<=1; s++ ) {
			ModelMovement move;
			move.isVisible = true;
			move.animKey = keyValue.first;
			move.loc = Vec3f( sinf(angle)*radius, cosf(angle)*radius, -1.27f );
			//move.loc = Vec3f( randFloat(-26,26), randFloat(10,-48), -1.27f );
			move.fps = randFloat(2,6);
			move.duration = (s==0) ? 0 : time;
			move.alwaysFaceAltar = true;
			
			model->appendMovement( move );
		}
		
		angle += (M_PI*2.0) / mAnimations.size();
	}
}


