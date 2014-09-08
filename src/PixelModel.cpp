//
//  PixelModel.cpp
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 1/22/14.
//
//

#include "PixelModel.h"
#include "cinder/gl/gl.h"
#include "cinder/app/App.h"
#include "churchUtil.h"
#include "cinder/Rand.h"
#include "PixelModelDirector.h"

using namespace ci;
using namespace ci::gl;
using namespace ci::app;

inline cinder::Vec3f lerpVec3( Vec3f a, Vec3f b, float p ) {
	return Vec3f( lerp(a.x,b.x,p), lerp(a.y,b.y,p), lerp(a.z,b.z,p) );
}

void PixelModel::init(cinder::params::InterfaceGl* params)
{
  mScale = Vec3f(0.46f, 0.46f, 0.46f);
	mRotationRads = 0.0f;
  mPosition = Vec3f(0.0f, 14.0f, -1.27f);
  params->addSeparator(mParamPrefix);
  params->addParam(mParamPrefix + ": Position", &mPosition);
  params->addParam(mParamPrefix + ": Scale", &mScale);
}

void PixelModel::clearMovements() {
	mMovements.resize( 0 );
}

void PixelModel::appendMovement( ModelMovement movement )
{
	movement.elapsed = 0.0f;
	
	// If this is the first animation, then copy .prevLoc to .loc
	if( !mMovements.size() ) {
		movement.prevLoc = movement.loc;
		
		// Get FPS and animation
		this->setFPS( movement.fps );
		
	} else {
		// Close the ending location of the previous animation.
		ModelMovement &lastMove = mMovements.back();
		movement.prevLoc = lastMove.loc;
		movement.prevRotation = lastMove.rotation;
		
		// If required stuff is missing (animation key, etc) then
		// copy it from the previous animation.
		if( movement.animKey.empty() ) movement.animKey = lastMove.animKey;
		if( !movement.fps ) movement.fps = lastMove.fps;
	}
	
	mMovements.push_back( movement );
}

#pragma mark - Convenience methods for building animations

// Default rotation: 0.0
void PixelModel::appendMovementInvisible( float duration, cinder::Vec3f loc ) {
	this->appendMovementInvisible( duration, loc, 0.0f );
}

void PixelModel::appendMovementInvisible( float duration, cinder::Vec3f loc, float rotation ) {
	ModelMovement move;
	move.isVisible = FALSE;
	move.loc = loc;
	move.rotation = rotation;
	move.duration = duration;
	
	this->appendMovement( move );
}

void PixelModel::appendMovementVars( std::string animKey, float fps, float duration, Vec3f loc, float rotation ){
	ModelMovement move;
	move.isVisible = TRUE;
	move.animKey = animKey;
	move.fps = fps;
	move.duration = duration;
	move.loc = loc;
	move.rotation = rotation;
	move.alwaysFaceAltar = FALSE;
	
	this->appendMovement( move );
}

void PixelModel::appendMovementVarsFacingAltar( std::string animKey, float fps, float duration, Vec3f loc ) {
	ModelMovement move;
	move.isVisible = TRUE;
	move.animKey = animKey;
	move.fps = fps;
	move.duration = duration;
	move.loc = loc;
	move.alwaysFaceAltar = TRUE;
	
	this->appendMovement( move );
}

#pragma mark - Animation state

void PixelModel::setAnimationKey( std::string key )
{
	// ignore redundant changes
	if( key == mAnimKey ) return;
	mAnimKey = key;
	
	mAnimElapsed = 0;
	
	// Get the actual meshes, yarrr
	// std::vector<cinder::gl::VboMeshRef> inFrames
	mFrames = PixelModelDirector::getInstance().animationMeshesForKey( key );
}

void PixelModel::setFPS( float inFPS ) {
	mFPS = inFPS;
}

// Self-calling function. Advances the current movement,
//   and pops it off the front of mMovements if the movement has expired.
void PixelModel::applyMovementElapsed( float elapsed ) {
	if( !mMovements.size() ) return;
	
	// Advance this movement's time
	ModelMovement &move = mMovements[0];
	move.elapsed += elapsed;
	
	// Debugging working with deque of structs
	//std::cout << " elapsed: " << move.elapsed << " :: duration: " << move.duration << "\n";
	
	if( move.elapsed >= move.duration ) {
		float overflow = move.elapsed - move.duration;
		mMovements.pop_front();
		if( !mMovements.size() ) return;
		
		// Get the new animation & FPS
		ModelMovement &newMove = mMovements[0];
		this->setFPS( newMove.fps );
		this->setAnimationKey( newMove.animKey );
		
		// The extra time should be applied to the new animation
		this->applyMovementElapsed( overflow );
	}
}

#pragma mark - Per frame

void PixelModel::update( float elapsed/*, PixelModelDirector* director*/ )
{
	// self-calling function which might reduce mMovements to 0 items
	this->applyMovementElapsed( elapsed );
	
	// sanity checks
	if( !mFrames.size() ) return;
	if( !mMovements.size() ) return;
	
	// Get the current movement
	ModelMovement &move = mMovements[0];
	mIsVisible = move.isVisible;

	float progress = 0.0f;
	if( move.duration > 0.0f ) {
		progress = move.elapsed / move.duration;	// Range: 0..1
	}
	
	// Calculate the current position
	mPosition = lerpVec3( move.prevLoc, move.loc, progress );
	
	// Always face the altar?
	if( move.alwaysFaceAltar ) {
		mRotationRads = atan2f( mPosition.y, mPosition.x ) + M_PI*0.5f;
	} else {
		mRotationRads = lerp( move.prevRotation, move.rotation, progress );
	}
	
	mAnimElapsed += elapsed;
}

void PixelModel::draw()
{
	//std::cout << "Ready to draw. frames: " << mFrames.size() << " :: movements: " << mMovements.size() << "\n";
	
	// sanity checks
	if( !mFrames.size() ) return;
	if( !mMovements.size() ) return;
	
	// Some moves are invisible.
	// Typically this occurs on the first movement, used to delay the PixelModel's entrance.
	if( !mIsVisible ) return;
	
	gl::pushMatrices();
	gl::translate(mPosition);
	gl::scale(mScale);
	gl::rotate(Vec3f( 0, 0, mRotationRads*(180/M_PI) ));
	
	// Select the correct animation frame (mesh).
	// Do this here in draw(), for sanity reasons. There was a crash between update() and draw() because
	//   the model changed to an animation with fewer frames than mCurrFrame (set in update()). The bug has
	//   probably been fixed, but I still feel safer selecting the correct frame here :P
	int totalFrames = mAnimElapsed * mFPS;
	int currentFrame = totalFrames % mFrames.size();
	gl::draw(mFrames.at(currentFrame));
	
	gl::popMatrices();
}
