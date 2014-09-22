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

// 0x888800: Prog shadows, Enforcer eyes
// These are not the "official" values from the game. Just need a fast sequence here
const float COLOR_CYCLE_0_FPS = 12.0f;
const uint COLOR_CYCLE_0_COUNT = 10;
const uint COLOR_CYCLE_0_AR[] = {
	0xff00ff, 0x00ff00, 0xffff00, 0x8800ff, 0xffffff,
	0x8dddf4, 0xff0000, 0x00ffff, 0xac373e, 0x981AAF //10
};

// 0x888888: Brains
const float COLOR_CYCLE_1_FPS = 60.0f;
const uint COLOR_CYCLE_1_COUNT = 44;
const uint COLOR_CYCLE_1_AR[] = {
	0x1A0027, 0x1A0027, 0x0000f0, 0x0000f0, 0x0000f0,
	0x0000f0, 0x1e48f3, 0x1e48f3, 0x1e48f3, 0x3079ff,//10
	0x3079ff, 0x4cbdff, 0x59deff, 0x64daff, 0x92e3ff,
	0x92e3ff, 0x8fe0ff, 0x8fe0ff, 0x8fe0ff, 0x8dddf4,//20
	0x8dddf4, 0x89d9c5, 0x78e0b0, 0xa1a1a1, 0xa1a1a1,
	0xa1a1a1, 0xb67d84, 0xb67d84, 0xa85e75, 0xba5d4c,//30
	0xba5d4c, 0xac373e, 0xac373e, 0x9f1732, 0x9f1732,
	0x87132e, 0x6b0f2a, 0x6b0f2a, 0x540b26, 0x540b26,//40
	0x480925, 0x480925, 0x300521, 0x300521
};

// 0x8888ff: Prog head, hulk arms
const float COLOR_CYCLE_2_FPS = 14.0f;
const uint COLOR_CYCLE_2_COUNT = 20;
const uint COLOR_CYCLE_2_AR[] = {
	0x200A9F, 0x06079B, 0x3A1096, 0x5511A5, 0x6213A7,
	0x7C16AB, 0x981AAF, 0xB11DB1, 0xAD1B78, 0xAE194F,//10
	0xA61616, 0xAE194F, 0xAD1B78, 0xB11DB1, 0x981AAF,
	0x7C16AB, 0x6213A7, 0x5511A5, 0x3A1096, 0x06079B //20
};


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
	this->cacheAnimation( allSprites, "brain_down", PixelModelDirector::walkAreas( Area(1, 179, 1+13, 179+15), 19, true ));
	this->cacheAnimation( allSprites, "brain_up", PixelModelDirector::walkAreas( Area(39, 83, 39+13, 83+15), 19, true ));
	this->cacheAnimation( allSprites, "brain_right_prog", {Area(171,179,171+13,179+16)} );
	
	//
	// HUMANITY (rhymes with FUTILITY)
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

	this->cacheAnimation( allSprites, "skull_and_crossbones", {Area(1,1,12,12) });
	
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

#pragma mark - Animated sequences

// Instead of malloc/dealloc'ing PixelModels, just create a batch
// on init (~50) and reuse them. getNextModel() returns the next
// PixelModel in the list, and returns around to [0] at the end.
PixelModel* PixelModelDirector::getNextModel()
{
	PixelModel* model = mModels[mModelIdx];
	mModelIdx = (mModelIdx+1) % mModels.size();
	model->clearMovements();
	return model;
}

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
		mSequenceTimeRemaining += randFloat( 0.25f, 1.5f );
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
		startLoc = Vec3f( atX, FRONT_Y+13.0f, TANK_Z );
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
	mSequenceTimeRemaining += randFloat( 0.5f, 2.0f );
}

#pragma mark - Animations: Rare + longer
// Longer animations that occur in front of the altar, where players will see them more easily.
// These are longer and more elaborate: A Brain prog's a Human, etc.

