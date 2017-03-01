#include "ShaderCollection.hpp"

ShaderCollection::ShaderCollection(const string &name, const string &directory)
: supportedFeatures_(~0),
enabledFeatures_(~0),
shaderVariants_()
{
    vertexShader_ = directory + name + ".vert.glsl";
    fragmentShader_ = directory + name + ".frag.glsl";
}

ShaderCollection::~ShaderCollection()
{
    for(unsigned int i = 0; i < shaderVariants_.size(); ++i)
    {
        delete shaderVariants_[i];
    }
}

void ShaderCollection::enableFeature(ShaderFeature feature)
{
    enabledFeatures_ |= feature;
}

void ShaderCollection::disableFeature(ShaderFeature feature)
{
    enabledFeatures_ &= ~feature;
}

void ShaderCollection::setSupportedFeatures(ShaderFeatureList supportedFeatures)
{
    supportedFeatures_ = supportedFeatures;
}

Shader* ShaderCollection::getVariant(ShaderFeatureList features)
{
    // Only use features that are enabled and supported.
    features &= (enabledFeatures_ & supportedFeatures_);
    
    // Try to find a cached shader.
    Shader* existing = findShader(features);
    if(existing != NULL)
    {
        return existing;
    }
    
    // No cached shader. Create a new shader.
    return createShader(features);
}

Shader* ShaderCollection::findShader(ShaderFeatureList features) const
{
    for(unsigned int i = 0; i < shaderVariants_.size(); ++i)
    {
        Shader* shader = shaderVariants_[i];
        
        if(shader->features() == features)
        {
            return shader;
        }
    }
    
    // No shader exists.
    return NULL;
}

Shader* ShaderCollection::createShader(ShaderFeatureList features)
{
    Shader* shader = new Shader(features, vertexShader_, fragmentShader_);
    shaderVariants_.push_back(shader);
    
    return shader;
}
