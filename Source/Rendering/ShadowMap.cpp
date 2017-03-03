#include "ShadowMap.hpp"

#include "UniformManager.hpp"

ShadowMap::ShadowMap(UniformManager* uniformManager, int width, int height)
    : uniformManager_(uniformManager),
    shadowCamera_()
{
    // Create a depth texture for the shadow map
    shadowDepthTexture_ = Texture::depth(width, height);
    
    // Add a border with max depth for areas outside the shadow map
    shadowDepthTexture_->setWrapMode(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
    shadowDepthTexture_->setBorder(1.0, 1.0, 1.0, 1.0);
    
    // Enable hardware bilinear PCF.
    shadowDepthTexture_->setCompareMode(GL_COMPARE_REF_TO_TEXTURE, GL_LEQUAL);
    shadowDepthTexture_->setMinFilter(GL_LINEAR);
    shadowDepthTexture_->setMagFilter(GL_LINEAR);
    
    // Use an orthographic camera for shadows.
    shadowCamera_.setType(CameraType::Orthographic);
    shadowCamera_.setOrthographicSize(100.0);
    
    // Use a negative near plane so the camera volume
    // is centered on the camera position.
    shadowCamera_.setNearPlane(-100.0);
    shadowCamera_.setFarPlane(100.0);
    
    // Create a framebuffer for the shadow map camera
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthTexture_->id(), 0);
    
    // Use the framebuffer as the shadow camera render target
    shadowCamera_.setFramebuffer(framebuffer);
    shadowCamera_.setPixelWidth(width);
    shadowCamera_.setPixelHeight(height);
}

ShadowMap::~ShadowMap()
{
    // Delete the framebuffer
    GLuint framebuffer = shadowCamera_.framebuffer();
    glDeleteFramebuffers(1, &framebuffer);
    
    // Delete the shadow map texture
    delete shadowDepthTexture_;
}

void ShadowMap::setResolution(int resolution)
{
    // Use square shadow map textures
    shadowDepthTexture_->setResolution(resolution, resolution);
    
    // Adjust the camera viewport to match
    shadowCamera_.setPixelWidth(resolution);
    shadowCamera_.setPixelHeight(resolution);
}

void ShadowMap::updatePosition(Camera* viewCamera, Light* light)
{
    // Ensure the shadow map is drawn from the light's view direction
    shadowCamera_.setRotation(light->rotation());
    
    // Centre the shadow map on the viewer.
    shadowCamera_.setPosition(viewCamera->position());
}

void ShadowMap::updateUniformBuffer(Camera* cam) const
{
    float w = shadowDepthTexture_->width();
    float h = shadowDepthTexture_->height();
    
    ShadowUniformBuffer shadowData;
    shadowData.worldToShadow = worldToShadowMatrix() * cam->cameraToWorldMatrix();
    shadowData.shadowTexelSize = Vector4(w, h, 1.0 / w, 1.0 / h);
    uniformManager_->updateShadowBuffer(shadowData);
}

void ShadowMap::bindTexture(GLenum textureSlot) const
{
    // Attach the depth texture
    shadowDepthTexture_->bind(textureSlot);
}

Matrix4x4 ShadowMap::worldToShadowMatrix() const
{
    // Offset matrix transforms coordinates from
    // [-1 - 1] to [0-1] for texture sampling.
    Matrix4x4 offsetMatrix;
    offsetMatrix.setRow(0, 0.5, 0.0, 0.0, 0.5);
    offsetMatrix.setRow(1, 0.0, 0.5, 0.0, 0.5);
    offsetMatrix.setRow(2, 0.0, 0.0, 0.5, 0.5);
    offsetMatrix.setRow(3, 0.0, 0.0, 0.0, 1.0);
    
    // Transform world coordinate to camera coordinates,
    // then to texture coordinates.
    return offsetMatrix * shadowCamera_.worldToCameraMatrix();
}
