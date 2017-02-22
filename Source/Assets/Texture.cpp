#include "Texture.hpp"

#include <QImage>
#include <cstdio>

Texture::Texture(GLuint id, int width, int height)
    : id_(id),
    width_(width),
    height_(height)
{
    
}

Texture::~Texture()
{
    glDeleteTextures(1, &id_);
}

void Texture::bind(GLenum textureSlot)
{
    glActiveTexture(textureSlot);
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
    
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    return new Texture(id, image.width(), image.height());
}
