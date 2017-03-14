#include "Shader.hpp"

#include <QFile>
#include <QTextStream>

#include "Platform.hpp"
#include "UniformManager.hpp"

Shader::Shader(const string &name, ShaderFeatureList features)
    : features_(features)
{
    // Get the fragment and vertex files
    string vertSource = SHADERS_DIRECTORY + name + ".vert.glsl";
    string fragSource = SHADERS_DIRECTORY + name + ".frag.glsl";
    
    // Compile vertex shader
    if(!compileShader(GL_VERTEX_SHADER, vertSource.c_str(), vertexShader_))
    {
        printf("Failed to compile vertex shader \n");
    }
    
    // Compile fragment shader
    if(!compileShader(GL_FRAGMENT_SHADER, fragSource.c_str(), fragmentShader_))
    {
        printf("Failed to compile fragment shader \n");
    }
    
    // Create program
    program_ = glCreateProgram();
    glAttachShader(program_, vertexShader_);
    glAttachShader(program_, fragmentShader_);
    glLinkProgram(program_);
    
    // Check for linking errors
    if(!checkLinkerErrors(program_))
    {
        printf("Failed to create program \n");
    }
    
    // Set uniform block binding
    setUniformBlockBinding("per_object_data", PerObjectUniformBuffer::BlockID);
    setUniformBlockBinding("scene_data", SceneUniformBuffer::BlockID);
    setUniformBlockBinding("shadow_data", ShadowUniformBuffer::BlockID);
    setUniformBlockBinding("voxel_data", VoxelsUniformBuffer::BlockID);
    
    // Store texture locations
    mainTextureLoc_ = glGetUniformLocation(program_, "_MainTexture");
    normalMapTextureLoc_ = glGetUniformLocation(program_, "_NormalMap");
    shadowMapTextureLoc_ = glGetUniformLocation(program_, "_ShadowMapTexture");
    shadowMaskTextureLoc_ = glGetUniformLocation(program_, "_ShadowMask");
    voxelDataTextureLoc_ = glGetUniformLocation(program_, "_VoxelData");
}

Shader::~Shader()
{
    glDeleteProgram(program_);
    glDeleteShader(vertexShader_);
    glDeleteShader(fragmentShader_);
}

bool Shader::hasFeature(ShaderFeature feature) const
{
    return (features_ & feature) != 0;
}

void Shader::bind()
{
    glUseProgram(program_);
    
    // Set texture locations
    glUniform1i(mainTextureLoc_, 0);
    glUniform1i(normalMapTextureLoc_, 1);
    glUniform1i(shadowMapTextureLoc_, 2);
    glUniform1i(shadowMaskTextureLoc_, 3);
    glUniform1i(voxelDataTextureLoc_, 4);
}

bool Shader::compileShader(GLenum type, const char* fileName, GLuint &id)
{
    QFile sourceFile(fileName);
    if(!sourceFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        printf("Failed to open shader file %s \n", fileName);
        return false;
    }
    
    // Get file contents
    QTextStream sourceStream(&sourceFile);
    QByteArray sourceBytes = sourceStream.readAll().toLocal8Bit();
    
    // Add feature #defines to the text
    string sourceText = (char*)sourceBytes.data();
    sourceText.insert(sourceText.find("\n"), createFeatureDefines());
    
    // Create and compile shader
    id = glCreateShader(type);
    char* sourceChars = (char*)sourceText.c_str();
    glShaderSource(id, 1, &sourceChars, NULL);
    glCompileShader(id);
    
    // Check for errors
    if(!checkShaderErrors(id))
    {
        printf("Shader error in %s \n", fileName);
        return false;
    }
    
    return true;
}

bool Shader::checkShaderErrors(GLuint shaderID)
{
    GLint ok;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &ok);
    
    if(!ok)
    {
        GLint logLength;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
        
        // Print the error log
        char* log = new char[logLength];
        glGetShaderInfoLog(shaderID, logLength, NULL, log);
        printf("Shader Error. Log: %s \n", log);
        delete[] log;
        
        // Error
        return false;
    }
    
    // No error
    return true;
}

bool Shader::checkLinkerErrors(GLuint programID)
{
    GLint linked = 0;
    glGetProgramiv(programID, GL_LINK_STATUS, (int*)&linked);
    
    if(!linked)
    {
        GLint logLength;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
        
        // Print the error log
        char* log = new char[logLength];
        glGetProgramInfoLog(programID, logLength, NULL, log);
        printf("Program link error. Log: %s \n", log);
        delete[] log;
        
        // Error
        return false;
    }
    
    // No error
    return true;
}

void Shader::setUniformBlockBinding(const char *blockName, GLuint id)
{
    GLuint blockIndex = glGetUniformBlockIndex(program_, blockName);
    
    if(blockIndex != GL_INVALID_INDEX)
    {
        glUniformBlockBinding(program_, blockIndex, id);
    }
}

string Shader::createFeatureDefines() const
{
    string defines = "";
    
    // Shader feature defines
    if(hasFeature(SF_Texture)) defines += "\n #define TEXTURE_ON";
    if(hasFeature(SF_NormalMap)) defines += "\n #define NORMAL_MAP_ON";
    if(hasFeature(SF_Specular)) defines += "\n #define SPECULAR_ON";
    if(hasFeature(SF_Cutout)) defines += "\n #define ALPHA_TEST_ON";
    
    // Debug feature defines
    if(hasFeature(SF_Debug_ShadowMapTexture)) defines += "\n #define DEBUG_SHADOW_MAP_TEXTURE";
    if(hasFeature(SF_Debug_DepthTexture)) defines += "\n #define DEBUG_DEPTH_TEXTURE";
    if(hasFeature(SF_Debug_ShowCascadeSplits)) defines += "\n #define DEBUG_SHOW_CASCADE_SPLITS";
    if(hasFeature(SF_Debug_ShowVoxelTreeDepth)) defines += "\n #define DEBUG_SHOW_VOXEL_TREE_DEPTH";
    
    return defines;
}
