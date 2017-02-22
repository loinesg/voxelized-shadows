#include "Mesh.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <cstdio>

using namespace std;

#include "Vector2.hpp"
#include "Vector3.hpp"

Mesh::Mesh(int verticesCount, int elementsCount, GLuint vertexArray, GLuint vertexPositionsBuffer, GLuint vertexNormalsBuffer, GLuint vertexTangentsBuffer, GLuint vertexTexcoordsBuffer, GLuint elementsBuffer)
    : verticesCount_(verticesCount),
    elementsCount_(elementsCount),
    vertexArray_(vertexArray),
    vertexPositionsBuffer_(vertexPositionsBuffer),
    vertexNormalsBuffer_(vertexNormalsBuffer),
    vertexTangentsBuffer_(vertexTangentsBuffer),
    vertexTexcoordsBuffer_(vertexTexcoordsBuffer),
    elementsBuffer_(elementsBuffer)
{
    
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &vertexArray_);
    glDeleteBuffers(1, &vertexPositionsBuffer_);
    glDeleteBuffers(1, &vertexNormalsBuffer_);
    glDeleteBuffers(1, &vertexTangentsBuffer_);
    glDeleteBuffers(1, &vertexTexcoordsBuffer_);
    glDeleteBuffers(1, &elementsBuffer_);
}

void Mesh::bind()
{
    glBindVertexArray(vertexArray_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsBuffer_);
}

Mesh* Mesh::load(const char* fileName)
{
    vector<Vector3> positions;
    vector<Vector3> normals;
    vector<Vector4> tangents;
    vector<Vector2> texcoords;
    vector<MeshElementIndex> elements;
    
    ifstream file(fileName);
    
    // Read lines from the mesh file
    while(file.is_open() && !file.fail() && !file.eof())
    {
        string type;
        file >> type;
        
        if(type == "vertex")
        {
            Vector3 v;
            file >> v;
            positions.push_back(v);
        }
        else if(type == "normal")
        {
            Vector3 n;
            file >> n;
            normals.push_back(n);
        }
        else if(type == "tangent")
        {
            Vector4 t;
            file >> t;
            tangents.push_back(t);
        }
        else if(type == "texcoord")
        {
            Vector2 t;
            file >> t;
            texcoords.push_back(t);
        }
        else if(type == "triangle")
        {
            MeshElementIndex a, b, c;
            file >> a >> b >> c;
            elements.push_back(a);
            elements.push_back(b);
            elements.push_back(c);
        }
        else
        {
            printf("Unknown mesh type %s in file %s \n", type.c_str(), fileName);
            return NULL;
        }
    }
    
    // Check for errors
    if(file.fail())
    {
        printf("Error reading mesh file %s \n", fileName);
        return NULL;
    }
    
    // Create vertex array
    GLuint vertexArray;
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    
    // Create vertex buffers
    GLuint vertexBuffers[4];
    glGenBuffers(4, vertexBuffers);
    
    // Positions buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * positions.size(), &positions[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normals buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * normals.size(), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, (void*)0);
    glEnableVertexAttribArray(1);
    
    // Tangents buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector4) * tangents.size(), &tangents[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 4, GL_FLOAT, false, 0, (void*)0);
    glEnableVertexAttribArray(2);
    
    // Texcoords buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2) * texcoords.size(), &texcoords[0], GL_STATIC_DRAW);
    glVertexAttribPointer(3, 2, GL_FLOAT, false, 0, (void*)0);
    glEnableVertexAttribArray(3);
    
    // Elements buffer
    GLuint elementsBuffer;
    glGenBuffers(1, &elementsBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MeshElementIndex) * elements.size(), &elements[0], GL_STATIC_DRAW);
    
    // Check for errors
    if(glGetError() != GL_NO_ERROR)
    {
        printf("Error creating mesh buffers \n");
        return NULL;
    }
    
    // No error
    return new Mesh((int)positions.size(), (int)elements.size(), vertexArray, vertexBuffers[0], vertexBuffers[1], vertexBuffers[2], vertexBuffers[3], elementsBuffer);
}
