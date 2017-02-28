#pragma once

#include <QGLWidget>

using namespace std;

class Texture
{
public:
    Texture(GLuint id, int width, int height);
    ~Texture();
    
    GLuint id() const { return id_; }
    int width() const { return width_; }
    int height() const { return height_; }
    
    // Basic texture settings
    void setWrapMode(GLint horizontal, GLint vertical);
    void setMinFilter(GLint filter);
    void setMagFilter(GLint filter);
    
    // Adds a colour border to the texture
    void setBorder(float r, float g, float b, float a);
    
    // For depth textures.
    // Sets the texture comparison mode and function
    void setCompareMode(GLenum mode, GLenum func);
    
    // Generates mipmaps for the texture
    void generateMipmaps();
    
    // Binds the texture to the specified target
    void bind(GLenum target);
    
    // Loads a texture from a file.
    static Texture* load(const char* fileName);
    
    // Creates a depth texture.
    static Texture* depth(int width, int height);
    
    // Creates a texture with a single colour channel.
    static Texture* singleChannel(int width, int height);
    
private:
    GLuint id_;
    int width_;
    int height_;
};
