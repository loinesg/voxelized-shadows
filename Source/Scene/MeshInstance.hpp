#pragma once

#include "Object.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

class MeshInstance : public Object
{
public:
    MeshInstance(Mesh* mesh, ShaderFeatureList shaderFeatures, Texture* texture, Texture* normalMap);
    
    Mesh* mesh() const { return mesh_; }
    ShaderFeatureList shaderFeatures() const { return shaderFeatures_; }
    Texture* texture() const { return texture_; }
    Texture* normalMap() const { return normalMap_; }
    
private:
    Mesh* mesh_;
    ShaderFeatureList shaderFeatures_;
    Texture* texture_;
    Texture* normalMap_;
};
