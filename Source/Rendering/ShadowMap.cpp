#include "ShadowMap.hpp"

#include <math.h>
#include <assert.h>

#include "UniformManager.hpp"
#include "Bounds.hpp"

ShadowMap::ShadowMap(Scene* scene, UniformManager* uniformManager, int cascadesCount, int resolution)
    : scene_(scene),
    uniformManager_(uniformManager),
    cascades_()
{
    assert(cascadesCount > 0 && cascadesCount <= 4);
    assert(resolution > 0);
    
    // Create a pass for rendering the shadow map.
    // Depth only, so no features are needed except cutout.
    string shadowCasterPassName = "ShadowCasterPass";
    shadowCasterPass_ = new RenderPass(shadowCasterPassName, uniformManager_);
    shadowCasterPass_->setSupportedFeatures(SF_Cutout);
    shadowCasterPass_->setClearFlags(GL_DEPTH_BUFFER_BIT);
    
    // Create a depth texture for the shadow map with hardware bilinear PCF.
    texture_ = Texture::depth(resolution * cascadesCount, resolution);
    texture_->setCompareMode(GL_COMPARE_REF_TO_TEXTURE, GL_LEQUAL);
    texture_->setMinFilter(GL_LINEAR);
    texture_->setMagFilter(GL_LINEAR);
    
    // Create a framebuffer for the shadow map camera
    glGenFramebuffers(1, &framebuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture_->id(), 0);

    // Setup the correct cascade count and resolution
    setCascades(cascadesCount, resolution);
}

ShadowMap::~ShadowMap()
{
    // Delete the framebuffer
    glDeleteFramebuffers(1, &framebuffer_);
    
    // Delete the shadow map texture
    delete texture_;
    
    // Delete the render pass
    delete shadowCasterPass_;
}

void ShadowMap::setCascades(int cascadesCount, int resolution)
{
    assert(cascadesCount > 0 && cascadesCount <= 4);
    assert(resolution > 0);
    
    cascadesCount_ = cascadesCount;
    resolution_ = resolution;
    
    // Use square shadow map textures
    // Place all cascade textures in a horizontal line
    texture_->setResolution(resolution_ * cascadesCount_, resolution_);
    
    // Set up each cascade
    for(int i = 0; i < cascadesCount_; ++i)
    {
        // Calculate the min and max distance of the cascade
        cascades_[i].minDistance = getCascadeMin(i, 150.0);
        cascades_[i].maxDistance = getCascadeMax(i, 150.0);
        
        // Adjust the camera viewport to match the texture atlas
        cascades_[i].camera.setPixelOffsetX(resolution_ * i);
        cascades_[i].camera.setPixelOffsetY(0);
        cascades_[i].camera.setPixelWidth(resolution_);
        cascades_[i].camera.setPixelHeight(resolution_);
        
        // Use an orthographic camera for shadows.
        cascades_[i].camera.setType(CameraType::Orthographic);
        
        // Use a negative near plane so the camera volume
        // is centered on the camera position.
        cascades_[i].camera.setNearPlane(-100.0);
        cascades_[i].camera.setFarPlane(100.0);
        
        // Use the framebuffer as the shadow camera render target
        cascades_[i].camera.setFramebuffer(framebuffer_);
    }
    
    // Ensure unused cascades are not sampled
    for(int i = cascadesCount_; i < MaxCascades; ++i)
    {
        // Make distance too far away to be used
        cascades_[i].minDistance = 10000000.0;
        cascades_[i].maxDistance = 10000000.0;
    }
}

