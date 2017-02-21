#pragma once

#include <QGLWidget>

#include "Matrix4x4.hpp"
#include "Texture.hpp"
#include "Shader.hpp"

typedef unsigned short MeshElementIndex;

class Mesh
{
public:
    Mesh(int verticesCount, int elementsCount, GLuint vertexArray, GLuint vertexPositionsBuffer, GLuint vertexNormalsBuffer, GLuint vertexTexcoordsBuffer, GLuint elementsBuffer);
    ~Mesh();
    
    int verticesCount() const { return verticesCount_; }
    int elementsCount() const { return elementsCount_; }
    GLuint vertexArray() const { return vertexArray_; }
    GLuint elementsBuffer() const { return elementsBuffer_; }
    
    void bind();
    
    static Mesh* load(const char* fileName);
    
private:
    int verticesCount_;
    int elementsCount_;
    GLuint vertexArray_;
    GLuint vertexPositionsBuffer_;
    GLuint vertexNormalsBuffer_;
    GLuint vertexTexcoordsBuffer_;
    GLuint elementsBuffer_;
};
