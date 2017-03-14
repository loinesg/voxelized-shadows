#pragma once

#define GL_GLEXT_PROTOTYPES 1 // Enables OpenGL 3 Features
#include <QGLWidget> // Links OpenGL Headers

#include "Scene.hpp"
#include "RenderPass.hpp"
#include "ShadowMap.hpp"
#include "ShadowMask.hpp"
#include "UniformManager.hpp"
#include "Overlay.hpp"
#include "VoxelTree.hpp"

class RendererWidget : public QGLWidget
{
public:
    RendererWidget(const QGLFormat &format);
    ~RendererWidget();
    
    Scene* scene() { return scene_; }
    UniformManager* uniformManager() { return uniformManager_; }

    // Main scene camera
    Camera* camera() const { return scene_->mainCamera(); }
    
    // Gets the current shadow method
    ShadowMaskMethod shadowMethod() const { return shadowMask_->method(); }
    
    // Shader feature toggling
    void enableFeature(ShaderFeature feature);
    void disableFeature(ShaderFeature feature);
    
    // Changes the overlay index. -1 means no overlay.
    void setOverlay(int overlayIndex);
    
    // Changes shadow rendering settings.
    void setShadowRenderMethod(ShadowMaskMethod method);
    void setShadowMapResolution(int resolution);
    void setShadowMapCascades(int cascades);
    
private:
    Scene* scene_;
    UniformManager* uniformManager_;
    
    GLuint sceneDepthFBO_;
    Texture* sceneDepthTexture_;
    ShadowMap* shadowMap_;
    VoxelTree* voxelTree_;
    ShadowMask* shadowMask_;
    
    RenderPass* sceneDepthPass_;
    RenderPass* forwardPass_;
    
    vector<Overlay*> overlays_;
    int currentOverlay_;

    // QGLWidget override methods
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    
    // Setup
    void createRenderPasses();
    void createScene();
    void createOverlays();
    
    // Render passes
    void renderShadowMap();
    void renderSceneDepth();
    void renderShadowMask();
    void renderForward();
};
