//
//  Camera.h
//  Landscape
//
//  Created by Simon Geilfus on 21/11/12.
//
//
#pragma once

#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"

class FpsCamUI
{
public:
  
	FpsCamUI();
	FpsCamUI( const ci::CameraPersp &aInitialCam );
  
  ~FpsCamUI();
  
  void update( bool yContraint = false, float y = 0.0f );
  
	const ci::CameraPersp& getCamera() const { return mCurrentCam; }
	void setCurrentCam( const ci::CameraPersp &aCurrentCam );
  
  void setSpeed( float speed ){ mSpeed = speed; }
  float getSpeed(){ return mSpeed; }
  
  bool mouseDown( ci::app::MouseEvent event );
  bool mouseDrag( ci::app::MouseEvent event );
  bool mouseMove( ci::app::MouseEvent event );
  bool mouseUp( ci::app::MouseEvent event );
  bool keyDown( ci::app::KeyEvent event );
  bool keyUp( ci::app::KeyEvent event );
  
  void registerEvents();
  void unregisterEvents();
  
  void forward();
  void back();
  void left();
  void right();
  void higher();
  void lower();
  
protected:
  
  void toggleCursorVisibility();
  
  std::vector< ci::CallbackId >	mEventsCallbacks;
  
  float                           mSpeed;
  ci::CameraPersp                 mCurrentCam;
  ci::Vec3f                       mPositionVelocity;
  ci::Vec2f                       mMousePos;
  ci::Quatf                       mOrientation, mOrientationTo;
	double							mTimeElapsed, mLastTime;
  bool                            mUpIsDown, mDownIsDown, mLeftIsDown, mRightIsDown, mHigherIsDown, mLowerIsDown;
  bool                            mMouseDown;
  bool                            mCursorHidden;
};