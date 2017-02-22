#include "MeshInstance.hpp"

MeshInstance::MeshInstance(Mesh* mesh, ShaderFeatureList shaderFeatures, Texture* texture, Texture* normalMap)
    : mesh_(mesh),
    shaderFeatures_(shaderFeatures),
    texture_(texture),
    normalMap_(normalMap)
{
    
}
