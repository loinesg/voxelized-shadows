#pragma once

#include <QGLWidget>
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
    SF_Cutout = 8
};


// Encodes a combination of shader features.
typedef unsigned int ShaderFeatureList;


// Manages a single variant of a shader.
class Shader
{
public:
    Shader(ShaderFeatureList features, const string &vertSource, const string &fragSource);
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
    
    // Shader compilation
    bool compileShader(GLenum type, const char* file, GLuint &id);
    bool checkShaderErrors(GLuint shaderID);
    bool checkLinkerErrors(GLuint programID);
    
    // Sets a uniform block binding
    void setUniformBlockBinding(const char* blockName, GLuint id);
    
    // Creates a #define list for the enabled features
    string createFeatureDefines() const;
};
