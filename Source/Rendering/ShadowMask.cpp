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
    shadowMapPass_->setSupportedFeatures(SF_Shadow_PCF_Filter);
    
    // RenderPass for the VoxelTree method
    voxelTreePass_ = new RenderPass("ShadowSamplingPass-Voxel", uniformManager);
    voxelTreePass_->setSupportedFeatures(SF_Shadow_PCF_Filter);
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

void ShadowMask::enableFeature(ShaderFeature feature)
{
    shadowMapPass_->enableFeature(feature);
    voxelTreePass_->enableFeature(feature);
}

void ShadowMask::disableFeature(ShaderFeature feature)
{
    shadowMapPass_->disableFeature(feature);
    voxelTreePass_->disableFeature(feature);
}

void ShadowMask::setMethod(ShadowMaskMethod method)
{
    method_ = method;
    
    if(method == SMM_Combined)
    {
        // The shadow map pass should not clear the framebuffer
        // when being rendered in combined mode as it is rendered
        // *after* the voxel tree pass.
        shadowMapPass_->setClearFlags(GL_NONE);
    }
    else
    {
        // Otherwise, clear
        shadowMapPass_->setClearFlags(GL_DEPTH_BUFFER_BIT);
    }
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
    
    // Execute the voxel tree pass, unless we are
    // using the shadow map only.
    if(method_ != SMM_ShadowMap)
    {
        // Bind the input shadow tree
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_BUFFER, voxelTree_->treeBufferTexture());
        
        // Render using the voxel tree pass
        voxelTreePass_->renderFullScreen();
    }
    
    // Execute the shadow map pass, unless we are
    // using the voxel tree only.
    if(method_ != SMM_VoxelTree)
    {
        // Bind the input shadow map texture
        shadowMapTexture_->bind(GL_TEXTURE2);
        
        // Use MIN blending in combined mode
        // This combines the shadow map sampling
        // with the earlier voxel tree sampling.
        if(method_ == SMM_Combined)
        {
            glEnable(GL_BLEND);
            glBlendEquation(GL_MIN);
        }
        
        // Render using the shadow map pass
        // This will not clear when in combined mode.
        shadowMapPass_->renderFullScreen();
        
        // Disable blending
        glDisable(GL_BLEND);
    }
}
