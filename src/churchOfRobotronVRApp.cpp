#include "cinder/app/AppNative.h"
#include "cinder/Camera.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/params/Params.h"
#include "cinder/Utilities.h"

#include "boost/asio.hpp"

#include "FPSCamUI.h"
#include "OculusVR.h"
#include "CameraStereoHMD.h"

#include "robotronScreen.h"
#include "MovieObject.h"
#include "leaderboard.h"
#include "environment.h"
#include "MovieCubes.h"

#include "PixelModelDirector.h"

using namespace ci;
using namespace ci::app;
using namespace std;

bool hasOculus = false;

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

//const std::string sermonBase = "/Users/chaz/dev/churchOfRobotronVR/resources/videos/";
const std::string sermonBase = "/Users/bzztbomb/projects/churchOfRobotron/videos/";
const std::string downloads = sermonBase;

const short multicast_port = 2085;

class receiver
{
public:
  receiver(boost::asio::io_service& io_service,
           const boost::asio::ip::address& listen_address,
           const boost::asio::ip::address& multicast_address)
  : socket_(io_service)
  {
    // Create the socket so that multiple may be bound to the same address.
    boost::asio::ip::udp::endpoint listen_endpoint(
                                                   listen_address, multicast_port);
    socket_.open(listen_endpoint.protocol());
    socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));

    // UNNEEDED BIND?
    //    socket_.bind(listen_endpoint);
    socket_.bind(boost::asio::ip::udp::endpoint( multicast_address, multicast_port ));
    
    // Join the multicast group. DON'T ACTUALLY DO THIS
//    socket_.set_option(
//                       boost::asio::ip::multicast::join_group(multicast_address));
    
    socket_.async_receive_from(
                               boost::asio::buffer(data_, max_length), sender_endpoint_,
                               boost::bind(&receiver::handle_receive_from, this,
                                           boost::asio::placeholders::error,
                                           boost::asio::placeholders::bytes_transferred));
  }
  
  void handle_receive_from(const boost::system::error_code& error,
                           size_t bytes_recvd)
  {
    if (!error)
    {
//      std::cout.write(data_, bytes_recvd);
//      std::cout << std::endl;
      
      std::string val(data_, bytes_recvd);
      if (val == "PlayerDeath")
      {
        mPlayerDeath = true;
      }
      
      socket_.async_receive_from(
                                 boost::asio::buffer(data_, max_length), sender_endpoint_,
                                 boost::bind(&receiver::handle_receive_from, this,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));
    }
  }
  
  bool getPlayerDeath()
  {
    bool ret = mPlayerDeath;
    mPlayerDeath = false;
    return ret;
  }
  
private:
  boost::asio::ip::udp::socket socket_;
  boost::asio::ip::udp::endpoint sender_endpoint_;
  enum { max_length = 1024 };
  char data_[max_length];
  
  bool mPlayerDeath;
};

class churchOfRobotronVRApp : public AppNative {
public:
  churchOfRobotronVRApp();
  
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
  MovieObject mSermon;
  Leaderboard mLeaderboard;
  MovieObject mRandoms;
	
  //Grunt mModel;
  //Enforcer mEnforcer;
  PixelModelDirector& mPixelModelDirector = PixelModelDirector::getInstance();
  
  float mEyeSeparation = 0;
  
  float mDeathFlash = 0;
  
  boost::asio::io_service io_service;
  std::shared_ptr<receiver> mReceiver;
  
  void oculusInit();
  void renderScene();
  void checkWindows();
  void initSocket();
};

churchOfRobotronVRApp::churchOfRobotronVRApp()
: mDeathFlash(1.0f)
{
  try {
    mReceiver = std::make_shared<receiver>(io_service,
                                           boost::asio::ip::address::from_string("10.0.150.107"),
                                           boost::asio::ip::address::from_string("10.0.150.255"));
  } catch (...) {
    console() << "Unable to listen for death!" << endl;
  }
}

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
  mEyeSeparation = 0.01f;
  
  mParams.addParam("Eye Separation", &mEyeSeparation);
  
  mStereoCamera.setEyeSeparation( mEyeSeparation );
  mEnvironment.init(&mParams);
  mScreen.init(&mParams);
  mLeaderboard.init(&mParams);
  
  CameraPersp cam;
  cam.lookAt(Vec3f::zero(), Vec3f::yAxis(), Vec3f::zAxis());
  cam.setFov(90);
  
  mCamera.setCurrentCam(cam);
  mCamera.registerEvents();
  mCamera.setSpeed(0.01);
  
  std::vector<string> sermons =
  {
    sermonBase + "church_of_robotron_sermon-__doctrine_of_error_640x472.mp4",
    sermonBase + "church_of_robotron_sermon-_doctrine_of_futility_640x472.mp4",
    sermonBase + "church_of_robotron_sermon-_eight_ways_640x472.mp4",
    sermonBase + "church_of_robotron_sermon-_the_ninth_position_640x472.mp4",
    sermonBase + "church_of_robotron_sermon-_what_are_the_robotrons_640x472.mp4"
  };
  mSermon.setMovieList(sermons);
  mSermon.setMute(true);
  mSermon.setPrefix("sermons");
  mSermon.init(&mParams);
  mSermon.setPosition(Vec3f(0.01, 7.74f, 0.47));
  mSermon.setScale(Vec2f(2.22, 2.84));
  
  std::vector<string> randoms =
  {
    downloads + "Glitch-logo-02.mp4",
    downloads + "Glitch-text-sprites-05.mp4",
    sermonBase + "116645203.mp4",
    downloads + "RobotronTextVideo.mov",
  };
  
  mRandoms.setMovieList(randoms);
  mRandoms.setMute(true);
  mRandoms.init(&mParams);
  
  //mModel.init(&mParams);
  //mEnforcer.init(&mParams);
	mPixelModelDirector.init(&mParams);
  
  getWindow()->setUserData( new WindowData );
  
}

void churchOfRobotronVRApp::oculusInit()
{  
  // Init OVR
  mOculusVR           = ovr::Device::create();
  if (mOculusVR)
    hasOculus = true;
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
  mPixelModelDirector.update();
	
  mStereoCamera.setEyeSeparation( mEyeSeparation );
  
  io_service.poll();
  
  if (mReceiver && mReceiver->getPlayerDeath())
  {
    mDeathFlash = 1.0f;
  }
  if (mDeathFlash > 0.05)
  {
    mDeathFlash *= 0.96;
  } else {
    mDeathFlash = 0.0f;
  }
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
    
    gl::setMatricesWindow( getWindowSize() );
    gl::setViewport( getWindowBounds() );
    gl::disableDepthRead();
    gl::disableDepthWrite();
    
    if (mDeathFlash)
    {
      gl::enableAlphaBlending();
      gl::color(ColorA(1.0f, 0.0f, 0.0f, mDeathFlash));
      gl::drawSolidRect(Rectf(0, 0, getWindowWidth() * 100, getWindowHeight() * 100));
      gl::disableAlphaBlending();
    }
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

    if (mDeathFlash)
    {
      gl::enableAlphaBlending();
      gl::color(ColorA(1.0f, 0.0f, 0.0f, mDeathFlash));
      gl::drawSolidRect(Rectf(0, 0, getWindowWidth() * 100, getWindowHeight() * 100));
      gl::disableAlphaBlending();
    }

    
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
  //mModel.draw();
  //mEnforcer.draw();
	mPixelModelDirector.draw();
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
