#include "ShadowMask.hpp"

#include "UniformManager.hpp"

ShadowMask::ShadowMask(UniformManager* uniformManager, ShadowMaskMethod method)
    : method_(method),
    texture_(NULL),
    voxelTree_(NULL)
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
    
    // RenderPass for the ShadowMap method
    shadowMapPass_ = new RenderPass("ShadowSamplingPass", uniformManager);
    shadowMapPass_->setSupportedFeatures(0);
    
    // RenderPass for the VoxelTree method
    voxelTreePass_ = new RenderPass("ShadowSamplingPass-Voxel", uniformManager);
    voxelTreePass_->setSupportedFeatures(0);
}

ShadowMask::~ShadowMask()
{
    // Delete the framebuffer
    glDeleteFramebuffers(1, &frameBuffer_);
    
    // Delete the texture
    delete texture_;
    
    // Delete the render passes
    delete shadowMapPass_;
    delete voxelTreePass_;
}

void ShadowMask::setMethod(ShadowMaskMethod method)
{
    method_ = method;
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

void ShadowMask::setVoxelTree(const VoxelTree* voxelTree)
{
    voxelTree_ = voxelTree;
}

void ShadowMask::render()
{
    // Bind the shadow mask framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_);
    
    // Dont depth test, and write to color only.
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    glColorMask(true, true, true, true);
    
    // Bind the input depth texture
    sceneDepthTexture_->bind(GL_TEXTURE0);
    
    // Draw in full screen
    glViewport(0, 0, texture_->width(), texture_->height());
    
    if(method_ == SMM_ShadowMap)
    {
        // Bind the input shadow map texture
        shadowMapTexture_->bind(GL_TEXTURE2);
        
        // Render using the shadow map pass
        shadowMapPass_->renderFullScreen();
    }
    else // method == SMM_VoxelTree
    {
        // Bind the input shadow tree
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_BUFFER, voxelTree_->treeBufferTexture());
        
        // Render using the voxel tree pass
        voxelTreePass_->renderFullScreen();
    }
}
