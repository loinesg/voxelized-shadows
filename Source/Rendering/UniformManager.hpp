#pragma once

#include <QGLWidget>

#include "Matrix4x4.hpp"


// Uniform buffer for per-object data
struct PerObjectUniformBuffer
{
    static const int BlockID = 0;
    
    // Transformation matrices
    Matrix4x4 localToWorld;
    Matrix4x4 modelViewProjection;
};


class UniformManager
{
public:
    UniformManager();
    ~UniformManager();
    
    void updatePerObjectBuffer(const PerObjectUniformBuffer &buffer);
    
private:
    GLuint perObjectBlockID_;
    
    void createBuffers();
};
