
//
//  Camera.cpp
//  Landscape
//
//  Created by Simon Geilfus on 21/11/12.
//
//

#include "FpsCamUI.h"

using namespace ci;
using namespace ci::app;
using namespace std;


FpsCamUI::FpsCamUI()
{
  mCurrentCam         = ci::CameraPersp();
  mSpeed              = 1.0f;
  mPositionVelocity   = Vec3f::zero();
  mOrientationTo      = mOrientation = Quatf( 0.0f, 0.0f, 0.0f );
  mUpIsDown           = mDownIsDown = mLeftIsDown = mRightIsDown = mMouseDown = mHigherIsDown = mLowerIsDown = false;
}
FpsCamUI::FpsCamUI( const ci::CameraPersp &aInitialCam )
{
  mCurrentCam         = aInitialCam;
  mSpeed              = 1.0f;
  mPositionVelocity   = Vec3f::zero();
  mOrientationTo      = mOrientation = Quatf( 0.0f, 0.0f, 0.0f );
  mUpIsDown           = mDownIsDown = mLeftIsDown = mRightIsDown = mMouseDown = mHigherIsDown = mLowerIsDown = false;
}

FpsCamUI::~FpsCamUI()
{
  if( mEventsCallbacks.size() )
    unregisterEvents();
}

void FpsCamUI::setCurrentCam( const ci::CameraPersp &aCurrentCam )
{
  mCurrentCam = aCurrentCam;
  mOrientation = mCurrentCam.getOrientation();
  mOrientationTo = mCurrentCam.getOrientation();
}

void FpsCamUI::forward(){ mPositionVelocity += mCurrentCam.getViewDirection() * mSpeed; }
void FpsCamUI::back(){ mPositionVelocity -= mCurrentCam.getViewDirection() * mSpeed; }
void FpsCamUI::left(){ mPositionVelocity -= mCurrentCam.getViewDirection().cross( Vec3f(0,1,0) ) * mSpeed; }
void FpsCamUI::right(){ mPositionVelocity += mCurrentCam.getViewDirection().cross( Vec3f(0,1,0) ) * mSpeed; }
void FpsCamUI::higher(){ mPositionVelocity += ( Vec3f(0,1,0) ) * mSpeed; }
void FpsCamUI::lower(){ mPositionVelocity -= ( Vec3f(0,1,0) ) * mSpeed; }

void FpsCamUI::update( bool yContraint, float y )
{
  if( mUpIsDown ) forward();
  if( mDownIsDown ) back();
  if( mLeftIsDown ) left();
  if( mRightIsDown ) right();
  if( mHigherIsDown ) higher();
  if( mLowerIsDown ) lower();
  if( mMouseDown )
  {
    
    
    mTimeElapsed = AppBasic::get()->getElapsedSeconds() - mLastTime;
    
    Vec2f mouseMovement = ( getWindowCenter() - mMousePos ) * 0.005f;
    mOrientationTo = Quatf( mouseMovement.y, 0.0f, 0.0f ) * mCurrentCam.getOrientation() * Quatf( 0.0f, 0.0f, mouseMovement.x );
    
    if( mTimeElapsed > 0.0005 ){
#if defined( CINDER_MAC )
      CGSetLocalEventsSuppressionInterval(0);
      if( !App::get()->isFullScreen() )
      {
        CGPoint p;
        p.x = App::get()->getWindowPosX() + getWindowCenter().x;
        p.y = App::get()->getWindowPosY() + getWindowCenter().y;
        CGWarpMouseCursorPosition(p);
      }
      //  CGWarpMouseCursorPosition( { App::get()->getWindowPosX() + getWindowCenter().x, App::get()->getWindowPosY() + getWindowCenter().y } );
      else
      {
        CGPoint p;
        p.x = getWindowCenter().x;
        p.y =  getWindowCenter().y;
        CGWarpMouseCursorPosition(p);
      }
      // CGWarpMouseCursorPosition( { getWindowCenter().x, getWindowCenter().y } );
#else
      POINT pt;
      pt.x = App::get()->getWindowWidth() / 2;
      pt.y = App::get()->getWindowHeight() / 2;
      
      HWND hWnd = App::get()->getRenderer()->getHwnd();
      ::ClientToScreen(hWnd, &pt);
      ::SetCursorPos(pt.x,pt.y);
#endif
    }
    
    mLastTime = AppBasic::get()->getElapsedSeconds();
    mMousePos = getWindowCenter();
  }
  
  
  Vec3f eyePoint = mCurrentCam.getEyePoint() + mPositionVelocity;
  if( yContraint )
    eyePoint.y = y;
  
  mCurrentCam.setEyePoint( eyePoint );
  mPositionVelocity *= 0.87f;
  
  mOrientation = mOrientation.slerp( 0.1f, mOrientationTo );
  mCurrentCam.setOrientation( mOrientationTo );
}

