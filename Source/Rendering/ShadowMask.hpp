#pragma once

#include <QGLWidget>

#include "Mesh.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Texture.hpp"
#include "UniformManager.hpp"

class ShadowMask
{
public:
    ShadowMask();
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
    
    // Binds the shadow mask texture.
    void bindTexture(GLenum textureSlot) const;
    
private:
    GLuint frameBuffer_;
    Texture* texture_;
};
