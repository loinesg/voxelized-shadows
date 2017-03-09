#pragma once

#define GL_GLEXT_PROTOTYPES 1 // Enables OpenGL 3 Features
#include <QGLWidget> // Links OpenGL Headers

#include <string>

using namespace std;

#include "Shader.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"

class Overlay
{
public:
    Overlay(const string &name, const string &shaderName, ShaderFeatureList shaderFeatures);
    ~Overlay();
    
    // Overlay name (Shown in GUI)
    string name() const { return name_; }
    
    // Display settings
    bool fullScreen() const { return fullScreen_; }
    bool useBlending() const { return useBlending_; }
    Texture* texture() const { return texture_; }
    
    // Display setting setters
    void setFullScreen(bool fullscreen);
    void setUseBlending(bool blending);
    void setTexture(Texture* texture);
    
    // Draws the overlay to the current framebuffer
    void draw(const Camera* camera);
    
private:
    string name_;
    Shader* shader_;
    Mesh* mesh_;
    Texture* texture_;
    bool fullScreen_;
    bool useBlending_;
};
