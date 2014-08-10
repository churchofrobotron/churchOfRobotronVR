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

// tEMP
#include "Enforcer.h"

using namespace ci;
using namespace ci::gl;
using namespace ci::app;

void PixelModelDirector::init( cinder::params::InterfaceGl* params )
{
	for( int i=0; i<8; i++ ) {
		Enforcer* enf = new Enforcer();
		enf->init(params);
		mModels.push_back(enf);
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
