#pragma once

#define GL_GLEXT_PROTOTYPES 1 // Enables OpenGL 3 Features
#include <QGLWidget> // Links OpenGL Headers

#include <vector>

using namespace std;

#include "Bounds.hpp"
#include "Matrix4x4.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"

typedef unsigned short MeshElementIndex;

class Mesh
{
public:
    Mesh(vector<Vector3> positions, vector<Vector3> normals, vector<Vector4> tangents, vector<Vector2> texcoords, vector<MeshElementIndex> elements);
    ~Mesh();
    
    // Object space vertex positions
    const Vector3* vertices() const { return &positions_[0]; }
    
    // Vertex and elements info
    int verticesCount() const { return verticesCount_; }
    int elementsCount() const { return elementsCount_; }
    GLuint vertexArray() const { return vertexArray_; }
    GLuint elementsBuffer() const { return elementsBuffer_; }
    
    // Attaches the fbo and elements buffer for use.
    void bind();
    
    // Creates a fullscreen quad
    static Mesh* fullScreenQuad();
    
    // Loads a mesh from a file
    static Mesh* load(const char* fileName);
    
private:
    vector<Vector3> positions_;
    int verticesCount_;
    int elementsCount_;
    GLuint vertexArray_;
    GLuint vertexBuffers_[4];
    GLuint elementsBuffer_;
};
