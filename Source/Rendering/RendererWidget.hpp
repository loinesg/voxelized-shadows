#pragma once

#define GL_GLEXT_PROTOTYPES 1 // Enables OpenGL 3 Features
#include <QGLWidget> // Links OpenGL Headers

#include "Scene.hpp"
#include "RenderPass.hpp"
#include "ShadowMap.hpp"
#include "ShadowMask.hpp"
#include "UniformManager.hpp"

class RendererWidget : public QGLWidget
{
public:
    RendererWidget(const QGLFormat &format);
    ~RendererWidget();
    
    Scene* scene() { return scene_; }
    UniformManager* uniformManager() { return uniformManager_; }

    // Main scene camera
    Camera* camera() const { return scene_->mainCamera(); }
    
    // Renderer textures
    Texture* shadowMap() { return shadowMap_->texture(); }
    Texture* sceneDepthTexture() { return sceneDepthTexture_; }
    Texture* shadowMask() { return shadowMask_->texture(); }
    
    // Shader feature toggling
    void enableFeature(ShaderFeature feature);
    void disableFeature(ShaderFeature feature);
    
    // Changes the overlay texture. Setting to NULL = no overlay.
    void setOverlayTexture(Texture* overlay);
    
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
    Shader* overlayShader_;
    Texture* overlayTexture_;

    // QGLWidget override methods
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    
    // Setup
    void createRenderPasses();
    void createScene();
    
    // Render passes
    void renderShadowMap();
    void renderSceneDepth();
    void renderShadowMask();
    void renderForward();
    
    // Applies camera properties to the opengl state
    void useCamera(Camera* camera);
    
    // Draws the overlay texture in the corner of the screen
    void drawOverlayTexture();
};
