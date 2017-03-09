#include "RendererWidget.hpp"

#include <assert.h>
#include <cstdio>

#include <iostream>

RendererWidget::RendererWidget(const QGLFormat &format)
    : QGLWidget(format),
    overlays_(),
    currentOverlay_(-1)
{
    sceneDepthTexture_ = NULL;
}

RendererWidget::~RendererWidget()
{
    delete scene_;
    delete uniformManager_;
    delete fullScreenQuad_;
    delete shadowMap_;
    delete shadowMask_;
    delete shadowCasterPass_;
    delete sceneDepthPass_;
    delete forwardPass_;
}

void RendererWidget::enableFeature(ShaderFeature feature)
{
    shadowCasterPass_->enableFeature(feature);
    sceneDepthPass_->enableFeature(feature);
    shadowMaskPass_->enableFeature(feature);
    forwardPass_->enableFeature(feature);
}

void RendererWidget::disableFeature(ShaderFeature feature)
{
    shadowCasterPass_->disableFeature(feature);
    sceneDepthPass_->disableFeature(feature);
    shadowMaskPass_->disableFeature(feature);
    forwardPass_->disableFeature(feature);
}

void RendererWidget::setOverlay(int overlayIndex)
{
    currentOverlay_ = overlayIndex;
}

void RendererWidget::setShadowMapResolution(int resolution)
{
    shadowMap_->setResolution(resolution);
}

void RendererWidget::setShadowMapCascades(int cascades)
{
    shadowMap_->setCascadesCount(cascades);
}

void RendererWidget::initializeGL()
{
    printf("Initializing OpenGL %s \n", glGetString(GL_VERSION));

    // Configure OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    // Create uniform buffers
    uniformManager_ = new UniformManager();
    
    // Create assets
    fullScreenQuad_ = Mesh::fullScreenQuad();
    shadowMap_ = new ShadowMap(uniformManager_, 4096, 2);
    shadowMask_ = new ShadowMask();
    
    // Create RenderPass instances
    createRenderPasses();
    
    // Load the scene
    createScene();
    
    glGenFramebuffers(1, &sceneDepthFBO_);
    
    sceneDepthTexture_ = Texture::depth(1, 1);
    sceneDepthTexture_->setWrapMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    sceneDepthTexture_->setMagFilter(GL_NEAREST);
    sceneDepthTexture_->setMinFilter(GL_NEAREST);
    
    glBindFramebuffer(GL_FRAMEBUFFER, sceneDepthFBO_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sceneDepthTexture_->id(), 0);
    
    // Create debug overlays
    // This is done last so overlays can reference other assets
    createOverlays();
}

void RendererWidget::resizeGL(int w, int h)
{
    // Update the main camera to match the new resolution
    camera()->setPixelWidth(w);
    camera()->setPixelHeight(h);
    
    // Make the shadow mask the same resolution
    shadowMask_->setResolution(w, h);
    
    // Make the scene depth texture the same resolution
    sceneDepthTexture_->setResolution(w, h);
}

void RendererWidget::paintGL()
{
    // Update scene uniform buffer
    SceneUniformBuffer data;
    data.screenResolution = Vector4(camera()->pixelWidth(), camera()->pixelHeight(), 0.0, 0.0);
    data.cameraPosition = Vector4(scene_->mainCamera()->position(), 1.0);
    data.clipToWorld = camera()->cameraToWorldMatrix();
    data.ambientLightColor = Vector4(scene_->mainLight()->ambient(), 1.0);
    data.lightColor = Vector4(scene_->mainLight()->color(), 1.0);
    data.lightDirection = -1.0 * scene_->mainLight()->forward();
    uniformManager_->updateSceneBuffer(data);
    
    // Render shadow depth to the shadow map framebuffer.
    renderShadowMap();
    
    // Render scene depth to the main framebuffer.
    renderSceneDepth();
    
    // Render the screen space shadow mask
    // using the shadow map and scene depth.
    renderShadowMask();
    
    // Final forward pass.
    renderForward();
    
    // Draw debug overlay
    if(currentOverlay_ != -1)
    {
        overlays_[currentOverlay_]->draw(camera());
    }
    
    // Schedule a redraw immediately
    update();
    
    // Print errors
    GLenum error;
    while((error = glGetError()) != GL_NO_ERROR)
    {
        printf("GLError %x \n", error);
    }
}

void RendererWidget::createRenderPasses()
{
    // Pass for rendering the shadow map.
    // Depth only, so no features are needed except cutout.
    string shadowCasterPassName = "ShadowCasterPass";
    shadowCasterPass_ = new RenderPass(shadowCasterPassName, uniformManager_);
    shadowCasterPass_->setSupportedFeatures(SF_Cutout);
    shadowCasterPass_->setClearFlags(GL_DEPTH_BUFFER_BIT);
    
    // Pass for rendering depth from the main camera
    // Depth only, so no features are needed except cutout.
    string sceneDepthPassName = "SceneDepthPass";
    sceneDepthPass_ = new RenderPass(sceneDepthPassName, uniformManager_);
    sceneDepthPass_->setSupportedFeatures(SF_Cutout);
    sceneDepthPass_->setClearFlags(GL_DEPTH_BUFFER_BIT);
    
    // Pass for sampling the shadow map into the screen space shadow mask.
    string shadowSamplingPassName = "ShadowSamplingPass";
    shadowMaskPass_ = new RenderPass(shadowSamplingPassName, uniformManager_);
    shadowMaskPass_->setSupportedFeatures(~0);
    
    // Pass for rendering the final image.
    // Uses all features.
    string forwardPassName = "ForwardPass";
    forwardPass_ = new RenderPass(forwardPassName, uniformManager_);
    forwardPass_->setSupportedFeatures(~0);
    forwardPass_->setClearColor(PassClearColor(0.78, 0.93, 0.92, 1.0));
}

