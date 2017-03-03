#include "ShadowMask.hpp"

#include "UniformManager.hpp"

ShadowMask::ShadowMask()
    : texture_(NULL)
{
    // Create a framebuffer for shadow mask rendering
    glGenFramebuffers(1, &frameBuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_);
    
    // Create a single channel texture for the shadow mask
    texture_ = Texture::singleChannel(1, 1);
    
    // Wrap is not needed
    texture_->setWrapMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    
    // Texture resolution = screen resolution so disable filtering
    texture_->setMinFilter(GL_NEAREST);
    texture_->setMagFilter(GL_NEAREST);
    
    // Attach the shadow mask texture to the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_->id(), 0);
}

ShadowMask::~ShadowMask()
{
    // Delete the framebuffer
    glDeleteFramebuffers(1, &frameBuffer_);
    
    // Delete any created texture
    if(texture_ != NULL)
    {
        delete texture_;
    }
}

void ShadowMask::setResolution(int width, int height)
{
    texture_->setResolution(width, height);
}

void ShadowMask::bindTexture(GLenum textureSlot) const
{
    // Attach the depth texture
    texture_->bind(textureSlot);
}

