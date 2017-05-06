#include "RenderPass.hpp"

RenderPass::RenderPass(const string &name, UniformManager* uniformManager)
    : name_(name),
    clearFlags_(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT),
    clearColor_(PassClearColor(0.0, 0.0, 0.0, 1.0)),
    shaderCollection_(new ShaderCollection(name)),
    uniformManager_(uniformManager)
{
    fullScreenQuad_ = Mesh::fullScreenQuad();
}

RenderPass::~RenderPass()
{
    delete shaderCollection_;
    delete fullScreenQuad_;
}

void RenderPass::setClearFlags(PassClearFlags clearFlags)
{
    clearFlags_ = clearFlags;
}

void RenderPass::setClearColor(PassClearColor color)
{
    clearColor_ = color;
}

void RenderPass::enableFeature(ShaderFeature feature)
{
    shaderCollection_->enableFeature(feature);
}

void RenderPass::disableFeature(ShaderFeature feature)
{
    shaderCollection_->disableFeature(feature);
}

void RenderPass::setSupportedFeatures(ShaderFeatureList supportedFeatures)
{
    shaderCollection_->setSupportedFeatures(supportedFeatures);
}

void RenderPass::submit(Camera* camera, const vector<MeshInstance*>* instances, bool drawStatic, bool drawDynamic)
{
    // Setup the camera uniform buffer
    CameraUniformBuffer cub;
    cub.screenResolution = Vector4(camera->pixelWidth(), camera->pixelHeight(), 0.0, 0.0);
    cub.cameraPosition = Vector4(camera->position(), 1.0);
    cub.viewProjection = camera->worldToCameraMatrix();
    cub.clipToWorld = camera->cameraToWorldMatrix();
    uniformManager_->updateCameraBuffer(cub);
    
    // Clear the screen
    glClearColor(clearColor_.x, clearColor_.y, clearColor_.z, clearColor_.w);
    glClear(clearFlags_);
    
    // Remember the previously used state
    // Used to skip needless state changes
    ShaderFeatureList prevShaderFeatures = ~0;
    Texture* prevTexture = NULL;
    Texture* prevNormalMap = NULL;
    Mesh* prevMesh = NULL;
    
    for(unsigned int i = 0; i < instances->size(); ++i)
    {
        MeshInstance* instance = (*instances)[i];
        
        // Check if the instance should be skipped due to its static flag
        if((instance->isStatic() && !drawStatic)
           || (!instance->isStatic() && !drawDynamic))
        {
            continue;
        }
        
        ShaderFeatureList shaderFeatures = instance->shaderFeatures();
        Texture* texture = instance->texture();
        Texture* normalMap = instance->normalMap();
        Mesh* mesh = instance->mesh();
        Matrix4x4 transform = instance->localToWorld();
        
        // Bind the correct shader
        if(shaderFeatures != prevShaderFeatures)
            shaderCollection_->getVariant(shaderFeatures)->bind();
        
        // Bind the correct main texture
        if(texture != prevTexture)
            texture->bind(GL_TEXTURE0);
        
        // Bind the correct normal map texture
        if(normalMap != prevNormalMap)
            normalMap->bind(GL_TEXTURE1);
            
        // Bind the correct mesh
        if(mesh != prevMesh)
            mesh->bind();
        
        // Apply per object uniform data
        PerObjectUniformBuffer data;
        data.localToWorld = transform;
        uniformManager_->updatePerObjectBuffer(data);
        
        // Draw the mesh
        glDrawElements(GL_TRIANGLES, mesh->elementsCount(), GL_UNSIGNED_SHORT, (void*)0);
        
        prevShaderFeatures = shaderFeatures;
        prevTexture = texture;
        prevNormalMap = normalMap;
        prevMesh = mesh;
    }
}

void RenderPass::renderFullScreen()
{
    // Use all supported shader features
    shaderCollection_->getVariant(~0)->bind();
    
    // Use the quad mesh
    fullScreenQuad_->bind();

    // Draw the quad
    glDrawElements(GL_TRIANGLES, fullScreenQuad_->elementsCount(), GL_UNSIGNED_SHORT, (void*)0);
}
