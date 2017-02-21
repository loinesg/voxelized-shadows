#pragma once

#include <vector>
#include <QGLWidget>

using namespace std;

#include "Camera.hpp"
#include "ShaderCollection.hpp"
#include "UniformManager.hpp"
#include "MeshInstance.hpp"

typedef GLbitfield PassClearFlags;
typedef Vector4 PassClearColor;

class RenderPass
{
public:
    RenderPass(const string &name, const string &shadersDirectory, UniformManager* uniformManager);
    ~RenderPass();
    
    string name() const { return name_; }
    PassClearFlags clearFlags() const { return clearFlags_; }
    PassClearColor clearColor() const { return clearColor_; }
    
    // Clear settings
    void setClearFlags(PassClearFlags clearFlags);
    void setClearColor(PassClearColor color);
    
    // Shader features
    void enableFeature(ShaderFeature feature);
    void disableFeature(ShaderFeature feature);
    void setSupportedFeatures(ShaderFeatureList supportedFeatures);
    
    // Sends draw commands to the graphics API.
    void submit(Camera* camera, const vector<MeshInstance>* instances);
                       
private:
    string name_;
    PassClearFlags clearFlags_;
    PassClearColor clearColor_;
    ShaderCollection* shaderCollection_;
    UniformManager* uniformManager_;
};
