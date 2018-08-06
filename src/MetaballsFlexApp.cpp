#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "ShaderHelper.hpp"
#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MetaballsFlexApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    void renderInitialBuffer();
    void computeDensityField();
private:
    CameraUi        mCameraController;
    CameraPersp     mCamera;
    vec3            mInitialCameraPos;
    
    std::vector<vec3>   mPointPositions;
    int                 mNumToRender;
    gl::VboRef          mPositionsVbo;
    
    //~ FBOs
    gl::FboRef          mGBufferFbo;
    gl::FboRef          mDensityValuesFbo;
    
    //~ shaders
    gl::GlslProgRef     mSimpleBillboardShader;
    gl::GlslProgRef     mMetaballsInitShader;
    gl::GlslProgRef     mDensityAccumShader;
    gl::GlslProgRef     mSsaoShader;
};

void MetaballsFlexApp::setup()
{
    // generate some random points
    mNumToRender = 100;
    for (int i = 0; i < mNumToRender; i++)
    {
        float randX = rand() % 10;
        float randY = rand() % 10;
        float randZ = rand() % 10;
        mPointPositions.push_back(vec3(randX, randY, randZ));
    }
    
    // prepare FBOs
    { // G-Buffer setup
        ivec2 sz = ivec2(getWindowWidth(), getWindowHeight());
        gl::Texture2dRef textureAttachments[4];
        //~ color buffer
        textureAttachments[0] = gl::Texture2d::create(sz.x, sz.y,
                                                      gl::Texture2d::Format().internalFormat(GL_RGB16F));
        //~ normal buffer
        textureAttachments[1] = gl::Texture2d::create(sz.x, sz.y,
                                                      gl::Texture2d::Format().internalFormat(GL_RGB16F));
        //~ view position buffer
        textureAttachments[2] = gl::Texture2d::create(sz.x, sz.y,
                                                      gl::Texture2d::Format().internalFormat(GL_RGB16F));
        //~ view position end buffer
        textureAttachments[3] = gl::Texture2d::create(sz.x, sz.y,
                                                      gl::Texture2d::Format().internalFormat(GL_RGB16F));
        gl::Fbo::Format fboFormat;
        fboFormat.attachment(GL_COLOR_ATTACHMENT0, textureAttachments[0]);
        fboFormat.attachment(GL_COLOR_ATTACHMENT1, textureAttachments[1]);
        fboFormat.attachment(GL_COLOR_ATTACHMENT2, textureAttachments[2]);
        fboFormat.attachment(GL_COLOR_ATTACHMENT3, textureAttachments[3]);
        mGBufferFbo = gl::Fbo::create(sz.x, sz.y, fboFormat);
    }
    { // density buffer setup
        ivec2 sz = ivec2(getWindowWidth(), getWindowHeight());
        gl::Texture2dRef textureAttachments[1];
        //~ density
        textureAttachments[0] = gl::Texture2d::create(sz.x, sz.y,
                                                      gl::Texture2d::Format().internalFormat(GL_RGB16F));
        
        gl::Fbo::Format fboFormat;
        fboFormat.attachment(GL_COLOR_ATTACHMENT0, textureAttachments[0]);
        mDensityValuesFbo = gl::Fbo::create(sz.x, sz.y, fboFormat);
    }
    
    // prepare buffers
    mPositionsVbo = gl::Vbo::create(GL_ARRAY_BUFFER, mPointPositions.size() * sizeof(vec3), mPointPositions.data(), GL_STATIC_DRAW);
    mPositionsVbo->bind();
    gl::enableVertexAttribArray(0);
    gl::vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    //~ Shaders setup
    mSimpleBillboardShader = gl::GlslProg::create(loadAsset("shaders/simple-billboards.vs"),
                                                  loadAsset("shaders/simple-billboards.fs"),
                                                  loadAsset("shaders/simple-billboards.gs"));
    
    mMetaballsInitShader = gl::GlslProg::create(loadAsset("shaders/metaballs-init.vs"),
                                                  loadAsset("shaders/metaballs-init.fs"),
                                                  loadAsset("shaders/metaballs-init.gs"));
    
    mDensityAccumShader = gl::GlslProg::create(loadAsset("shaders/density-accum.vs"),
                                                loadAsset("shaders/density-accum.fs"),
                                                loadAsset("shaders/density-accum.gs"));
    
    mSsaoShader = gl::GlslProg::create(loadAsset("shaders/ssao.vs"),
                                       loadAsset("shaders/ssao.fs"));
    
    // setup basic cameraui for now
    mInitialCameraPos = vec3(10, 0, 0);
    mCamera.lookAt(mInitialCameraPos, vec3(0,0,0));
    mCameraController = CameraUi(&mCamera, getWindow(), -1);
}

void MetaballsFlexApp::mouseDown( MouseEvent event )
{
}

void MetaballsFlexApp::update()
{
    
}

void MetaballsFlexApp::renderInitialBuffer()
{
    {
        mGBufferFbo->bindFramebuffer();
        gl::clear( Color( 0.90, 0.90, 0.95 ) );
        mPositionsVbo->bind();
        gl::ScopedGlslProg billboard(mMetaballsInitShader);
        mMetaballsInitShader->uniform("Scale", 1.0f);
        mMetaballsInitShader->uniform("modelViewMatrix", mCamera.getViewMatrix());
        mMetaballsInitShader->uniform("projectionMatrix", mCamera.getProjectionMatrix());
        gl::drawArrays(GL_POINTS, 0, mNumToRender);
        mGBufferFbo->unbindFramebuffer();
    }
}

void MetaballsFlexApp::computeDensityField()
{
    {
        mDensityValuesFbo->bindFramebuffer();
        gl::clear( Color( 0.90, 0.90, 0.95 ) );
        mPositionsVbo->bind();
        gl::ScopedGlslProg billboard(mDensityAccumShader);
        mDensityAccumShader->uniform("Scale", 1.0f);
        mDensityAccumShader->uniform("modelViewMatrix", mCamera.getViewMatrix());
        mDensityAccumShader->uniform("projectionMatrix", mCamera.getProjectionMatrix());
        mDensityAccumShader->uniform("screenWidth", getWindowWidth());
        mDensityAccumShader->uniform("screenHeight", getWindowHeight());
        
        gl::ScopedTextureBind scopedTexBind(mGBufferFbo->getTexture2d(GL_COLOR_ATTACHMENT2), 0);
        mDensityAccumShader->uniform("lambdaTexture", 0);
        
        gl::drawArrays(GL_POINTS, 0, mNumToRender);
        mDensityValuesFbo->unbindFramebuffer();
    }
}

void MetaballsFlexApp::draw()
{
    renderInitialBuffer();
    computeDensityField();
    
//    auto text = mGBufferFbo->getTexture2d(GL_COLOR_ATTACHMENT2);
    auto text = mDensityValuesFbo->getTexture2d(GL_COLOR_ATTACHMENT0);
    gl::draw(text);
}

CINDER_APP( MetaballsFlexApp, RendererGl )