// Hmm. Grunts. It will be awesome if 3-5 Grunts can emerge from the different
// passages, and converge to exit out another passage.
//
// Behold this amazing overhead map:
//
//     front
//   0 +---+ 1
//     | * |   <-- altar
//   2 +---+ 3
//     |   |
//   4 +---+ 5
//      back
//

const Vec2f PT0 = Vec2f( -ROAD_X, FRONT_Y-2.0f );
const Vec2f PT1 = Vec2f( ROAD_X, FRONT_Y-2.0f );
const Vec2f PT2 = Vec2f( -ROAD_X, ALLEY_LEFT_Y );
const Vec2f PT3 = Vec2f( ROAD_X, ALLEY_RIGHT_Y );
const Vec2f PT4 = Vec2f( -ROAD_X, BACK_Y );
const Vec2f PT5 = Vec2f( ROAD_X, BACK_Y );
const Vec2f PT_AR[] = {PT0,PT1,PT2,PT3,PT4,PT5};

// Exit points are off the sides/top/bottom.
const float MARGIN = 10.0f;
const Vec2f EXIT0 = Vec2f( PT0.x, PT0.y+MARGIN );
const Vec2f EXIT1 = Vec2f( PT1.x, PT1.y+MARGIN );
const Vec2f EXIT2 = Vec2f( PT2.x-MARGIN, PT2.y );
const Vec2f EXIT3 = Vec2f( PT3.x+MARGIN, PT3.y );
const Vec2f EXIT4 = Vec2f( PT4.x, PT4.y-MARGIN );
const Vec2f EXIT5 = Vec2f( PT5.x, PT5.y-MARGIN );
const Vec2f EXIT_AR[] = {EXIT0,EXIT1,EXIT2,EXIT3,EXIT4,EXIT5};

inline float distance( Vec2f pt0, Vec2f pt1 ) {
	Vec2f diff = Vec2f( pt0.x-pt1.x, pt0.y-pt1.y );
	return sqrtf( diff.x*diff.x + diff.y*diff.y );
}

inline Vec2f pointWithRandomness( Vec2f pt ) {
	const float POINT_RAND = 2.0f;
	return Vec2f( pt.x + randFloat(-POINT_RAND,POINT_RAND), pt.y + randFloat(-POINT_RAND,POINT_RAND) );
}

