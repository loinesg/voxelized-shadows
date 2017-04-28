#pragma once

#define GL_GLEXT_PROTOTYPES 1 // Enables OpenGL 3 Features
#include <QGLWidget> // Links OpenGL Headers

#include <string>
#include <vector>

using namespace std;


enum ShaderFeature
{
    // Enables texturing
    SF_Texture = 1,
    
    // Enables normal mapping
    SF_NormalMap = 2,
    
    // Enables specular highlights
    SF_Specular = 4,
    
    // Enables AlphaTest cutout transparency
    SF_Cutout = 8,
    
    // Visualization of shadow maps
    SF_Debug_ShadowMapTexture = 16,
    
    // Visualization of depth textures
    SF_Debug_DepthTexture = 32,
    
    // Visualization of cascade split distances
    SF_Debug_ShowCascadeSplits = 64,
    
    // Visualization of the projected shadow map
    SF_Debug_ShowProjectedShadowMap = 128,
    
    // Visualization of the voxel tree traversal depth
    SF_Debug_ShowVoxelTreeDepth = 256,
    
    // Enables voxel PCF filtering
    SF_Shadow_PCF_Filter = 512,
};


// Encodes a combination of shader features.
typedef unsigned int ShaderFeatureList;


// Manages a single variant of a shader.
class Shader
{
public:
    Shader(const string &name, ShaderFeatureList features);
    ~Shader();
    
    // Feature management
    ShaderFeatureList features() const { return features_; }
    bool hasFeature(ShaderFeature feature) const;
    
    // Program and shader ids
    GLuint program() const { return program_; }
    GLuint vertexShader() const { return vertexShader_; }
    GLuint fragmentShader() const { return fragmentShader_; }
    
    void bind();
    
private:
    ShaderFeatureList features_;
    GLuint program_;
    GLuint vertexShader_;
    GLuint fragmentShader_;
    GLint mainTextureLoc_;
    GLint normalMapTextureLoc_;
    GLint shadowMapTextureLoc_;
    GLint shadowMaskTextureLoc_;
    GLint voxelDataTextureLoc_;
    
    // Shader compilation
    bool compileShader(GLenum type, const char* file, GLuint &id);
    bool checkShaderErrors(GLuint shaderID);
    bool checkLinkerErrors(GLuint programID);
    
    // Sets a uniform block binding
    void setUniformBlockBinding(const char* blockName, GLuint id);
    
    // Creates a #define list for the enabled features
    string createFeatureDefines() const;
};
