#include "MeshInstance.hpp"

MeshInstance::MeshInstance(Mesh* mesh, ShaderFeatureList shaderFeatures, Texture* texture)
    : mesh_(mesh),
    shaderFeatures_(shaderFeatures),
    texture_(texture)
{
    
}