void PixelModelDirector::startSequence_GruntsOnAllSides() {
	const float GRUNT_SPEED = 0.10f;	// smaller => faster :P
	const float GRUNT_Z = -1.3f;
	const float GRUNT_FPS = 5.0;
	const float GRUNT_RANDOM_DELAY_BEFORE_ENTER = 5.0f;
	
	// Pick an exit. All Grunts will leave here.
	//int exitIdx = arc4random() % 6;
	
	// Actually I think this works best if they all converge where the player
	// can see them, so index 0 or 1
	int exitIdx = arc4random() % 2;
	
	// Build an array of exits to enter from. Obviously these should *not*
	// include the exitIdx.
	std::vector<int> enterIdx_ar;
	for( int i=0; i<6; i++ ) {
		if( i==exitIdx ) continue;
		enterIdx_ar.push_back(i);
	}

	// Shuffle the array
	std::random_shuffle( enterIdx_ar.begin(), enterIdx_ar.end() );

	// How many grunts?
	int gruntCount = 15 + arc4random() % 24;
	for( int g=0; g<gruntCount; g++ ) {
		PixelModel* grunt = this->getNextModel();
		int idx = enterIdx_ar.at( g % enterIdx_ar.size() );
		
		// Grunt should enter from EXIT_n.
		Vec2f pt = pointWithRandomness( EXIT_AR[idx] );
		
		grunt->appendMovementInvisible(randFloat(GRUNT_RANDOM_DELAY_BEFORE_ENTER), Vec3f(pt.x,pt.y,GRUNT_Z));
		
		Vec2f pt2 = pointWithRandomness( PT_AR[idx] );
		float time = GRUNT_SPEED * distance( pt, pt2 );
		grunt->appendMovementVars( "grunt", GRUNT_FPS, time, Vec3f(pt2.x,pt2.y,GRUNT_Z), 0.0f );
		
		// Make a path to the exit. Hmm, this is kind of a weird problem!
		// Given the map coordinates, we can move to different points by
		// adding specific values to our current index.
		//   2==back. -1==left. etc.
		// So let's make an array of steps to get from the entrance index
		// to the exit, and then shuffle it. HOLY SHIT THE GENIUS IS SO BRIGHT
		// MY EYES AGULAHULAGAULHAUG
		std::vector<int> step_ar;
		
		// Left-or-right movement is needed?
		if( (idx%2) != (exitIdx%2) ) {
			step_ar.push_back( ((idx%2)==1) ? -1 : 1 );
		}
		
		// Up-and-down movement
		int idx_2 = (int)floorf(idx/2);
		int exitIdx_2 = (int)floorf(exitIdx/2);
		for( int y=idx_2; y<exitIdx_2; y++ ) {
			step_ar.push_back( 2 );
		}
		for( int y=exitIdx_2; y<idx_2; y++ ) {
			step_ar.push_back( -2 );
		}
		
		std::random_shuffle( step_ar.begin(), step_ar.end() );
		
		// For each step: Walk to the new location
		for( int s=0; s<step_ar.size(); s++ ) {
			idx += step_ar[s];
			
			// Sanity check: idx must be in range [0..5] :P
			if( (idx<0) || (idx>5) ) break;
			
			pt = pt2;
			pt2 = pointWithRandomness( PT_AR[idx] );
			time = GRUNT_SPEED * distance( pt, pt2 );
			grunt->appendMovementVars( "grunt", GRUNT_FPS, time, Vec3f(pt2.x,pt2.y,GRUNT_Z), 0.0f );
			
			// Add a micro delay
			grunt->appendMovementVars( "grunt", GRUNT_FPS, randFloat(0.4), Vec3f(pt2.x,pt2.y,GRUNT_Z), 0.0f );
		}
		
		// Exit, and we're done
		pt = pt2;
		pt2 = pointWithRandomness( EXIT_AR[exitIdx] );
		time = GRUNT_SPEED * distance( pt, pt2 );
		grunt->appendMovementVars( "grunt", GRUNT_FPS, time, Vec3f(pt2.x,pt2.y,GRUNT_Z), 0.0f );
		
		// And we're done!
	}
	
	// Lazy: Just wait a few seconds before next anim starts
	mSequenceTimeRemaining += 12.0f;
}

const float HUMAN_Z = -1.3f;
const float HUMAN_FPS = 4.0f;
const float BACKSTAGE_Y = FRONT_Y + 10.0f;

std::string randomHumanPrefix() {
	int humanType = (arc4random() % 3);
	switch( humanType ) {
		case 0:	return "mommy_";
		case 1: return "daddy_";
		case 2: return "mikey_";
	}
	return "";
}

