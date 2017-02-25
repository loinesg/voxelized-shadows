#include "RendererWidget.hpp"

#include <assert.h>
#include <cstdio>

#include <iostream>

RendererWidget::RendererWidget(const QGLFormat &format)
    : QGLWidget(format)
{
    
}

RendererWidget::~RendererWidget()
{
    delete scene_;
    delete forwardPass_;
    delete uniformManager_;
}

void RendererWidget::enableFeature(ShaderFeature feature)
{
    forwardPass_->enableFeature(feature);
}

void RendererWidget::disableFeature(ShaderFeature feature)
{
    forwardPass_->disableFeature(feature);
}

void RendererWidget::initializeGL()
{
    printf("Initializing OpenGL %s \n", glGetString(GL_VERSION));

    // Configure OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    // Create managers
    uniformManager_ = new UniformManager();
    
    // Create required assets
    createRenderPasses();
    createScene();
}

void RendererWidget::resizeGL(int w, int h)
{
    // Update the main camera to match the new resolution
    camera()->setPixelWidth(w);
    camera()->setPixelHeight(h);
}

void RendererWidget::paintGL()
{
    renderForward();
    
    // Redraw immediately
    update();
}

void RendererWidget::createRenderPasses()
{
    string shaderDirectory = "voxelized-shadows.app/Contents/Resources/Shaders/";
    string forwardPassName = "ForwardPass";
    
    // Pass for rendering the final image.
    // Uses all features.
    forwardPass_ = new RenderPass(forwardPassName, shaderDirectory, uniformManager_);
    forwardPass_->setSupportedFeatures(~0);
    forwardPass_->setClearColor(PassClearColor(0.6, 0.1, 0.1, 1.0));
}

void RendererWidget::createScene()
{
    scene_ = new Scene();
    scene_->loadFromFile("voxelized-shadows.app/Contents/Resources/Scenes/scene.scene");
}

void RendererWidget::renderForward()
{
    // Update scene uniform buffer
    SceneUniformBuffer data;
    data.cameraPosition = Vector4(scene_->mainCamera()->position(), 1.0);
    data.ambientLightColor = Vector4(scene_->mainLight()->ambient(), 1.0);
    data.lightColor = Vector4(scene_->mainLight()->color(), 1.0);
    data.lightDirection = -1.0 * scene_->mainLight()->forward();
    uniformManager_->updateSceneBuffer(data);
    
    // Render the final image
    useCamera(scene_->mainCamera());
    forwardPass_->submit(scene_->mainCamera(), scene_->meshInstances());
}

void RendererWidget::useCamera(Camera* camera)
{
    // Bind the correct framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, camera->framebuffer());
    
    // Update the viewport to match the camera width/height
    glViewport(0, 0, camera->pixelWidth(), camera->pixelHeight());
}
