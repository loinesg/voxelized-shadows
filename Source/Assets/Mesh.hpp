#pragma once

#include <QGLWidget>
#include <vector>

using namespace std;

#include "Matrix4x4.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"

typedef unsigned short MeshElementIndex;

class Mesh
{
public:
    Mesh(int verticesCount, int elementsCount, GLuint vertexArray, GLuint vertexPositionsBuffer, GLuint vertexNormalsBuffer, GLuint vertexTangentsBuffer, GLuint vertexTexcoordsBuffer, GLuint elementsBuffer);
    ~Mesh();
    
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
    
    // Creates a mesh from lists of vertices, normals, tangents, texcoords and elements.
    static Mesh* create(vector<Vector3> positions, vector<Vector3> normals, vector<Vector4> tangents, vector<Vector2> texcoords, vector<MeshElementIndex> elements);
    
private:
    int verticesCount_;
    int elementsCount_;
    GLuint vertexArray_;
    GLuint vertexPositionsBuffer_;
    GLuint vertexNormalsBuffer_;
    GLuint vertexTangentsBuffer_;
    GLuint vertexTexcoordsBuffer_;
    GLuint elementsBuffer_;
};