bool FpsCamUI::mouseDown( ci::app::MouseEvent event ){
#if defined( CINDER_MAC )
  CGSetLocalEventsSuppressionInterval(0);
  //console() << "x" << endl;
  CGPoint p;
  p.x = App::get()->getWindowPosX() + App::get()->getWindowWidth() / 2.0f;
  p.y = App::get()->getWindowPosY() + App::get()->getWindowHeight() / 2.0f;
  
  CGWarpMouseCursorPosition(p);
  //  CGWarpMouseCursorPosition(
  // { App::get()->getWindowPosX() + App::get()->getWindowWidth() / 2.0f, App::get()->getWindowPosY() + App::get()->getWindowHeight() / 2.0f }
  
  
#else
  POINT pt;
  pt.x = AppBasic::get()->getWindowWidth() / 2;
  pt.y = AppBasic::get()->getWindowHeight() / 2;
  
  HWND hWnd = AppBasic::get()->getRenderer()->getHwnd();
  ::ClientToScreen(hWnd, &pt);
  ::SetCursorPos(pt.x,pt.y);
#endif
  mMouseDown = true;
  mMousePos = event.getPos();
  AppBasic::get()->hideCursor();
  return false;
}

bool FpsCamUI::mouseDrag( ci::app::MouseEvent event ){
  
  mMousePos = event.getPos();
  return false;
}

bool FpsCamUI::mouseUp( ci::app::MouseEvent event ){
  mMouseDown = false;
  AppBasic::get()->showCursor();
  return false;
}
bool FpsCamUI::mouseMove( ci::app::MouseEvent event )
{
  //   console() << "x";
  mMousePos = event.getPos();
  return false;
}


bool FpsCamUI::keyDown( KeyEvent event )
{
  if (event.isAltDown())
    return false;

  //   console() << "key";
  int code = event.getCode();
  if( code == 'w' || code == KeyEvent::KEY_UP ) mUpIsDown = true;
  else if( code == 's' || code == KeyEvent::KEY_DOWN ) mDownIsDown = true;
  else if( code == 'a' || code == KeyEvent::KEY_LEFT ) mLeftIsDown = true;
  else if( code == 'd' || code == KeyEvent::KEY_RIGHT ) mRightIsDown = true;
  else if( code == 'q' ) mHigherIsDown = true;
  else if( code == 'z' ) mLowerIsDown = true;
  return false;
}

bool FpsCamUI::keyUp( KeyEvent event )
{
  if (event.isAltDown())
    return false;
  
  int code = event.getCode();
  if( code == 'w' || code == KeyEvent::KEY_UP ) mUpIsDown = false;
  else if( code == 's' || code == KeyEvent::KEY_DOWN ) mDownIsDown = false;
  else if( code == 'a' || code == KeyEvent::KEY_LEFT ) mLeftIsDown = false;
  else if( code == 'd' || code == KeyEvent::KEY_RIGHT ) mRightIsDown = false;
  else if( code == 'q'  ) mHigherIsDown = false;
  else if( code == 'z'  ) mLowerIsDown = false;
  return false;
}

void FpsCamUI::registerEvents()
{
  ci::app::WindowRef w = getWindow();
  w->getSignalMouseDown().connect(std::bind(&FpsCamUI::mouseDown, this, std::placeholders::_1));
  w->getSignalMouseDrag().connect(std::bind(&FpsCamUI::mouseDrag, this, std::placeholders::_1));
  w->getSignalMouseUp().connect(std::bind(&FpsCamUI::mouseUp, this, std::placeholders::_1));
  w->getSignalMouseMove().connect(std::bind(&FpsCamUI::mouseMove, this, std::placeholders::_1));
  w->getSignalKeyDown().connect(std::bind(&FpsCamUI::keyDown, this, std::placeholders::_1));
  w->getSignalKeyUp().connect(std::bind(&FpsCamUI::keyUp, this, std::placeholders::_1));
}
void FpsCamUI::unregisterEvents()
{
  /*AppBasic::get()->unregisterMouseDown( mEventsCallbacks[ 0 ] );
   AppBasic::get()->unregisterMouseDrag( mEventsCallbacks[ 1 ] );
   AppBasic::get()->unregisterMouseUp( mEventsCallbacks[ 2 ] );
   AppBasic::get()->unregisterMouseMove( mEventsCallbacks[ 3 ] );
   AppBasic::get()->unregisterMouseWheel( mEventsCallbacks[ 4 ] );
   AppBasic::get()->unregisterKeyDown( mEventsCallbacks[ 5 ] );
   AppBasic::get()->unregisterKeyUp( mEventsCallbacks[ 6 ] );
   */}