// Human enters upstage left, being chased by Hulk.
void PixelModelDirector::startSequence_hulkCrushesPunyHuman() {
	const float HULK_Z = -1.1f;
	const float CRUSH_Y = 3.25f;
	
	PixelModel* hulk = this->getNextModel();
	PixelModel* human = this->getNextModel();
	std::string humanPrefix = randomHumanPrefix();
	
	const float HUMAN_CHASE_TIME = 10.0f;
	const float FINAL_HUMAN_X = 0.5f;
	human->appendMovementInvisible( 0, Vec3f(-ROAD_X,BACKSTAGE_Y,HUMAN_Z));
	human->appendMovementVars(humanPrefix+"down", HUMAN_FPS, HUMAN_CHASE_TIME*0.334, Vec3f(-ROAD_X,FRONT_Y,HUMAN_Z), 0.0f );
	
	// Do a simple walk across backstage; at a random x, branch down.
	// So potential paths look like: |__, ^|_, ^^|
	float walkDownRatio = 0.15;	// multiply by HUMAN_CHASE_TIME for time spent walking down
	float downTime = HUMAN_CHASE_TIME*walkDownRatio;
	
	float branchAt = randFloat(0.75f,0.9f);	// Branch closer to the center, so it catches the user's eye
	float right0time = (HUMAN_CHASE_TIME*0.666 - downTime) * branchAt;
	float right1time = (HUMAN_CHASE_TIME*0.666 - downTime) * (1.0-branchAt);
	float branchX = lerp( -ROAD_X, FINAL_HUMAN_X, branchAt );
	
	human->appendMovementVars(humanPrefix+"right", HUMAN_FPS, right0time, Vec3f(branchX,FRONT_Y,HUMAN_Z), 0.0f );
	human->appendMovementVars(humanPrefix+"down", HUMAN_FPS, downTime, Vec3f(branchX,CRUSH_Y,HUMAN_Z), 0.0f );
	human->appendMovementVars(humanPrefix+"right", HUMAN_FPS, right1time, Vec3f(FINAL_HUMAN_X,CRUSH_Y,HUMAN_Z), 0.0f );
	
	// Okay. The Hulk can pretty much do the same thing, but he's faster (duh).
	const float HULK_FPS = 5.1f;
	const float HULK_CHASE_TIME = HUMAN_CHASE_TIME * 0.6f;
	const float HULK_CHASE_DELAY = HUMAN_CHASE_TIME - HULK_CHASE_TIME;
	const float HULK_CRUSH_X = FINAL_HUMAN_X - 2.0f;
	
	hulk->appendMovementInvisible( HULK_CHASE_DELAY, Vec3f(-ROAD_X,BACKSTAGE_Y,HULK_Z), 0.0f );
	hulk->appendMovementVars("hulk_down", HULK_FPS, HULK_CHASE_TIME*0.334f, Vec3f(-ROAD_X,FRONT_Y,HULK_Z), 0.0f );
	
	downTime = HULK_CHASE_TIME*walkDownRatio;
	branchAt = randFloat(0.6f,0.9f);	// Branch closer to the center, so it catches the user's eye
	right0time = (HULK_CHASE_TIME*0.666 - downTime) * branchAt;
	right1time = (HULK_CHASE_TIME*0.666 - downTime) * (1.0-branchAt);
	branchX = lerp( -ROAD_X, HULK_CRUSH_X, branchAt );
	
	hulk->appendMovementVars("hulk_right", HULK_FPS, right0time, Vec3f(branchX,FRONT_Y,HULK_Z), 0.0f );
	hulk->appendMovementVars("hulk_down", HULK_FPS, downTime, Vec3f(branchX,CRUSH_Y,HULK_Z), 0.0f );
	hulk->appendMovementVars("hulk_right", HULK_FPS, right1time, Vec3f(HULK_CRUSH_X,CRUSH_Y,HULK_Z), 0.0f );
	
	// GET CRUSHED! oh noes
	const float SKULL_Z = HUMAN_Z + 0.7f;
	human->appendMovementVars("skull_and_crossbones", 0, 0, Vec3f(FINAL_HUMAN_X,CRUSH_Y,SKULL_Z), 0.0f );
	human->appendMovementVars("skull_and_crossbones", 0, 4.0f, Vec3f(FINAL_HUMAN_X,CRUSH_Y,SKULL_Z), 0.0f );
	
	// Hulk: Keep moving right, then walk away to the back
	float timeToEdge = (right0time+right1time) * (fabsf(ROAD_X-HULK_CRUSH_X)/fabsf(HULK_CRUSH_X-ROAD_X));
	hulk->appendMovementVars("hulk_right", HULK_FPS, timeToEdge, Vec3f(ROAD_X,CRUSH_Y,HULK_Z), 0.0f );
	hulk->appendMovementVars("hulk_up", HULK_FPS, timeToEdge*4.0f, Vec3f(ROAD_X,BACK_Y-20.0f,HULK_Z), 0.0f );

	mSequenceTimeRemaining += 10.0f;
}

