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
#include "cinder/gl/GlslProg.h"
#include "cinder/params/Params.h"
#include "cinder/gl/Vbo.h"
#include "PixelModel.h"

class PixelModelDirector
{

public:
	static PixelModelDirector& getInstance()
	{
		static PixelModelDirector instance;
		return instance;
	}
	
	std::vector<cinder::gl::VboMeshRef> animationMeshesForKey( std::string key );
	
	void init( cinder::params::InterfaceGl* params );
	void update();
	void draw();
	
private:
	// Singleton pattern
	PixelModelDirector(){}
	PixelModelDirector( PixelModelDirector const& );
	void operator=(PixelModelDirector const& );

	static std::vector<cinder::Area> walkAreas( cinder::Area home, int offsetX, BOOL dipHomeFrame );
	static std::vector<cinder::Area> fourFrameAreas( cinder::Area home, int offsetX );

	void cacheAnimation( cinder::Surface8u allSprites, std::string key, std::vector<cinder::Area> areas );
	std::map< std::string, std::vector<cinder::gl::VboMeshRef> > mAnimations;
	std::vector<PixelModel*> mModels;
	
	// Shader + color cycling
	cinder::gl::GlslProg mPixelModelShader;
	cinder::Vec4f colorCycle0;
	cinder::Vec4f colorCycle1;
	cinder::Vec4f colorCycle2;
	
	cinder::Timer mTimer;
	double mPrevSeconds;
	
	// Internal: Choreographed sequence
	int mModelIdx;
	float mSequenceTimeRemaining;
	float mTimeSinceRareSeq;
	int mRareSeqIndex;
	
	PixelModel* getNextModel();

	// Common sequences
	void startSequence_EnforcersFlyOver();
	void startSequence_Tank();

	// Rare sequences
	void startSequence_brainProgsHuman();

	// Dev sequences
	void startSequence_HerdOfGrunts();
	void startSequence_TestAllAnims();
};

#endif /* defined(__churchOfRobotronVR__PixelModelDirector__) */
