#include "MeshInstance.hpp"

MeshInstance::MeshInstance(Mesh* mesh, ShaderFeatureList shaderFeatures, Texture* texture, Texture* normalMap)
    : static_(true),
    mesh_(mesh),
    shaderFeatures_(shaderFeatures),
    texture_(texture),
    normalMap_(normalMap)
{
    
}

bool MeshInstance::isStatic() const
{
    return static_;
}

void MeshInstance::makeNonStatic()
{
    static_ = false;
}
