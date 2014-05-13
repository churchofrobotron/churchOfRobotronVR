#include "cinder/app/AppNative.h"
#include "cinder/Camera.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/params/Params.h"
#include "cinder/Utilities.h"

#include "FPSCamUI.h"
#include "OculusVR.h"
#include "CameraStereoHMD.h"

#include "robotronScreen.h"
#include "MovieObject.h"
#include "leaderboard.h"
#include "environment.h"
#include "Grunt.h"
#include "Enforcer.h"
#include "MovieCubes.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct WindowData
{
  enum RenderType
  {
    rtNormal,
    rtOculus
  };
  
  WindowData()
  : mRenderType(rtNormal)
  {
  }

  RenderType mRenderType;
};

class churchOfRobotronVRApp : public AppNative {
public:
  void prepareSettings( Settings* settings );
	void setup();
	void keyDown( KeyEvent event );

	void update();
	void draw();
  virtual void	resize();
private:
  ovr::DeviceRef              mOculusVR;
  ovr::DistortionHelperRef    mDistortionHelper;
  CameraStereoHMD             mStereoCamera;
  gl::Fbo                     mOculusFbo;
  std::weak_ptr<cinder::app::Window> mOculusWindow;

  params::InterfaceGl mParams;
  bool mShowParams;
  
  FpsCamUI mCamera;
  
  Environment mEnvironment;
  RobotronScreen mScreen;
  MovieCubes mSermon;
  Leaderboard mLeaderboard;
  MovieObject mRandoms;
  Grunt mModel;
  Enforcer mEnforcer;
  
  void oculusInit();
  void renderScene();
  void checkWindows();
};

void churchOfRobotronVRApp::prepareSettings( Settings* settings )
{
	settings->setTitle( "Church of Robotron" );
}

void churchOfRobotronVRApp::setup()
{
  oculusInit();
  
  gl::disableVerticalSync();
  
  mShowParams = false;
  
  mParams = params::InterfaceGl( "Church of Robotron", Vec2i( 225, 200 ) );
  mParams.hide();
  
  // Create Render Target a bit bigger to compensate the distortion quality loss
  gl::Fbo::Format format;
  format.enableColorBuffer();
  format.enableDepthBuffer();
  format.setSamples( 8 );
  
  mOculusFbo = gl::Fbo( 1600, 1000, format );
  mDistortionHelper   = ovr::DistortionHelper::create();
  // Create Stereo Camera
  mStereoCamera.setFov(mOculusVR ? mOculusVR->getFov() : 125.0f);
  mStereoCamera.setEyePoint(Vec3f(0, 12, 0));
  mStereoCamera.lookAt(Vec3f::zero(), Vec3f::yAxis() + mStereoCamera.getEyePoint(), Vec3f::zAxis());
  
  // Need to tweak this..
  mStereoCamera.setEyeSeparation( 0.15f );
  mEnvironment.init(&mParams);
  mScreen.init(&mParams);
  mLeaderboard.init(&mParams);
  
  CameraPersp cam;
  cam.lookAt(Vec3f::zero(), Vec3f::yAxis(), Vec3f::zAxis());
  cam.setFov(90);
  
  mCamera.setCurrentCam(cam);
  mCamera.registerEvents();
  mCamera.setSpeed(0.01);
  
  const std::string sermonBase = "/Users/bzztbomb/projects/churchOfRobotron/videos/";
//  std::vector<string> sermons =
//  {
//    sermonBase + "church_of_robotron_sermon-__doctrine_of_error_640x472.mp4",
//    sermonBase + "church_of_robotron_sermon-_doctrine_of_futility_640x472.mp4",
//    sermonBase + "church_of_robotron_sermon-_eight_ways_640x472.mp4",
//    sermonBase + "church_of_robotron_sermon-_the_ninth_position_640x472.mp4",
//    sermonBase + "church_of_robotron_sermon-_what_are_the_robotrons_640x472.mp4"
//  };
  std::vector<string> sermons =
  {
    sermonBase + "rotojames.mov"
  };
  mSermon.setMovieList(sermons);
  mSermon.setMute(true);
  mSermon.init(&mParams);
  
  std::string downloads = "/Users/bzztbomb/Downloads/";
  std::vector<string> randoms =
  {
    downloads + "Glitch-logo-02.mp4",
    sermonBase + "116645203.mp4"
  };
  
  mRandoms.setMovieList(randoms);
  mRandoms.setMute(true);
  mRandoms.init(&mParams);
  
  mModel.init(&mParams);
  mEnforcer.init(&mParams);
  
  getWindow()->setUserData( new WindowData );
  
}

