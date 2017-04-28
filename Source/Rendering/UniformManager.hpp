#pragma once

#define GL_GLEXT_PROTOTYPES 1 // Enables OpenGL 3 Features
#include <QGLWidget> // Links OpenGL Headers

#include "Matrix4x4.hpp"
#include "Vector4.hpp"


// Uniform buffer for per-object data
struct PerObjectUniformBuffer
{
    static const int BlockID = 0;
    
    Matrix4x4 localToWorld;
    Matrix4x4 modelViewProjection;
};


// Uniform buffer for global scene data
struct SceneUniformBuffer
{
    static const int BlockID = 1;
    
    Vector4 screenResolution;
    Vector4 cameraPosition;
    Matrix4x4 clipToWorld;
    Vector4 ambientLightColor;
    Vector4 lightColor;
    Vector4 lightDirection; // To light. Normalized
};


// Uniform buffer for shadow data
struct ShadowUniformBuffer
{
    static const int BlockID = 2;
    
    float cascadeDistancesSqr[4];
    Matrix4x4 worldToShadow[4];
};

// Uniform buffer for voxelized shadows
struct VoxelsUniformBuffer
{
    static const int BlockID = 3;
    
    Matrix4x4 worldToVoxels;
    
    uint32_t voxelTreeHeight;
    uint32_t tileSubdivisions;
    
    // The total number of voxels used in the PCF kernel
    uint32_t pcfSampleCount;
    
    // The number of leaf nodes visited for each PCF kernel.
    uint32_t pcfLookups;
    
    struct PCFOffset
    {
        uint32_t xOffset;
        uint32_t yOffset;
        uint32_t bitmaskHigh;
        uint32_t bitmaskLow;
    };
    
    // The precomputed information for each PCF lookup
    // 9 values per leaf index, as 17x17 PCF uses 9 lookups.
    PCFOffset pcfOffsets[64*9];
    
    // 3 unused ints are placed between each address as
    // padding, so each value is 16 bytes (GLSL limitation)
    int rootAddresses[16 * 16 * 4];
};

class UniformManager
{
public:
    UniformManager();
    ~UniformManager();
    
    void updatePerObjectBuffer(const PerObjectUniformBuffer &buffer);
    void updateSceneBuffer(const SceneUniformBuffer &buffer);
    void updateShadowBuffer(const ShadowUniformBuffer &buffer);
    void updateVoxelBuffer(const void* data, int sizeBytes);
    
private:
    GLuint perObjectBlockID_;
    GLuint sceneBlockID_;
    GLuint shadowBlockID_;
    GLuint voxelBlockID_;
    
    void createBuffers();
};
