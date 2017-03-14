#include "Mesh.hpp"

#include <string>
#include <fstream>
#include <cstdio>

Mesh::Mesh(vector<Vector3> positions, vector<Vector3> normals, vector<Vector4> tangents, vector<Vector2> texcoords, vector<MeshElementIndex> elements)
    : positions_(positions),
    verticesCount_(positions.size()),
    elementsCount_(elements.size())
{
    // Create vertex array
    glGenVertexArrays(1, &vertexArray_);
    glBindVertexArray(vertexArray_);
    
    // Create vertex buffers
    glGenBuffers(4, vertexBuffers_);
    
    // Positions buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers_[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * positions.size(), &positions[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normals buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers_[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * normals.size(), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, (void*)0);
    glEnableVertexAttribArray(1);
    
    // Tangents buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers_[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector4) * tangents.size(), &tangents[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 4, GL_FLOAT, false, 0, (void*)0);
    glEnableVertexAttribArray(2);
    
    // Texcoords buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers_[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2) * texcoords.size(), &texcoords[0], GL_STATIC_DRAW);
    glVertexAttribPointer(3, 2, GL_FLOAT, false, 0, (void*)0);
    glEnableVertexAttribArray(3);
    
    // Elements buffer
    glGenBuffers(1, &elementsBuffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsBuffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MeshElementIndex) * elements.size(), &elements[0], GL_STATIC_DRAW);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &vertexArray_);
    glDeleteBuffers(4, vertexBuffers_);
    glDeleteBuffers(1, &elementsBuffer_);
}

void Mesh::bind()
{
    glBindVertexArray(vertexArray_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsBuffer_);
}

Mesh* Mesh::fullScreenQuad()
{
    // Create attribute lists
    vector<Vector3> positions;
    vector<Vector3> normals;
    vector<Vector4> tangents;
    vector<Vector2> texcoords;
    vector<MeshElementIndex> elements;
    
    // 4 vertices are needed
    positions.push_back(Vector3(-1.0, -1.0, 1.0));
    positions.push_back(Vector3(1.0, -1.0, 1.0));
    positions.push_back(Vector3(1.0, 1.0, 1.0));
    positions.push_back(Vector3(-1.0, 1.0, 1.0));
    
    for(int i = 0; i < 4; ++i)
    {
        normals.push_back(Vector3::zero());
        tangents.push_back(Vector4(0.0, 0.0, 0.0, 0.0));
        texcoords.push_back(Vector2(0.0, 0.0));
    }
    
    // First triangle (lower right)
    elements.push_back(0);
    elements.push_back(1);
    elements.push_back(2);
    
    // Second triangle (upper left)
    elements.push_back(0);
    elements.push_back(2);
    elements.push_back(3);
    
    // Create mesh
    return new Mesh(positions, normals, tangents, texcoords, elements);
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
            elements.push_back(c);
            elements.push_back(b);
            elements.push_back(a);
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
    
    return new Mesh(positions, normals, tangents, texcoords, elements);
}
