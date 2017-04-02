#pragma once

#define GL_GLEXT_PROTOTYPES 1 // Enables OpenGL 3 Features
#include <QGLWidget> // Links OpenGL Headers

#include "RenderPass.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"
#include "Texture.hpp"
#include "VoxelTree.hpp"

// The method to use for shadowing
enum ShadowMaskMethod
{
    // Traditional shadow mapping
    SMM_ShadowMap,
  
    // Sampling the voxelized shadow tree
    SMM_VoxelTree,
    
    // Shadow map for dynamics + voxel tree for statics
    SMM_Combined
};

class ShadowMask
{
public:
    ShadowMask(UniformManager* uniformManager, ShadowMaskMethod method);
    ~ShadowMask();
    
    // The currently active method
    ShadowMaskMethod method() const { return method_; }
    
    // The shadow mask framebuffer
    GLuint frameBuffer() const { return frameBuffer_; }
    
    // The shadow mask texture
    Texture* texture() { return texture_; }

    // Shadow mask resolution.
    // Should match screen resolution.
    int width() const { return texture_->width(); }
    int height() const { return texture_->height(); }
    
    // Changes the method being used
    void setMethod(ShadowMaskMethod method);
    
    // Changes the shadow mask resolution to match the screen.
    void setResolution(int width, int height);
    
    // Sets input tetxures
    void setShadowMapTexture(Texture* shadowMapTexture);
    void setSceneDepthTexture(Texture* sceneDepthTexture);
    
    // Sets the input voxel tree
    void setVoxelTree(const VoxelTree* voxelTree);
    
    // Renders the shadow mask
    void render();
    
private:
    ShadowMaskMethod method_;
    
    // Framebuffer and shadow mask texture
    GLuint frameBuffer_;
    Texture* texture_;
    
    // Render passes
    RenderPass* shadowMapPass_;
    RenderPass* voxelTreePass_;
    
    // Input texture
    Texture* shadowMapTexture_;
    Texture* sceneDepthTexture_;
    
    // Input voxelised shadow tree
    const VoxelTree* voxelTree_;
};
