#include "VoxelTree.hpp"

#include <assert.h>
#include <math.h>

#include <QElapsedTimer>

#include "ShadowMap.hpp"

VoxelTree::VoxelTree(UniformManager* uniformManager, const Scene* scene, int resolution)
    : uniformManager_(uniformManager),
    scene_(scene),
    resolution_(resolution)
{
    // Resolution must be at least 8x8 so that leaf masks can be used
    assert(resolution >= 8);
}

VoxelTree::~VoxelTree()
{
    
}

void VoxelTree::build()
{
    QElapsedTimer timer;
    timer.start();
    
    // Create the depth map using dual shadow maps
    // rendered from the scene.
    VoxelDepthMap depthMap = computeDualShadowMaps();

    printf("-- Building Depth Hierarchy took %lld ms \n", timer.restart());
    
    // Create a single builder
    VoxelBuilder builder(&depthMap);
    builder.build();
    
    printf("-- Building Shadow Tree took %lld ms \n", timer.restart());
    
    // Create the buffer
    glGenBuffers(1, &buffer_);
    glBindBuffer(GL_TEXTURE_BUFFER, buffer_);
    glBufferData(GL_TEXTURE_BUFFER, builder.treeSizeBytes(), builder.tree(), GL_STATIC_DRAW);
    
    // Create the buffer texture
    glGenTextures(1, &bufferTexture_);
    glBindTexture(GL_TEXTURE_BUFFER, bufferTexture_);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_R32UI, buffer_);
    
    printf("-- Transfer To GPU took %lld ms \n", timer.restart());
    
    // Update the uniform buffer
    VoxelsUniformBuffer buffer;
    buffer.worldToVoxels = worldToShadow_;
    buffer.voxelTreeHeight = log2(resolution_);
    buffer.rootAddress = builder.rootAddress();
    uniformManager_->updateVoxelBuffer(&buffer, sizeof(VoxelsUniformBuffer));
    
    // Calculate the uncompressed and compressed sizes
    size_t uncompressedB = resolution_ * resolution_ * 3; // 3 Bytes per depth sample
    size_t uncompressedKB = uncompressedB / 1024;
    size_t uncompressedMB = uncompressedKB / 1024;
    size_t sizeB = builder.treeSizeBytes();
    size_t sizeKB = sizeB / 1024;
    size_t sizeMB = sizeKB / 1024;
    
    // Output build results for debugging
    printf("Resolution: %d x %d \n", resolution_, resolution_);
    printf("  Original: %luB (%luKB, %luMB) \n", uncompressedB, uncompressedKB, uncompressedMB);
    printf("Voxel Size: %luB (%luKB, %luMB) \n", sizeB, sizeKB, sizeMB);
}

Bounds VoxelTree::sceneBoundsLightSpace() const
{
    // Get the world to light space transformation matrix (without translation)
    Matrix4x4 worldToLight = scene_->mainLight()->worldToLocal();
    worldToLight.set(0, 3, 0.0);
    worldToLight.set(1, 3, 0.0);
    worldToLight.set(2, 3, 0.0);
    
    // Create a Bounds containing the origin only.
    Bounds bounds = Bounds(Vector3::zero(), Vector3::zero());
    
    // Expand the scene bounds to cover each mesh
    const vector<MeshInstance>* instances = scene_->meshInstances();
    for(auto instance = instances->begin(); instance != instances->end(); ++instance)
    {
        // Get the model to light transformation
        Matrix4x4 modelToLight = worldToLight * instance->localToWorld();
        
        Mesh* mesh = instance->mesh();
        for(int v = 0; v < mesh->verticesCount(); ++v)
        {
            // Convert each point to light space
            Vector4 modelPos = Vector4(mesh->vertices()[v], 1.0);
            Vector4 lightPos = modelToLight * modelPos;
            
            // Ensure the bounds cover the vertex
            bounds.expandToCover(lightPos.vec3());
        }
    }
    
    // Return the final bounds
    return bounds;
}

VoxelDepthMap VoxelTree::computeDualShadowMaps()
{
    // Resolution must be set
    assert(resolution_ > 0);
    
    // Create a single shadow map cascade to cover the entire scene
    ShadowMap shadowMap(scene_, uniformManager_, 1, resolution_);
    shadowMap.setLightSpaceBounds(sceneBoundsLightSpace());
    
    // Render the shadow map normally
    shadowMap.renderCascades();
    
    // Store the depths as the shadow entry depths
    float* entryDepths = new float[resolution_ * resolution_];
    glReadPixels(0, 0, resolution_, resolution_, GL_DEPTH_COMPONENT, GL_FLOAT, entryDepths);
    
    // Render the shadow map back faces
    glCullFace(GL_FRONT);
    shadowMap.renderCascades();
    glCullFace(GL_BACK);
    
    // Store the depths as the shadow exit depths
    float* exitDepths = new float[resolution_ * resolution_];
    glReadPixels(0, 0, resolution_, resolution_, GL_DEPTH_COMPONENT, GL_FLOAT, exitDepths);

    // Store the new world to shadow matrix scaled by the voxel resolution
    Matrix4x4 scaleMat = Matrix4x4::scale(Vector3(resolution_, resolution_, resolution_));
    Matrix4x4 transMat = shadowMap.worldToShadowMatrix(0);
    worldToShadow_ = scaleMat * transMat;
    
    // Create the depth map
    return VoxelDepthMap(resolution_, entryDepths, exitDepths);
}
