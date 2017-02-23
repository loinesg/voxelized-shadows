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

void RendererWidget::initializeGL()
{
    printf("Initializing OpenGL %s \n", glGetString(GL_VERSION));

    // Configure OpenGL state
    glEnable(GL_DEPTH_TEST);
    
    // Create managers
    uniformManager_ = new UniformManager();
    
    // Create required assets
    createRenderPasses();
    createScene();
}

void RendererWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    
    camera()->setPixelWidth(w);
    camera()->setPixelHeight(h);
}

void RendererWidget::paintGL()
{
    SceneUniformBuffer data;
    data.cameraPosition = Vector4(scene_->mainCamera()->position(), 1.0);
    data.ambientLightColor = Vector4(scene_->mainLight()->ambient(), 1.0);
    data.lightColor = Vector4(scene_->mainLight()->color(), 1.0);
    data.lightDirection = -1.0 * scene_->mainLight()->forward();
    uniformManager_->updateSceneBuffer(data);
    
    forwardPass_->submit(scene_->mainCamera(), scene_->meshInstances());
    
    // Redraw immediately
    update();
}

void RendererWidget::createRenderPasses()
{
    string shaderDirectory = "voxelized-shadows.app/Contents/Resources/Shaders/";
    string forwardPassName = "ForwardPass";
    
    forwardPass_ = new RenderPass(forwardPassName, shaderDirectory, uniformManager_);
    forwardPass_->setSupportedFeatures(SF_Texture | SF_Cutout | SF_NormalMap);
    forwardPass_->setClearColor(PassClearColor(0.6, 0.1, 0.1, 1.0));
}

void RendererWidget::createScene()
{
    scene_ = new Scene();
    scene_->loadFromFile("voxelized-shadows.app/Contents/Resources/Scenes/scene.scene");
}
