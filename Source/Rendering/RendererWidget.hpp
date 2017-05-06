#pragma once

#define GL_GLEXT_PROTOTYPES 1 // Enables OpenGL 3 Features
#include <QGLWidget> // Links OpenGL Headers

#include "RendererStats.hpp"
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
    RendererWidget(const QGLFormat &format, int voxelResolution);
    ~RendererWidget();
    
    Scene* scene() { return scene_; }
    UniformManager* uniformManager() { return uniformManager_; }

    // Current rendering info
    int resolutionX() const { return shadowMask_->texture()->width(); }
    int resolutionY() const { return shadowMask_->texture()->height(); }
    const RendererStats* stats() const { return stats_; }
    
    // Main scene camera
    Camera* camera() const { return scene_->mainCamera(); }
    
    // Gets the current shadow method
    ShadowMaskMethod shadowMethod() const { return shadowMask_->method(); }
    
    // The current rendering components
    const ShadowMap* shadowMap() const { return shadowMap_; }
    const VoxelTree* voxelTree() const { return voxelTree_; }
    
    // Shader feature toggling
    void enableFeature(ShaderFeature feature);
    void disableFeature(ShaderFeature feature);
    
    // Changes the overlay index. -1 means no overlay.
    void setOverlay(int overlayIndex);
    
    // Changes shadow rendering settings.
    void setShadowRenderMethod(ShadowMaskMethod method);
    void setShadowMapResolution(int resolution);
    void setShadowMapCascades(int cascades);
    void setVoxelPCFFilterSize(int kernelSize);
    
    // Forces the voxel tree to be completely built before
    // starting to render the scene. Used for profiling.
    void precomputeTree();
    
private:
    Scene* scene_;
    UniformManager* uniformManager_;
    
    // Logs rendering stats for display in the UI
    RendererStats* stats_;
    
    GLuint sceneDepthFBO_;
    Texture* sceneDepthTexture_;
    ShadowMap* shadowMap_;
    VoxelTree* voxelTree_;
    ShadowMask* shadowMask_;
    
    RenderPass* sceneDepthPass_;
    RenderPass* forwardPass_;
    
    vector<Overlay*> overlays_;
    int currentOverlay_;
    
    int voxelResolution_;

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
