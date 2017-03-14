#pragma once

#define GL_GLEXT_PROTOTYPES 1 // Enables OpenGL 3 Features
#include <QGLWidget> // Links OpenGL Headers

#include "Scene.hpp"
#include "RenderPass.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Texture.hpp"
#include "UniformManager.hpp"
#include "Bounds.hpp"

struct ShadowCascade
{
    // Cascade distance
    float minDistance;
    float maxDistance;
    
    // Camera used to render the cascade
    Camera camera;
};

class ShadowMap
{
    // A 4 cascade limit allows distances to fit in a single vec4
    // and component-wise distance comparisons in the shader.
    static const int MaxCascades = 4;
    
public:
    ShadowMap(const Scene* scene, UniformManager* uniformManager, int cascadesCount, int resolution);
    ~ShadowMap();
    
    // The shadow map depth texture.
    // Contains an atlas of all shadow cascades
    Texture* texture() { return texture_; }
    
    // Cascade settings
    int resolution() const { return resolution_; }
    int cascadesCount() const { return cascadesCount_; }
    
    // Computes the world to shadow transformation matrix for the given cascade
    Matrix4x4 worldToShadowMatrix(int cascade) const;
    
    // Sets the resolution of each cascade
    void setCascades(int cascadesCount, int resolution);
    
    // Moves the shadow map to fit the current view
    void updatePosition(Camera* viewCamera);
    
    // Directly sets the shadow map bounds in light space.
    void setLightSpaceBounds(Bounds lightSpaceBounds);
    
    // Updates the shadows uniform buffer
    void updateUniformBuffer() const;
    
    // Rerenders all shadow map cascades
    void renderCascades();
    
private:
    const Scene* scene_;
    UniformManager* uniformManager_;
    Texture* texture_;
    GLuint framebuffer_;
    ShadowCascade cascades_[MaxCascades];
    
    // Render pass for shadow cascades
    RenderPass* shadowCasterPass_;
    
    int resolution_;
    int cascadesCount_;
    
    // Computes the start distance of a shadow cascade
    float getCascadeMin(int cascade, float farPlane) const;
    
    // Computes the end distance of a shadow cascade
    float getCascadeMax(int cascade, float farPlane) const;
};
