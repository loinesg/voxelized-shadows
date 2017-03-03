#pragma once

#include <QGLWidget>

#include "Camera.hpp"
#include "Light.hpp"
#include "Texture.hpp"
#include "UniformManager.hpp"

class ShadowMap
{
public:
    ShadowMap(UniformManager* uniformManager, int width, int height);
    ~ShadowMap();
    
    Texture* texture() { return shadowDepthTexture_; }
    Camera* camera () { return &shadowCamera_; }
    int width() const { return shadowDepthTexture_->width(); }
    int height() const { return shadowDepthTexture_->height(); }
    
    // Sets the shadow map resolution
    void setResolution(int resolution);
    
    // Fits the shadow map to the current view frustum
    void updatePosition(Camera* viewCamera, Light* light);
    
    // Updates the shadows uniform buffer
    void updateUniformBuffer(Camera* cam) const;
    
    // Binds the shadow map texture.
    void bindTexture(GLenum textureSlot) const;
    
private:
    UniformManager* uniformManager_;
    Camera shadowCamera_;
    Texture* shadowDepthTexture_;
    
    // Computes the world to shadow transformation matrix
    Matrix4x4 worldToShadowMatrix() const;
};