void ShadowMap::updatePosition(Camera* viewCamera, Light* light)
{
    // Ensure each cascade is drawn from the correct direction
    for(int i = 0; i < cascadesCount_; ++i)
    {
        cascades_[i].camera.setRotation(light->rotation());
    }
    
    // Get the world to light space transformation matrix (without translation)
    Matrix4x4 worldToLight = cascades_[0].camera.worldToLocal();
    worldToLight.set(0, 3, 0.0);
    worldToLight.set(1, 3, 0.0);
    worldToLight.set(2, 3, 0.0);
    
    // Get the light to world transformation matrix (without translation)
    Matrix4x4 lightToWorld = cascades_[0].camera.localToWorld();
    lightToWorld.set(0, 3, 0.0);
    lightToWorld.set(1, 3, 0.0);
    lightToWorld.set(2, 3, 0.0);
    
    // Compute the view to light matrix
    Matrix4x4 viewToLight = worldToLight * viewCamera->localToWorld();
    
    // Set up each cascade camera size and centre
    for(int i = 0; i < cascadesCount_; ++i)
    {
        // Compute the corners of the frustum segment in view space.
        Vector4 corners[8];
        viewCamera->getFrustumCorners(cascades_[i].minDistance, corners);
        viewCamera->getFrustumCorners(cascades_[i].maxDistance, corners + 4);
        
        // Size the cascade camera to cover the frustum bounds.
        // Compute in view space so the size doesn't change during camera rotation.
        Bounds viewSpaceBounds = Bounds::cover(corners, 8);
        Vector3 viewSpaceSize = viewSpaceBounds.size();
        cascades_[i].camera.setOrthographicSize(viewSpaceSize.magnitude());
        
        // Calculate the shadow map centre in light space
        Vector4 viewSpaceCentre = Vector4(viewSpaceBounds.centre(), 1.0);
        Vector4 lightSpaceCentre = viewToLight * viewSpaceCentre;
        
        // Snap the centre to the nearest texel in light space.
        float texelSize = cascades_[i].camera.orthographicSize() / (float)resolution_;
        lightSpaceCentre.x = roundf(lightSpaceCentre.x / texelSize) * texelSize;
        lightSpaceCentre.y = roundf(lightSpaceCentre.y / texelSize) * texelSize;
        
        // Compute the cascade centre in world space
        Vector4 centre = lightToWorld * lightSpaceCentre;
        
        // The camera has a -ve near clip plane, so placing the camera in
        // the centre of the cascade will render the full cascade.
        cascades_[i].camera.setPosition(centre.vec3());
    }
}

void ShadowMap::updateUniformBuffer() const
{
    ShadowUniformBuffer shadowData;

    for(int i = 0; i < MaxCascades; ++i)
    {
        // Store the min distance
        float minDistance = cascades_[i].minDistance;
        float minDistanceSqr = minDistance * minDistance;
        shadowData.cascadeDistancesSqr[i] = minDistanceSqr;
        
        // Compute the world to shadow matrix
        shadowData.worldToShadow[i] = worldToShadowMatrix(i);
    }
    
    // Update the uniform block for all shaders
    uniformManager_->updateShadowBuffer(shadowData);
}

void ShadowMap::renderCascades()
{
    // Enable depth biasing to prevent shadow acne
    glPolygonOffset(2.5, 10.0);
    glEnable(GL_POLYGON_OFFSET_FILL);
    
    // Write to the depth buffer only.
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glColorMask(false, false, false, false);
    
    // Render each shadow cascade
    for(int c = 0; c < cascadesCount_; ++c)
    {
        // Use the cascade camera
        cascades_[c].camera.bind();
        
        // Only clear the shadow map if this is the first cascade being rendered
        shadowCasterPass_->setClearFlags(c == 0 ? GL_DEPTH_BUFFER_BIT : GL_NONE);
        
        // Render the scene using the camera.
        shadowCasterPass_->submit(&cascades_[c].camera, scene_->meshInstances());
    }
    
    // Disable depth biasing
    glDisable(GL_POLYGON_OFFSET_FILL);
}

float ShadowMap::getCascadeMin(int cascade, float farPlane) const
{
    // Ensure cascade 0 starts at distance 0
    if(cascade == 0)
    {
        return 0.0;
    }
    
    // Ensure the last cascade finished at farPlane
    // i.e. ensure the index after the last cascade starts at farPlane
    if(cascade == cascadesCount_)
    {
        return farPlane;
    }
    
    // Compute the linear and logarithmic distances
    float linearDistance = (farPlane / (float)cascadesCount_) * cascade;
    float logarithmicDistance = pow(farPlane, (1.0 / (float)cascadesCount_) * cascade);
    
    // Perform a weighted average of the 2 distances
    const float linearWeight = 0.3;
    const float logarithmicWeight = 1.0 - linearWeight;
    
    // Combine the 2 distances
    return (linearDistance * linearWeight) + (logarithmicDistance * logarithmicWeight);
}

float ShadowMap::getCascadeMax(int cascade, float farPlane) const
{
    // = min distance of next cascade
    return getCascadeMin(cascade + 1, farPlane);
}

Matrix4x4 ShadowMap::worldToShadowMatrix(int cascade) const
{
    // Determine the proportion of the screen the cascade is drawn to
    float width = 1.0 / cascadesCount_;
    
    // Offset matrix transforms coordinates from [-1 - 1] to [0-1]
    // and restricts to the cascade's viewport
    Matrix4x4 offsetMatrix;
    offsetMatrix.setRow(0, 0.5 * width, 0.0, 0.0, (0.5 / cascadesCount_) + (width * cascade));
    offsetMatrix.setRow(1, 0.0, 0.5, 0.0, 0.5);
    offsetMatrix.setRow(2, 0.0, 0.0, 0.5, 0.5);
    offsetMatrix.setRow(3, 0.0, 0.0, 0.0, 1.0);
    
    // Transform world coordinate to camera coordinates,
    // then to texture coordinates.
    return offsetMatrix * cascades_[cascade].camera.worldToCameraMatrix();
}
