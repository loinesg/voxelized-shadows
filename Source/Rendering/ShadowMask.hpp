#pragma once

#define GL_GLEXT_PROTOTYPES 1 // Enables OpenGL 3 Features
#include <QGLWidget> // Links OpenGL Headers

#include "RenderPass.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"
#include "Texture.hpp"

class ShadowMask
{
public:
    ShadowMask(UniformManager* uniformManager);
    ~ShadowMask();
    
    // The shadow mask framebuffer
    GLuint frameBuffer() const { return frameBuffer_; }
    
    // The shadow mask texture
    Texture* texture() { return texture_; }

    // Shadow mask resolution.
    // Should match screen resolution.
    int width() const { return texture_->width(); }
    int height() const { return texture_->height(); }
    
    // Changes the shadow mask resolution to match the screen.
    void setResolution(int width, int height);
    
    // Sets input tetxures
    void setShadowMapTexture(Texture* shadowMapTexture);
    void setSceneDepthTexture(Texture* sceneDepthTexture);
    
    // Renders the shadow mask
    void render();
    
private:
    // Framebuffer and shadow mask texture
    GLuint frameBuffer_;
    Texture* texture_;
    
    // Render pass
    RenderPass* shadowMaskPass_;
    
    // Input texture
    Texture* shadowMapTexture_;
    Texture* sceneDepthTexture_;
};
