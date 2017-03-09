#pragma once

#define GL_GLEXT_PROTOTYPES 1 // Enables OpenGL 3 Features
#include <QGLWidget> // Links OpenGL Headers

#include "Scene.hpp"
#include "RenderPass.hpp"
#include "ShadowMap.hpp"
#include "ShadowMask.hpp"
#include "UniformManager.hpp"
#include "Overlay.hpp"

class RendererWidget : public QGLWidget
{
public:
    RendererWidget(const QGLFormat &format);
    ~RendererWidget();
    
    Scene* scene() { return scene_; }
    UniformManager* uniformManager() { return uniformManager_; }

    // Main scene camera
    Camera* camera() const { return scene_->mainCamera(); }
    
    // Shader feature toggling
    void enableFeature(ShaderFeature feature);
    void disableFeature(ShaderFeature feature);
    
    // Changes the overlay index. -1 means no overlay.
    void setOverlay(int overlayIndex);
    
    // Changes the shadow map settings.
    void setShadowMapResolution(int resolution);
    void setShadowMapCascades(int cascades);
    
private:
    Scene* scene_;
    UniformManager* uniformManager_;
    
    GLuint sceneDepthFBO_;
    Texture* sceneDepthTexture_;
    ShadowMap* shadowMap_;
    ShadowMask* shadowMask_;
    
    RenderPass* shadowCasterPass_;
    RenderPass* sceneDepthPass_;
    RenderPass* shadowMaskPass_;
    RenderPass* forwardPass_;
    
    Mesh* fullScreenQuad_;
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
    
    // Applies camera properties to the opengl state
    void useCamera(Camera* camera);
};