void RendererWidget::createScene()
{
    scene_ = new Scene();
    scene_->loadFromFile("scene.scene");
}

void RendererWidget::createOverlays()
{
    // Shadow Map overlay
    Overlay* shadowMapOverlay = new Overlay("Shadow Map", "DebugOverlay", SF_Debug_ShadowMapTexture);
    shadowMapOverlay->setFullScreen(false);
    shadowMapOverlay->setUseBlending(false);
    shadowMapOverlay->setTexture(shadowMap_->texture());
    overlays_.push_back(shadowMapOverlay);
    
    // Scene depth overlay
    Overlay* sceneDepthOverlay = new Overlay("Scene Depth", "DebugOverlay", SF_Debug_DepthTexture);
    sceneDepthOverlay->setFullScreen(true);
    sceneDepthOverlay->setUseBlending(false);
    sceneDepthOverlay->setTexture(sceneDepthTexture_);
    overlays_.push_back(sceneDepthOverlay);
    
    // Shadow mask overlay
    Overlay* shadowMaskOverlay = new Overlay("Shadow Mask", "DebugOverlay", 0);
    shadowMaskOverlay->setFullScreen(true);
    shadowMaskOverlay->setUseBlending(false);
    shadowMaskOverlay->setTexture(shadowMask_->texture());
    overlays_.push_back(shadowMaskOverlay);
    
    // Cascade Splits overlay
    Overlay* cascadeSplitsOverlay = new Overlay("Cascade Splits", "ShadowSamplingPass", SF_Debug_ShowCascadeSplits);
    cascadeSplitsOverlay->setFullScreen(true);
    cascadeSplitsOverlay->setUseBlending(true);
    cascadeSplitsOverlay->setTexture(sceneDepthTexture_);
    overlays_.push_back(cascadeSplitsOverlay);
}

void RendererWidget::renderShadowMap()
{
    // Update shadow map position
    shadowMap_->updatePosition(scene_->mainCamera(), scene_->mainLight());
    
    // Update the shadow uniform buffer
    shadowMap_->updateUniformBuffer();
    
    // Enable depth biasing to prevent shadow acne
    glPolygonOffset(2.5, 10.0);
    glEnable(GL_POLYGON_OFFSET_FILL);
    
    // Write to the depth buffer only.
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glColorMask(false, false, false, false);
    
    // Render each shadow cascade
    for(int c = 0; c < shadowMap_->cascadesCount(); ++c)
    {
        // Use the camera for the cascade
        useCamera(shadowMap_->getCamera(c));
        
        // Only clear the shadow map if this is the first cascade being rendered
        shadowCasterPass_->setClearFlags(c == 0 ? GL_DEPTH_BUFFER_BIT : GL_NONE);
        
        // Render the scene using the camera.
        shadowCasterPass_->submit(shadowMap_->getCamera(c), scene_->meshInstances());
    }
    
    // Disable depth biasing
    glDisable(GL_POLYGON_OFFSET_FILL);
}

void RendererWidget::renderSceneDepth()
{
    // Use the main camera
    useCamera(scene_->mainCamera());
    
    // Write to the depth framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, sceneDepthFBO_);
    
    // Write to the depth buffer only.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(true);
    glColorMask(false, false, false, false);
    
    // Render the scene depth map
    sceneDepthPass_->submit(scene_->mainCamera(), scene_->meshInstances());
}

void RendererWidget::renderShadowMask()
{
    // Render in full screen
    int screenWidth = camera()->pixelWidth();
    int screenHeight = camera()->pixelHeight();
    glViewport(0, 0, screenWidth, screenHeight);
    
    // Bind the shadow mask framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMask_->frameBuffer());
    
    // Dont depth test, and write to color only.
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);
    glColorMask(true, true, true, true);
    
    // Bind the shadow map texture
    shadowMap_->bindTexture(GL_TEXTURE2);
    
    // Bind the scene depth texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneDepthTexture_->id());
    
    // Render the shadow mask
    shadowMaskPass_->renderFullScreen(0, fullScreenQuad_);
}

void RendererWidget::renderForward()
{
    // Use the screen space shadow mask texture
    shadowMask_->bindTexture(GL_TEXTURE3);
    
    // Only render fragments that passed the earlier depth prepass.
    // Write to colour, but not depth.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(true);
    glColorMask(true, true, true, true);
    
    // Render the final image
    useCamera(scene_->mainCamera());
    forwardPass_->submit(scene_->mainCamera(), scene_->meshInstances());
}

void RendererWidget::useCamera(Camera* camera)
{
    // Bind the correct framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, camera->framebuffer());
    
    // Update the viewport to match the camera width/height
    glViewport(camera->pixelOffsetX(), camera->pixelOffsetY(), camera->pixelWidth(), camera->pixelHeight());
}
