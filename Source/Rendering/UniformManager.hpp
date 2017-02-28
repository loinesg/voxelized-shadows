#pragma once

#include <QGLWidget>

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
    Vector4 ambientLightColor;
    Vector4 lightColor;
    Vector4 lightDirection; // To light. Normalized
};


// Uniform buffer for shadow data
struct ShadowUniformBuffer
{
    static const int BlockID = 2;
    
    Matrix4x4 worldToShadow;
    Vector4 shadowTexelSize; // w, h, 1/w, 1/h
};


class UniformManager
{
public:
    UniformManager();
    ~UniformManager();
    
    void updatePerObjectBuffer(const PerObjectUniformBuffer &buffer);
    void updateSceneBuffer(const SceneUniformBuffer &buffer);
    void updateShadowBuffer(const ShadowUniformBuffer &buffer);
    
private:
    GLuint perObjectBlockID_;
    GLuint sceneBlockID_;
    GLuint shadowBlockID_;
    
    void createBuffers();
};
