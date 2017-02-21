#pragma once

#include <QGLWidget>

#include "Matrix4x4.hpp"
#include "Vector4.hpp"


// Uniform buffer for per-object data
struct PerObjectUniformBuffer
{
    static const int BlockID = 0;
    
    // Transformation matrices
    Matrix4x4 localToWorld;
    Matrix4x4 modelViewProjection;
};


// Uniform buffer for global scene data
struct SceneUniformBuffer
{
    static const int BlockID = 1;
    
    // Light settings
    Vector4 lightColor;
    Vector4 lightDirection; // To light. Normalized
};


class UniformManager
{
public:
    UniformManager();
    ~UniformManager();
    
    void updatePerObjectBuffer(const PerObjectUniformBuffer &buffer);
    void updateSceneBuffer(const SceneUniformBuffer &buffer);
    
private:
    GLuint perObjectBlockID_;
    GLuint sceneBlockID_;
    
    void createBuffers();
};