void churchOfRobotronVRApp::oculusInit()
{  
  // Init OVR
  mOculusVR           = ovr::Device::create();
}

void churchOfRobotronVRApp::keyDown( KeyEvent event )
{
  if (event.isAltDown())
  {
    switch (event.getCode())
    {
      case KeyEvent::KEY_w:
        checkWindows();
        break;
      case KeyEvent::KEY_t:
        {
          WindowData* wd = getWindow()->getUserData<WindowData>();
          wd->mRenderType = (wd->mRenderType == WindowData::rtNormal) ?
            WindowData::rtOculus : WindowData::rtNormal;
        }
        break;
      case KeyEvent::KEY_f:
        {
          FullScreenOptions fo;
          fo.secondaryDisplayBlanking(false);
          setFullScreen( !isFullScreen(), fo );
        }
        break;
      case KeyEvent::KEY_p:
        {
          if (mParams.isVisible())
            mParams.hide();
          else
            mParams.show();
        }
        break;
    }
  }
}

void churchOfRobotronVRApp::update()
{
  if( mOculusVR )
  {
    Quatf orientation = mOculusVR->getOrientation();
    mStereoCamera.setOrientation( orientation * Quatf( Vec3f::xAxis(), M_PI / 2.0f) );
  }
  
  mCamera.update();
  mEnvironment.update();
  mSermon.update();
  mRandoms.update();
  mLeaderboard.update();
  mModel.update();
  mEnforcer.update();
}

void churchOfRobotronVRApp::resize()
{
}

void churchOfRobotronVRApp::draw()
{
  WindowData* wd = getWindow()->getUserData<WindowData>();
  if (wd->mRenderType == WindowData::rtNormal)
  {
    // clear out the window with black
    gl::pushMatrices();
    gl::clear( Color( 0, 0, 0 ) );
//    if (mOculusVR)
    if (false)
    {
      mStereoCamera.enableStereoLeft();
      gl::setMatrices(mStereoCamera);
    } else {
      gl::setMatrices(mCamera.getCamera());
    }
    
    renderScene();
    
    gl::popMatrices();
    mParams.draw();
    gl::enableAlphaBlending();
    gl::drawString("FPS:" + cinder::toString(getAverageFps()), Vec2f(10,10));
    gl::disableAlphaBlending();
  } else {
    // clear out the window with black
    gl::clear( Color( 0, 0, 0 ) );
    
    {
      // Start Rendering to Our Side by Side RenderTarget
      gl::SaveFramebufferBinding bindingSaver;
      mOculusFbo.bindFramebuffer();
      
      // Clear
      gl::clear( ColorA( 0.0f, 0.0f, 0.0f, 1.0f ) );
      
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
    gl::enableAlphaBlending();
    gl::drawString("FPS:" + cinder::toString(getAverageFps()), Vec2f(10,10));
    gl::disableAlphaBlending();
  }
}

void churchOfRobotronVRApp::renderScene()
{
  mEnvironment.draw();
  mSermon.render();
  mRandoms.render();
  mScreen.draw();
  mLeaderboard.draw();
  mModel.draw();
  mEnforcer.draw();
  mEnvironment.drawLast();
}

void churchOfRobotronVRApp::checkWindows()
{
  auto ow = mOculusWindow.lock();
  if (ow)
    return;

  auto& displays = Display::getDisplays();
  for (auto i : displays)
  {
    if (i != Display::getMainDisplay())
    {
      if ((i->getWidth() == 1280) && (i->getHeight() == 800))
      {
        console() << "Create VR Window!" << std::endl;
        FullScreenOptions fo;
        fo.secondaryDisplayBlanking(false);
        
        Window::Format format;
        format.setFullScreen(true, fo);
        format.setDisplay(i);
        
        mOculusWindow = createWindow(format);
        auto new_ow = mOculusWindow.lock();
        WindowData* wd = new WindowData;
        wd->mRenderType = WindowData::rtOculus;
        new_ow->setUserData( wd );
        return;
      }
    }
  }
}

CINDER_APP_NATIVE( churchOfRobotronVRApp, RendererGl )
