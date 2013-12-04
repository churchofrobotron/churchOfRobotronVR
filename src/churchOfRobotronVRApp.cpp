#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Arcball.h"
#include "cinder/Camera.h"

#include "FPSCamUI.h"

#include "robotronScreen.h"

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
  RobotronScreen mScreen;
  FpsCamUI mCamera;
};

void churchOfRobotronVRApp::setup()
{
  mScreen.init();
  CameraPersp cam;
  cam.lookAt(Vec3f::zero(), Vec3f::yAxis(), Vec3f::zAxis());
  mCamera.setCurrentCam(cam);
  mCamera.registerEvents();
  mCamera.setSpeed(0.001);
}

void churchOfRobotronVRApp::mouseDown(MouseEvent event)
{
}

void churchOfRobotronVRApp::mouseDrag(MouseEvent event)
{
}

void churchOfRobotronVRApp::update()
{
  mCamera.update();
}

void churchOfRobotronVRApp::resize()
{
}

void churchOfRobotronVRApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatrices(mCamera.getCamera());
  
  mScreen.draw();
  gl::popMatrices();
}

CINDER_APP_NATIVE( churchOfRobotronVRApp, RendererGl )
