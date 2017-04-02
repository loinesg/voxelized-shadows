#pragma once

#define GL_GLEXT_PROTOTYPES 1 // Enables OpenGL 3 Features
#include <QGLWidget> // Links OpenGL Headers

#include <vector>

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
    RenderPass(const string &name, UniformManager* uniformManager);
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
    // The meshes can be filtered based on their static flag state.
    void submit(Camera* camera, const vector<MeshInstance*>* instances, bool drawStatic = true, bool drawDynamic = true);
    
    // Draws a full screen quad using all enabled shader features.
    void renderFullScreen();
                       
private:
    string name_;
    PassClearFlags clearFlags_;
    PassClearColor clearColor_;
    ShaderCollection* shaderCollection_;
    UniformManager* uniformManager_;
    Mesh* fullScreenQuad_;
};
