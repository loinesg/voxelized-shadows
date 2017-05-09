#pragma once

#include "Object.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

class MeshInstance : public Object
{
public:
    MeshInstance(Mesh* mesh, ShaderFeatureList shaderFeatures, Texture* texture, Texture* normalMap);
    
    // True if the mesh is static.
    // This will cause the mesh to be rendered into voxel trees.
    bool isStatic() const;
    
    // The shader features enabled by the mesh
    // They will be used in the relevent passes if globally enabled.
    ShaderFeatureList shaderFeatures() const { return shaderFeatures_; }
    
    // The mesh to be rendered
    Mesh* mesh() const { return mesh_; }
    
    // The textures used for shading
    Texture* texture() const { return texture_; }
    Texture* normalMap() const { return normalMap_; }
    
    // Makes the mesh non static
    // This should be done for animated meshes
    void makeNonStatic();
    
private:
    
    // True if static.
    // Static objects are not animated and are rendered into voxel trees
    bool static_;
    
    // Rendering assets / settings
    Mesh* mesh_;
    ShaderFeatureList shaderFeatures_;
    Texture* texture_;
    Texture* normalMap_;
};
