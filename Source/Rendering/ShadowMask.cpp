#include "ShadowMask.hpp"

#include "UniformManager.hpp"

ShadowMask::ShadowMask(UniformManager* uniformManager)
    : texture_(NULL)
{
    // Create a single channel texture for the shadow mask
    texture_ = Texture::singleChannel(1, 1);
    texture_->setWrapMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    texture_->setMinFilter(GL_NEAREST);
    texture_->setMagFilter(GL_NEAREST);
    
    // Create a framebuffer for shadow mask rendering
    glGenFramebuffers(1, &frameBuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_->id(), 0);
    
    // Create the shadow mask pass. No features needed.
    string shadowMaskPassName = "ShadowSamplingPass";
    shadowMaskPass_ = new RenderPass(shadowMaskPassName, uniformManager);
    shadowMaskPass_->setSupportedFeatures(0);
}

ShadowMask::~ShadowMask()
{
    // Delete the framebuffer
    glDeleteFramebuffers(1, &frameBuffer_);
    
    // Delete the texture
    delete texture_;
    
    // Delete the render
    delete shadowMaskPass_;
}

void ShadowMask::setResolution(int width, int height)
{
    texture_->setResolution(width, height);
}

void ShadowMask::setShadowMapTexture(Texture* shadowMapTexture)
{
    shadowMapTexture_ = shadowMapTexture;
}

void ShadowMask::setSceneDepthTexture(Texture* sceneDepthTexture)
{
    sceneDepthTexture_ = sceneDepthTexture;
}

void ShadowMask::render()
{
    // Bind the shadow mask framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_);
    
    // Dont depth test, and write to color only.
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    glColorMask(true, true, true, true);
    
    // Bind the input textures
    sceneDepthTexture_->bind(GL_TEXTURE0);
    shadowMapTexture_->bind(GL_TEXTURE2);
    
    // Draw in full screen
    glViewport(0, 0, texture_->width(), texture_->height());
    
    // Render the shadow mask
    shadowMaskPass_->renderFullScreen();
}