// Human enters upstage right. Brain enters upstage left.
void PixelModelDirector::startSequence_brainProgsHuman() {
	const float BRAIN_FPS = 6.0f;
	const float BRAIN_Z = -1.1f;
	
	const float PROG_X_BRAIN = -1.2f - 1.5f;
	const float PROG_X_HUMAN = -1.2f + 1.5f;
	const float PROG_Y = 3.0f;
	
	PixelModel* brain = this->getNextModel();
	PixelModel* human = this->getNextModel();
	std::string humanPrefix = randomHumanPrefix();
	
	// Semi-random walks. Emerge from front left&right streets, and meander towards the altar.
	const float HUMAN_WALK_DURATION = 10.0f;	// The brain will walk faster than the human.
	human->appendMovementInvisible( 0, Vec3f(ROAD_X,BACKSTAGE_Y,HUMAN_Z) );
	human->appendMovementVars( humanPrefix+"down", HUMAN_FPS, HUMAN_WALK_DURATION*0.334, Vec3f(ROAD_X,FRONT_Y,HUMAN_Z), 0.0f );
	human->appendMovementVars( humanPrefix+"left", HUMAN_FPS, HUMAN_WALK_DURATION*0.333, Vec3f(PROG_X_HUMAN,FRONT_Y,HUMAN_Z), 0.0f );
	human->appendMovementVars( humanPrefix+"down", HUMAN_FPS, HUMAN_WALK_DURATION*0.333, Vec3f(PROG_X_HUMAN,PROG_Y,HUMAN_Z), 0.0f );
	
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
	// "brain_right_prog" looks like crap here. Can't tell what's going on.
	brain->appendMovementVars( "brain_right", 0, PROG_DURATION+0.5f, Vec3f(PROG_X_BRAIN,PROG_Y,BRAIN_Z), 0.0f );

	// Human prog motion is more complicated. Jitter up and down.
	const int PROG_VIBRATE_STEPS = 20;
	for( int i=0; i<PROG_VIBRATE_STEPS; i++ ) {
		human->appendMovementVars( humanPrefix+"prog_inv", 0, (PROG_DURATION/PROG_VIBRATE_STEPS), Vec3f(PROG_X_HUMAN,PROG_Y,HUMAN_PROG_Z+HUMAN_PROG_Z_VIBRATE*randFloat(1.0)), 0.0f );
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
	
	// Create afterimages
	for( int i=0; i<3; i++ ) {
		PixelModel* image = this->getNextModel();
		image->appendMovementInvisible( afterImageDelay + (i+1)*0.2f, Vec3f(PROG_X_HUMAN,PROG_Y,HUMAN_PROG_Z) );
		image->appendMovementVars( humanPrefix+"prog_inv", 0, PROG_FLY_TIME*0.2, pt0, 0.0f );
		image->appendMovementVars( humanPrefix+"prog_inv", 0, PROG_FLY_TIME*0.8, pt1, -M_PI/2 );
	}
	
	human->appendMovementVars( humanPrefix+"prog", 0, PROG_FLY_TIME*0.2, pt0, 0.0f );
	human->appendMovementVars( humanPrefix+"prog", 0, PROG_FLY_TIME*0.8, pt1, -M_PI/2 );
	
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

#pragma mark - per frame

inline cinder::Vec4f hexColorToVec4f( uint hex ) {
	return Vec4f( ((hex&0xff0000)>>16) * (1.0f/0xff),
				  ((hex&0x00ff00)>> 8) * (1.0f/0xff),
				  ((hex&0x0000ff)    ) * (1.0f/0xff),
				 1.0 );
}

void PixelModelDirector::update()
{
	double seconds = mTimer.getSeconds();
	float elapsed = (float)(seconds - mPrevSeconds);
	mPrevSeconds = seconds;
	
	mTimeSinceRareSeq += elapsed;
	
	// Advance color cycling
	
	// OLD: Color0 (electrodes & tank fills): I don't have the actual values,
	// so I'm swizzling CC1 with an offset :P
	/*
	uint cc0idx = ((uint)(seconds * COLOR_CYCLE_0_FPS) + COLOR_CYCLE_0_OFFSET) % COLOR_CYCLE_1_COUNT;
	uint cc0 = COLOR_CYCLE_1_AR[cc0idx];
	//cc0	= ((cc0&0xffff00)>>8) | ((cc0&0x0000ff)<<16);	// 0xBBRRGG. "Borg" lol, looks too blue/green tho
	cc0 = ((cc0&0xff0000)>>8) | ((cc0&0x00ff00)<<8) | (cc0&0x0000ff);	// 0xGGRRBB
	colorCycle0 = hexColorToVec4f( cc0 );
	 */
	
	uint cc0idx = (uint)(seconds * COLOR_CYCLE_0_FPS) % COLOR_CYCLE_0_COUNT;
	colorCycle0 = hexColorToVec4f( COLOR_CYCLE_0_AR[cc0idx] );
	
	uint cc1idx = (uint)(seconds * COLOR_CYCLE_1_FPS) % COLOR_CYCLE_1_COUNT;
	colorCycle1 = hexColorToVec4f( COLOR_CYCLE_1_AR[cc1idx] );
	
	// CC2 sucks, it's too dark, so bitwise noodly to make it brighter :P
	uint cc2idx = (uint)(seconds * COLOR_CYCLE_2_FPS) % COLOR_CYCLE_2_COUNT;
	uint cc2 = COLOR_CYCLE_2_AR[cc2idx] << 3;
	uint r = (cc2&0xff0000)>>16;
	uint g = (cc2&0x00ff00)>>8;
	uint b = (cc2&0x0000ff);
	
	// Saturate. Remove dullest channel, and increase the brightest one.
	if( (r<g) && (r<b) ) {
		r = 0;
	} else if( g<b ) {
		g = 0;
	} else {
		b = 0;
	}
	
	if( (r>g) && (r>b) ) {
		r |= 0xdd;
	} else if( g>b ) {
		g |= 0xdd;
	} else {
		b |= 0xdd;
		// Bright blue by itself is a little weak, so boost another channel
		if( r>g ) {
			r |= 0xbb;
		} else {
			g |= 0xbb;
		}
	}
	
	cc2 = (r<<16) | (g<<8) | b;
	
	colorCycle2 = hexColorToVec4f( cc2 );
	
	mSequenceTimeRemaining -= elapsed;
	if( mSequenceTimeRemaining <= 0.0f ) {
		// Choose a new sequence. Yay
		
		// Test sequences. Don't use these in production :P
		//this->startSequence_HerdOfGrunts();
		//this->startSequence_TestAllAnims();
		
		BOOL useRareSeq = FALSE;	// Default: use a non-rare common sequence which occurs off to the side.
		if( (mTimeSinceRareSeq > MIN_TIME_BETWEEN_RARE_SEQUENCES) && (randFloat(1.0)<0.5f) ) {
			mTimeSinceRareSeq = 0.0f;	// reset
			useRareSeq = TRUE;
		}
		
		if( useRareSeq ) {	// Rare sequences
			// Sequence order: Grunts, Brain, Grunts, Hulk, (repeat)
			int r = mRareSeqIndex % 4;
			if( (r==0) || (r==2) ) {	// Grunts are super-good so do them more often
				this->startSequence_GruntsOnAllSides();
			} else if( r==1 ) {
				this->startSequence_brainProgsHuman();
			} else if( r==3 ) {
				this->startSequence_hulkCrushesPunyHuman();
			}
			
			mRareSeqIndex++;
			
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
	
	mPixelModelShader.uniform("colorCycle0",colorCycle0);
	mPixelModelShader.uniform("colorCycle1",colorCycle1);
	mPixelModelShader.uniform("colorCycle2",colorCycle2);
	
	for( auto model : mModels ) {
		model->draw();
	}
	
	mPixelModelShader.unbind();
}
