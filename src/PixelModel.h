//
//  PixelModel.h
//  churchOfRobotronVR
//
//  Created by Brian Richardson on 1/22/14.
//
//

#ifndef __churchOfRobotronVR__PixelModel__
#define __churchOfRobotronVR__PixelModel__

#include "cinder/TriMesh.h"
#include "cinder/params/Params.h"
#include "cinder/gl/Vbo.h"
//#include "PixelModelDirector.h"

struct ModelMovement {
	// Simple motion tweens. These values are supplied from previous object:
	cinder::Vec3f prevLoc;
	float prevRotation;	// in radians
	//float prevScale;
	
	BOOL isVisible;
	float duration;
	float elapsed;
	//enum easing;	// ???
	std::string animKey;
	float fps;
	BOOL alwaysFaceAltar;
	
	cinder::Vec3f loc;
	float rotation;	// in radians
	//float scale;
};

class PixelModel
{
public:
  PixelModel(const std::string& prefix)
  {
    mParamPrefix = prefix;
  }
  
	void init(cinder::params::InterfaceGl* params);
	void update( float elapsed/*, PixelModelDirector* director*/ );
	void draw();

	// Movement
	void clearMovements();

	void appendMovement( ModelMovement movement );
	void appendMovementInvisible( float duration, cinder::Vec3f loc );
	void appendMovementInvisible( float duration, cinder::Vec3f loc, float rotation );
	void appendMovementVars( std::string animKey, float fps, float duration, cinder::Vec3f loc, float rotation );
	void appendMovementVarsFacingAltar( std::string animKey, float fps, float duration, cinder::Vec3f loc );

	std::deque<ModelMovement> mMovements;

private:
	void applyMovementElapsed( float elapsed );
	void setAnimationKey( std::string key );
	void setFPS( float inFPS );
	
	BOOL mIsVisible;
	std::string mParamPrefix;
	cinder::Vec3f mPosition;
	cinder::Vec3f mScale;
	float mRotationRads;	// Apparent rotation on Y axis. (It's actually the model's Z axis)
	
	std::vector<cinder::gl::VboMeshRef> mFrames;

	// animation timing
	float mFPS;
	float mAnimElapsed;
	std::string mAnimKey;
};

#endif /* defined(__churchOfRobotronVR__PixelModel__) */
