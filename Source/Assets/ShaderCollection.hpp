#pragma once

#define GL_GLEXT_PROTOTYPES 1 // Enables OpenGL 3 Features
#include <QGLWidget> // Links OpenGL Headers

#include <string>
#include <vector>

using namespace std;

#include "Shader.hpp"

// Manages loading and binding variants of a shader.
class ShaderCollection
{
public:
    ShaderCollection(const string &name, const string &directory);
    ~ShaderCollection();
    
    // Feature toggling
    void enableFeature(ShaderFeature feature);
    void disableFeature(ShaderFeature feature);
    void setSupportedFeatures(ShaderFeatureList supportedFeatures);
    
    // Finding and loading variants
    Shader* getVariant(ShaderFeatureList features);
    
private:
    string vertexShader_;
    string fragmentShader_;
    ShaderFeatureList supportedFeatures_;
    ShaderFeatureList enabledFeatures_;
    vector<Shader*> shaderVariants_;
    
    // Variant lookup and creation
    Shader* findShader(ShaderFeatureList features) const;
    Shader* createShader(ShaderFeatureList features);
};
