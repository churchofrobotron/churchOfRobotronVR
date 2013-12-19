#include "cinder/app/AppNative.h"
#include "cinder/Camera.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"

#include "FPSCamUI.h"

#include "robotronScreen.h"
#include "MovieObject.h"
#include "OculusVR.h"
#include "CameraStereoHMD.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class churchOfRobotronVRApp : public AppNative {
public:
	void setup();
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );

	void update();
	void draw();
  virtual void	resize();
private:
  ovr::DeviceRef              mOculusVR;
  ovr::DistortionHelperRef    mDistortionHelper;
  CameraStereoHMD             mStereoCamera;
  gl::Fbo                     mOculusFbo;
  
  RobotronScreen mScreen;
  FpsCamUI mCamera;
  MovieObjectPtr mSermon;
  
  void oculusInit();
  void renderScene();
};

void churchOfRobotronVRApp::setup()
{
  oculusInit();
  
  // Create Render Target a bit bigger to compensate the distortion quality loss
  gl::Fbo::Format format;
  format.enableColorBuffer();
  format.enableDepthBuffer();
//  format.setSamples( 8 );
  
  mOculusFbo = gl::Fbo( 1600, 1000, format );
  mDistortionHelper   = ovr::DistortionHelper::create();
  // Create Stereo Camera
  //  mStereoCamera = CameraStereoHMD( 640, 800, mOculusVR ? mOculusVR->getFov() : 125, mOculusVR ? mOculusVR->getEyeToScreenDistance() : 10, 10000.0f );
  mStereoCamera.lookAt(Vec3f::zero(), Vec3f::yAxis(), Vec3f::zAxis());
//  mStereoCamera.setEyePoint( Vec3f(0,-1,0) ); //Vec3f::zero());
//  mStereoCamera.lookAt(Vec3f::zero());
//  mStereoCamera.setWorldUp( Vec3f::zAxis());
  
  // Make the stereo a bit stronger
  mStereoCamera.setEyeSeparation( 0.15f );
  
  mScreen.init();
  CameraPersp cam;
  cam.lookAt(Vec3f::zero(), Vec3f::yAxis(), Vec3f::zAxis());
  
  mCamera.setCurrentCam(cam);
  mCamera.registerEvents();
  mCamera.setSpeed(0.001);
  
  mSermon = make_shared<MovieObject>("/Users/bzztbomb/projects/churchOfRobotron/videos/church_of_robotron_sermon-__doctrine_of_error_640x472.mp4");
  
  setWindowSize( 1280, 800 );
}

void churchOfRobotronVRApp::oculusInit()
{
  OVR::System::Init( OVR::Log::ConfigureDefaultLog( OVR::LogMask_All ) );
  
  // Init OVR
  mOculusVR           = ovr::Device::create();
}

void churchOfRobotronVRApp::mouseDown(MouseEvent event)
{
}

void churchOfRobotronVRApp::mouseDrag(MouseEvent event)
{
}

void churchOfRobotronVRApp::update()
{
  // Extrat Oculus Orientation and Update Camera
  Quatf orientation;
  
  if( mOculusVR )
  {
    orientation = mOculusVR->getOrientation();
//    mStereoCamera.setOrientation( orientation * Quatf( Vec3f( 0, 1, 0 ), M_PI ) );
    mStereoCamera.setOrientation( orientation * Quatf( Vec3f::xAxis(), M_PI / 2.0f) );
  } else {
    mCamera.update();
  }
  
  mCamera.update();
  mSermon->update();
}

void churchOfRobotronVRApp::resize()
{
}

void churchOfRobotronVRApp::draw()
{
  bool normalRender = false;
  if (normalRender)
  {
    // clear out the window with black
    gl::clear( Color( 0, 0, 0 ) );
    gl::setMatrices(mCamera.getCamera());
    
    renderScene();
    
    gl::popMatrices();
  } else {
    // clear out the window with black
    gl::clear( Color( 0, 0, 0 ) );
    
    {
      // Start Rendering to Our Side by Side RenderTarget
      gl::SaveFramebufferBinding bindingSaver;
      mOculusFbo.bindFramebuffer();
      
      // Clear
      gl::clear( ColorA( 1.0f, 0.0f, 1.0f, 1.0f ) );
      
      // Render Left Eye
      gl::setViewport( Area( Vec2f( 0.0f, 0.0f ), Vec2f( mOculusFbo.getWidth() / 2.0f, mOculusFbo.getHeight() ) ) );
      mStereoCamera.enableStereoLeft();
      gl::setMatrices(mStereoCamera);
      renderScene();
      
      // Render Right Eye
      gl::setViewport( Area( Vec2f( mOculusFbo.getWidth() / 2.0f, 0.0f ), Vec2f( mOculusFbo.getWidth(), mOculusFbo.getHeight() ) ) );
      mStereoCamera.enableStereoRight();
      gl::setMatrices(mStereoCamera);
      renderScene();
    }
    
    // Back to 2d rendering
    gl::setMatricesWindow( getWindowSize() );
    gl::setViewport( getWindowBounds() );
    gl::disableDepthRead();
    gl::disableDepthWrite();

    gl::color(Color::white());
    
    // Send the Side by Side texture to our distortion correction shader
    mOculusFbo.getTexture().setFlipped(true);
    mDistortionHelper->render( mOculusFbo.getTexture(), getWindowBounds() );
    
    // TODO: Draw FPS
  }
}

void churchOfRobotronVRApp::renderScene()
{
  mSermon->render();
  mScreen.draw();
}

CINDER_APP_NATIVE( churchOfRobotronVRApp, RendererGl )
