#pragma once

#define GL_GLEXT_PROTOTYPES 1 // Enables OpenGL 3 Features
#include <QGLWidget> // Links OpenGL Headers

#include "Camera.hpp"
#include "Light.hpp"
#include "Texture.hpp"
#include "UniformManager.hpp"

// Information on a single cascade
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
    ShadowMap(UniformManager* uniformManager, int resolution, int cascadesCount);
    ~ShadowMap();
    
    // The shadow map depth texture.
    // Contains an atlas of all shadow cascades
    Texture* texture() { return texture_; }
    
    // Cascade settings
    int resolution() const { return resolution_; }
    int cascadesCount() const { return cascadesCount_; }
    
    // Returns the camera for the given cascade index.
    Camera* getCamera(int cascade);
    
    // Sets the resolution of each cascade
    void setResolution(int resolution);
    
    // Sets the number of cascades.
    // This may involve resizing the shadow depth texture.
    void setCascadesCount(int cascades);
    
    // Moves the shadow map to fit the current view and light direction
    void updatePosition(Camera* viewCamera, Light* light);
    
    // Updates the shadows uniform buffer
    void updateUniformBuffer() const;
    
    // Binds the shadow map texture.
    void bindTexture(GLenum textureSlot) const;
    
private:
    UniformManager* uniformManager_;
    Texture* texture_;
    GLuint framebuffer_;
    ShadowCascade cascades_[MaxCascades];
    
    int resolution_;
    int cascadesCount_;
    
    // Sets up the texture resolution and cascade configuration
    void setup();
    
    // Computes the start distance of a shadow cascade
    float getCascadeStart(float maxDistance, int cascade) const;
    
    // Computes the world to shadow transformation matrix for the given cascade
    Matrix4x4 worldToShadowMatrix(int cascade) const;
};
