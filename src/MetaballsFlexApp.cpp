#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MetaballsFlexApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void MetaballsFlexApp::setup()
{
}

void MetaballsFlexApp::mouseDown( MouseEvent event )
{
}

void MetaballsFlexApp::update()
{
}

void MetaballsFlexApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( MetaballsFlexApp, RendererGl )
