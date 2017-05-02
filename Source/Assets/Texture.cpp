#include "Texture.hpp"

#include <QImage>
#include <cstdio>

Texture::Texture(GLuint id, int width, int height, GLint internalFormat, GLenum format)
    : id_(id),
    width_(width),
    height_(height),
    internalFormat_(internalFormat),
    format_(format)
{
    
}

Texture::~Texture()
{
    glDeleteTextures(1, &id_);
}

void Texture::setWrapMode(GLint horizontal, GLint vertical)
{
    glBindTexture(GL_TEXTURE_2D, id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, horizontal);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, vertical);
}

void Texture::setMinFilter(GLint filter)
{
    glBindTexture(GL_TEXTURE_2D, id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
}

void Texture::setMagFilter(GLint filter)
{
    glBindTexture(GL_TEXTURE_2D, id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
}

void Texture::setResolution(int width, int height)
{
    // Check if the texture is already the correct resolution
    if(width_ == width && height_ == height)
    {
        return;
    }
    
    // Resize the texture
    glBindTexture(GL_TEXTURE_2D, id_);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat_, width, height, 0, format_, GL_UNSIGNED_BYTE, (void*)0);
    
    // Store the new resolution
    width_ = width;
    height_ = height;
}

void Texture::setCompareMode(GLenum mode, GLenum func)
{
    glBindTexture(GL_TEXTURE_2D, id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, mode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, func);
}

void Texture::generateMipmaps()
{
    glBindTexture(GL_TEXTURE_2D, id_);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::bind(GLenum target)
{
    glActiveTexture(target);
    glBindTexture(GL_TEXTURE_2D, id_);
}

Texture* Texture::load(const char* fileName)
{
    QImage image;
    if(!image.load(QString(fileName)))
    {
        printf("Could not open texture file %s \n", fileName);
        return NULL;
    }
    
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA8,
                 image.width(),
                 image.height(),
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 QGLWidget::convertToGLFormat(image).bits());
    
    Texture* texture = new Texture(id, image.width(), image.height(), GL_RGBA8, GL_RGBA);
    texture->setWrapMode(GL_REPEAT, GL_REPEAT);
    texture->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
    texture->setMagFilter(GL_LINEAR);
    texture->generateMipmaps();
    
    return texture;
}

Texture* Texture::depth(int width, int height)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    
    return new Texture(texture, width, height, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT);
}

Texture* Texture::singleChannel(int width, int height)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
    
    return new Texture(texture, width, height, GL_RED, GL_RED);
}
