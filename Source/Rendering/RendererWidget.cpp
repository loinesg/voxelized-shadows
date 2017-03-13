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
    delete shadowMap_;
    delete shadowMask_;
    
    // Delete render passes
    delete sceneDepthPass_;
    delete forwardPass_;
}

void RendererWidget::enableFeature(ShaderFeature feature)
{
    sceneDepthPass_->enableFeature(feature);
    forwardPass_->enableFeature(feature);
}

void RendererWidget::disableFeature(ShaderFeature feature)
{
    sceneDepthPass_->disableFeature(feature);
    forwardPass_->disableFeature(feature);
}

void RendererWidget::setOverlay(int overlayIndex)
{
    currentOverlay_ = overlayIndex;
}

void RendererWidget::setShadowMapResolution(int resolution)
{
    shadowMap_->setCascades(shadowMap_->cascadesCount(), resolution);
}

void RendererWidget::setShadowMapCascades(int cascades)
{
    shadowMap_->setCascades(cascades, shadowMap_->resolution());
}

void RendererWidget::initializeGL()
{
    printf("Initializing OpenGL %s \n", glGetString(GL_VERSION));

    // Configure OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    // Load the scene
    createScene();
    
    // Create uniform buffers
    uniformManager_ = new UniformManager();
    
    // Create assets
    shadowMap_ = new ShadowMap(scene_, uniformManager_, 2, 4096);
    voxelTree_ = new VoxelTree(uniformManager_, scene_, 4096);
    voxelTree_->build();
    shadowMask_ = new ShadowMask(uniformManager_);
    
    // Create RenderPass instances
    createRenderPasses();
    
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
    // Pass for rendering depth from the main camera
    // Depth only, so no features are needed except cutout.
    string sceneDepthPassName = "SceneDepthPass";
    sceneDepthPass_ = new RenderPass(sceneDepthPassName, uniformManager_);
    sceneDepthPass_->setSupportedFeatures(SF_Cutout);
    sceneDepthPass_->setClearFlags(GL_DEPTH_BUFFER_BIT);
    
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
    shadowMap_->updatePosition(scene_->mainCamera());
    
    // Update the shadow uniform buffer
    shadowMap_->updateUniformBuffer();
    
    // Render all cascades
    shadowMap_->renderCascades();
}

void RendererWidget::renderSceneDepth()
{
    // Use the main camera
    scene_->mainCamera()->bind();
    
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
    // Assign the input textures
    shadowMask_->setShadowMapTexture(shadowMap_->texture());
    shadowMask_->setSceneDepthTexture(sceneDepthTexture_);
    
    // Assign the voxel tree buffer
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_BUFFER, voxelTree_->treeBufferTexture());
    
    // Render the shadow mask
    shadowMask_->render();
}

void RendererWidget::renderForward()
{
    // Use the screen space shadow mask texture
    shadowMask_->texture()->bind(GL_TEXTURE3);
    
    // Only render fragments that passed the earlier depth prepass.
    // Write to colour, but not depth.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(true);
    glColorMask(true, true, true, true);
    
    // Use the main camera
    scene_->mainCamera()->bind();
        
    // Render the final image
    forwardPass_->submit(scene_->mainCamera(), scene_->meshInstances());
}
