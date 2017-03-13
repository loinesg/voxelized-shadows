#pragma once

#define GL_GLEXT_PROTOTYPES 1 // Enables OpenGL 3 Features
#include <QGLWidget> // Links OpenGL Headers

#include "Scene.hpp"
#include "Light.hpp"
#include "MeshInstance.hpp"
#include "Bounds.hpp"
#include "ShadowMap.hpp"
#include "UniformManager.hpp"
#include "VoxelBuilder.hpp"

class VoxelTree
{
public:
    VoxelTree(UniformManager* uniformManager, const Scene* scene, int resolution);
    ~VoxelTree();
    
    // The voxels buffer texture id
    GLuint treeBufferTexture() const { return bufferTexture_; }
    
    // Builds the tree at the specified resolution.
    void build();
    
private:
    UniformManager* uniformManager_;
    const Scene* scene_;
    
    int resolution_;
    Matrix4x4 worldToShadow_;
    
    // The voxel buffer and containing buffer texture.
    GLuint buffer_;
    GLuint bufferTexture_;
    
    // Computes bounds of the entire scene in light space.
    Bounds sceneBoundsLightSpace() const;
    
    // Renders dual shadow maps for the scene.
    VoxelDepthMap computeDualShadowMaps();
};
