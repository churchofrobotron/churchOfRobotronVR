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
#include "LiveAssetManager.h"

using namespace ci;
using namespace ci::gl;
using namespace ci::app;

const int MAX_PIXEL_MODELS = 50;

// "Rare" sequences are longer, and happen in front of the altar, where they can distract the player.
const float MIN_TIME_BETWEEN_RARE_SEQUENCES = 25.0f;	// seconds

// Scene model layout
const float ROAD_X = 16.0f;
const float SIDE_X = 32.0f;	// looking right from altar: buildings start here
const float FRONT_Y = 7.0f;	// looking forward from altar: buildings start here
const float BACK_Y = -48.0f;	// looking backward from altar: buildings start here
const float ALLEY_LEFT_Y = -12.0f;
const float ALLEY_RIGHT_Y = -7.0f;

void PixelModelDirector::init( cinder::params::InterfaceGl* params )
{
	// Prepare the shader
	LiveAssetManager::load("pixelModelVert.glsl", "pixelModelFrag.glsl",
						   [this](ci::DataSourceRef vert,ci::DataSourceRef frag)
						   {
							   try
							   {
								   mPixelModelShader = gl::GlslProg(vert, frag);
							   }
							   catch (std::exception& e)
							   {
								   app::console() << e.what() << std::endl;
							   }
						   });
	
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
	this->cacheAnimation( allSprites, "mommy_prog_inv", { Area(112,178,112+9,178+15) });
	this->cacheAnimation( allSprites, "mommy_prog", { Area(121,178,121+9,178+15) });
	
	this->cacheAnimation( allSprites, "daddy_left", PixelModelDirector::walkAreas( Area(259, 1, 259+8, 1+12), 15, false ));
	this->cacheAnimation( allSprites, "daddy_right", PixelModelDirector::walkAreas( Area(1, 20, 1+8, 20+12), 15, false ));
	this->cacheAnimation( allSprites, "daddy_down", PixelModelDirector::walkAreas( Area(46, 20, 46+8, 20+12), 15, true ));
	this->cacheAnimation( allSprites, "daddy_up", PixelModelDirector::walkAreas( Area(92, 20, 92+8, 20+12), 15, true ));
	this->cacheAnimation( allSprites, "daddy_prog_inv", { Area(131,178,131+10,178+15) });
	this->cacheAnimation( allSprites, "daddy_prog", { Area(141,178,141+10,178+15) });

	this->cacheAnimation( allSprites, "mikey_left", PixelModelDirector::walkAreas( Area(136, 20, 136+5, 20+10), 11, false ));
	this->cacheAnimation( allSprites, "mikey_right", PixelModelDirector::walkAreas( Area(169, 20, 169+5, 20+10), 11, false ));
	this->cacheAnimation( allSprites, "mikey_down", PixelModelDirector::walkAreas( Area(202, 20, 202+5, 20+11), 11, true ));
	this->cacheAnimation( allSprites, "mikey_up", PixelModelDirector::walkAreas( Area(235, 20, 235+5, 20+11), 11, true ));
	this->cacheAnimation( allSprites, "mikey_prog_inv", { Area(151,178,151+9,178+15) });
	this->cacheAnimation( allSprites, "mikey_prog", { Area(160,178,160+9,178+15) });

	for( int i=0; i<MAX_PIXEL_MODELS; i++ ) {
		PixelModel* model = new PixelModel("model"+std::to_string(i));
		model->init(params);
		mModels.push_back(model);
	}
				
	mPrevSeconds = 0;
	mModelIdx = 0;
	mSequenceTimeRemaining = 0;	// This will trigger a new sequence right away
	mTimeSinceRareSeq = 0.0f;
	mRareSeqIndex = 0;
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
	
	mTimeSinceRareSeq += elapsed;
	
	// Advance color cycling
	PixelModel::updateTime(elapsed);
	
	mSequenceTimeRemaining -= elapsed;
	if( mSequenceTimeRemaining <= 0.0f ) {
		// Choose a new sequence. Yay
		
		// Test sequences. Don't use these in production :P
		//this->startSequence_HerdOfGrunts();
		//this->startSequence_TestAllAnims();
		
		BOOL useRareSeq = FALSE;	// Default: use a non-rare common sequence which occurs off to the side.
		if( (mTimeSinceRareSeq > MIN_TIME_BETWEEN_RARE_SEQUENCES) && (randFloat(1.0)<0.1) ) {
			mTimeSinceRareSeq = 0.0f;	// reset
			useRareSeq = TRUE;
		}
		
		if( useRareSeq ) {	// Rare sequences
			this->startSequence_brainProgsHuman();
			
		} else {	// Common sequences
			float rand = randFloat(1.0f);
			if( rand < 0.25f ) {
				this->startSequence_EnforcersFlyOver();
			} else {
				this->startSequence_Tank();
			}
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
	
	mPixelModelShader.bind();
	
	/*
	Vec4f cc0( 1.0f, 0.0f, 1.0f, 1.0f );
	mCubeShader.uniform("cycleColor0",cc0);
	Vec4f cc1( 1.0f, 1.0f, 0.0f, 1.0f );
	mCubeShader.uniform("cycleColor1",cc1);
	 */
	
	for( auto model : mModels ) {
		model->draw();
	}

	mPixelModelShader.unbind();
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
			if( randFloat() < 0.5f ) {
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

// Tank rolls down the road, or across (between the buildings)
void PixelModelDirector::startSequence_Tank() {
	const float TANK_Z = -1.3f;
	
	PixelModel* model = this->getNextModel();
	float duration = randFloat(10.0f, 15.0f);
	Vec3f startLoc, endLoc;
	float rotation;
	BOOL doFlip = (arc4random()%2)==1;
	
	int path = (arc4random() % 3);
	if( (path==0) || (path==1) ) {	// Left or Right of altar
		float atX = (ROAD_X + randFloat(-2.0,2.0)) * ((path==0) ? -1 : 1);
		startLoc = Vec3f( atX, FRONT_Y+10.0f, TANK_Z );
		endLoc = Vec3f( atX, BACK_Y-20.0f, TANK_Z );
		rotation = M_PI/2 * (doFlip ? 1 : -1);
		
	} else if( path==2 ) {	// Across the gas station, between the buildings
		float offsetY = randFloat(-2.0,2.0);
		startLoc = Vec3f( -SIDE_X-10.0f, ALLEY_LEFT_Y+offsetY, TANK_Z );
		endLoc = Vec3f( SIDE_X+10.0f, ALLEY_RIGHT_Y+offsetY, TANK_Z );
		rotation = (doFlip ? M_PI : 0);
	}
	
	// Maybe swap start & end locs, so tanks move in both directions
	if( doFlip ) {
		Vec3f tempLoc = startLoc;
		startLoc = endLoc;
		endLoc = tempLoc;
	}
	
	model->appendMovementInvisible( 0, startLoc, rotation );
	model->appendMovementVars( "tank", 6, duration, endLoc, rotation );

	// Some overlap with the next animation
	mSequenceTimeRemaining = duration * randFloat( 0.1f, 0.3f );
}

#pragma mark - Animations: Rare + longer
// Longer animations that occur in front of the altar, where players will see them more easily.
// These are longer and more elaborate: A Brain prog's a Human, etc.

void PixelModelDirector::startSequence_brainProgsHuman() {
	const float BRAIN_FPS = 6.0f;
	
	const float BRAIN_Z = -1.1f;
	const float HUMAN_Z = -1.4f;
	
	const float PROG_X_BRAIN = -1.2f - 1.5f;
	const float PROG_X_HUMAN = -1.2f + 1.5f;
	const float PROG_Y = 3.0f;
	const float BACKSTAGE_Y = FRONT_Y + 10.0f;
	
	PixelModel* brain = this->getNextModel();
	PixelModel* human = this->getNextModel();
	
	int humanType = (arc4random() % 3);
	std::string humanPrefix;
	switch( humanType ) {
		case 0:	humanPrefix = "mommy_"; break;
		case 1: humanPrefix = "daddy_"; break;
		case 2: humanPrefix = "mikey_"; break;
	}
	
	// Semi-random walks. Emerge from front left&right streets, and meander towards the altar.
	const float HUMAN_WALK_DURATION = 10.0f;	// The brain will walk faster than the human.
	human->appendMovementInvisible( 0, Vec3f(ROAD_X,BACKSTAGE_Y,HUMAN_Z) );
	human->appendMovementVars( humanPrefix+"down", 4, HUMAN_WALK_DURATION*0.334, Vec3f(ROAD_X,FRONT_Y,HUMAN_Z), 0.0f );
	human->appendMovementVars( humanPrefix+"left", 4, HUMAN_WALK_DURATION*0.333, Vec3f(PROG_X_HUMAN,FRONT_Y,HUMAN_Z), 0.0f );
	human->appendMovementVars( humanPrefix+"down", 4, HUMAN_WALK_DURATION*0.333, Vec3f(PROG_X_HUMAN,PROG_Y,HUMAN_Z), 0.0f );
	
	// Brain should wait before appearing, then walk faster. Walk an 'L' path, so emerge rapidly from the side.
	const float BRAIN_WALK_DURATION = HUMAN_WALK_DURATION * 0.8f;
	const float BRAIN_WALK_DELAY = HUMAN_WALK_DURATION - BRAIN_WALK_DURATION;
	brain->appendMovementInvisible( BRAIN_WALK_DELAY, Vec3f(-ROAD_X,BACKSTAGE_Y,BRAIN_Z) );
	brain->appendMovementVars( "brain_down", BRAIN_FPS, BRAIN_WALK_DURATION*0.667, Vec3f(-ROAD_X,PROG_Y,BRAIN_Z), 0.0f );
	brain->appendMovementVars( "brain_right", BRAIN_FPS, BRAIN_WALK_DURATION*0.333, Vec3f(PROG_X_BRAIN,PROG_Y,BRAIN_Z), 0.0f );
	
	// GET PROGGED SUCKER
	const float PROG_DURATION = 1.0f;
	const float HUMAN_PROG_Z = -0.3f;
	const float HUMAN_PROG_Z_VIBRATE = -0.7f;
	brain->appendMovementVars( "brain_right", 0, PROG_DURATION+0.5f, Vec3f(PROG_X_BRAIN,PROG_Y,BRAIN_Z), 0.0f );

	// Human prog motion is more complicated. Jitter up and down.
	const int PROG_COLOR_SEQ = 1;
	const int PROG_VIBRATE_STEPS = 20;
	for( int i=0; i<PROG_VIBRATE_STEPS; i++ ) {
		human->appendMovementVars( humanPrefix+"prog_inv", 0, (PROG_DURATION/PROG_VIBRATE_STEPS), Vec3f(PROG_X_HUMAN,PROG_Y,HUMAN_PROG_Z+HUMAN_PROG_Z_VIBRATE*randFloat(1.0)), 0.0f, PROG_COLOR_SEQ );
	}
	
	// Brain walks away. Let's try: right (some distance), up to the wall, right to the road, up to backstage.
	float initialRightRatio = randFloat(1.0f);
	brain->appendMovementVars( "brain_right", BRAIN_FPS, BRAIN_WALK_DURATION*0.667*(initialRightRatio*0.5), Vec3f(ROAD_X*initialRightRatio,PROG_Y,BRAIN_Z), 0.0f );	// right (some distance)
	brain->appendMovementVars( "brain_up", BRAIN_FPS, BRAIN_WALK_DURATION*0.667*0.5, Vec3f(ROAD_X*initialRightRatio,FRONT_Y,BRAIN_Z), 0.0f ); // up to the wall
	brain->appendMovementVars( "brain_right", BRAIN_FPS, BRAIN_WALK_DURATION*0.667*((1.0f-initialRightRatio)*0.5), Vec3f(ROAD_X,PROG_Y,BRAIN_Z), 0.0f );	// right to the road
	brain->appendMovementVars( "brain_up", BRAIN_FPS, BRAIN_WALK_DURATION*0.333, Vec3f(ROAD_X,BACKSTAGE_Y,BRAIN_Z), 0.0f );
	
	// Prog'd human flies away... with afterimages
	const float PROG_FLY_TIME = 4.0f;
	float afterImageDelay = HUMAN_WALK_DURATION + PROG_DURATION;
	Vec3f pt0 = Vec3f(ROAD_X,PROG_Y,HUMAN_PROG_Z);
	Vec3f pt1 = Vec3f(ROAD_X,BACK_Y-20.0f,HUMAN_PROG_Z);
	const int IMAGE_COLOR_SEQ = 4;
	
	// Create afterimages
	for( int i=0; i<3; i++ ) {
		PixelModel* image = this->getNextModel();
		image->appendMovementInvisible( afterImageDelay + (i+1)*0.2f, Vec3f(PROG_X_HUMAN,PROG_Y,HUMAN_PROG_Z) );
		image->appendMovementVars( humanPrefix+"prog_inv", 0, PROG_FLY_TIME*0.2, pt0, 0.0f, IMAGE_COLOR_SEQ );
		image->appendMovementVars( humanPrefix+"prog_inv", 0, PROG_FLY_TIME*0.8, pt1, M_PI/2, IMAGE_COLOR_SEQ );
	}
	
	human->appendMovementVars( humanPrefix+"prog", 0, PROG_FLY_TIME*0.2, pt0, 0.0f, PROG_COLOR_SEQ );
	human->appendMovementVars( humanPrefix+"prog", 0, PROG_FLY_TIME*0.8, pt1, M_PI/2, PROG_COLOR_SEQ );
	
	// Phew! That was compicated.
	mSequenceTimeRemaining = HUMAN_WALK_DURATION + PROG_DURATION;
}

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


