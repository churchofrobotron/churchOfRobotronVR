#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "robotronScreen.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class churchOfRobotronVRApp : public AppNative {
public:
	void setup();
	void mouseDown( MouseEvent event );
	void update();
	void draw();
private:
  RobotronScreen mScreen;
};

void churchOfRobotronVRApp::setup()
{
  mScreen.init();
}

void churchOfRobotronVRApp::mouseDown( MouseEvent event )
{
}

void churchOfRobotronVRApp::update()
{
}

void churchOfRobotronVRApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );

  mScreen.draw();
}

CINDER_APP_NATIVE( churchOfRobotronVRApp, RendererGl )
