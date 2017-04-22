#include "VoxelTree.hpp"

#include <assert.h>
#include <math.h>

VoxelTree::VoxelTree(UniformManager* uniformManager, const Scene* scene, int resolution)
    : uniformManager_(uniformManager),
    scene_(scene),
    startedTiles_(0),
    mergedTiles_(0),
    uploadedTiles_(0),
    treeResolution_(resolution),
    shadowMap_(scene, uniformManager, 1, 4),
    voxelWriter_(),
    activeTiles_(),
    activeTilesMutex_()
{
    // Make each tile as small as possible.
    tileResolution_ = 4096;
    while(totalTiles() > MaxTileCount)
    {
        // Double the resolution until under the tile count limit.
        tileResolution_ *= 2;
    }
    
    // Each tile must be at least 8x8 so that leaf masks can be used
    // and no more than 16K (maximum texture resolution)
    assert(tileResolution_ >= 8);
    assert(tileResolution_ <= 16384);
    
    // Set the correct shadow map resolution
    shadowMap_.setCascades(1, tileResolution_);
    
    // For now, use a dummy tree consisting of a single, fully
    // unshadowed inner node
    VoxelInnerNode node;
    node.childMask = 21845; // = 0101010101010101 = 8 Unshadowed children
    VoxelPointer nodePtr = voxelWriter_.writeNode(node, 0, 0);
    
    // Set the dummy node as the root for every tile
    for(int i = 0; i < MaxTileCount; ++i)
    {
        treePointers_[i] = nodePtr;
    }
    
    // Create the buffer to hold the tree
    glGenBuffers(1, &buffer_);
    glBindBuffer(GL_TEXTURE_BUFFER, buffer_);
    glGenTextures(1, &bufferTexture_);
    glBindTexture(GL_TEXTURE_BUFFER, bufferTexture_);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_R32UI, buffer_);
    
    // Set the initial buffer values
    updateBuffers();
    
    // Start the tile merging thread
    mergingThread_ = thread(&VoxelTree::mergeTiles, this);
}

size_t VoxelTree::sizeBytes() const
{
    return voxelWriter_.dataSizeBytes();
}

size_t VoxelTree::sizeMB() const
{
    return sizeBytes() / (1024 * 1024);
}

size_t VoxelTree::originalSizeBytes() const
{
    // Using 3 bytes -> 24 bits per pixel
    return treeResolution_ * treeResolution_ * 3;
}

size_t VoxelTree::originalSizeMB() const
{
    return originalSizeBytes() / (1024 * 1024);
}

void VoxelTree::updateBuild()
{
    // Start another tile build if the limit is not currently met
    int activeTiles = startedTiles_ - mergedTiles_;
    if(activeTiles < ConcurrentBuilds && startedTiles_ < totalTiles())
    {
        startTileBuild();
    }
    
    // Reupload the tree to the gpu if more tiles have finished
    if(uploadedTiles_ < mergedTiles_)
    {
        updateBuffers();
    }
}

void VoxelTree::startTileBuild()
{
    int tileIndex = startedTiles_;
    startedTiles_ ++;
    
    // Compute the light space bounds of the tile
    Bounds bounds = tileBoundsLightSpace(tileIndex);
    
    // Get the entry and exit depths for the tile by rendering
    // a dual shadow map.
    float* entryDepths;
    float* exitDepths;
    computeDualShadowMaps(bounds, &entryDepths, &exitDepths);
    
    // Create the builder.
    VoxelBuilder* builder = new VoxelBuilder(tileIndex, tileResolution_, entryDepths, exitDepths);
    
    // Add to the active tiles list
    activeTilesMutex_.lock();
    activeTiles_.push_back(builder);
    activeTilesMutex_.unlock();
}

void VoxelTree::mergeTiles()
{
    // Keep looking for tiles to merge until finished
    while(mergedTiles_ < totalTiles())
    {
        // Look for a finished builder
        activeTilesMutex_.lock();
        VoxelBuilder* builder = findFinishedBuilder();
        activeTilesMutex_.unlock();
        
        if(builder == NULL)
        {
            continue;
        }
        
        // Gather the subtree information
        int tile = builder->tileIndex();
        uint32_t* subtree = (uint32_t*)builder->tree();
        VoxelPointer subtreeRoot = builder->rootAddress();

        // Write the tree to the combined tree and store the root node location
        VoxelPointer ptr = voxelWriter_.writeTree(subtree, subtreeRoot, tileResolution_);
        treePointers_[tile] = ptr;
        
        // The builder is no longer needed
        delete builder;
        
        // Update the merged tiles count
        mergedTiles_ ++;
    }
}

VoxelBuilder* VoxelTree::findFinishedBuilder()
{
    // Look for a builder that has finished
    for(unsigned int i = 0; i < activeTiles_.size(); ++i)
    {
        // Get the builder
        VoxelBuilder* builder = activeTiles_[i];
        
        // Check the state is Done
        if(builder->buildState() == VoxelBuilderState::Done)
        {
            // Remove the builder from the active list
            std::swap(activeTiles_[i], activeTiles_.back());
            activeTiles_.pop_back();
            
            return builder;
        }
    }
    
    // No builders are finished
    return NULL;
}

void VoxelTree::updateBuffers()
{
    updateUniformBuffer();
    updateTreeBuffer();
}

void VoxelTree::updateUniformBuffer()
{
    // Cover the scene witht the shadowmap and get the world to shadow matrix
    shadowMap_.setLightSpaceBounds(sceneBoundsLightSpace());
    Matrix4x4 worldToShadow = shadowMap_.worldToShadowMatrix(0);
    
    // Scale the world to shadow matrix by the total voxel resolution
    Vector3 scale;
    scale.x = treeResolution_;
    scale.y = treeResolution_;
    scale.z = tileResolution_; // The trees are only tiled in x and y
    worldToShadow = Matrix4x4::scale(scale) * worldToShadow;
    
    // Update the uniform buffer
    VoxelsUniformBuffer buffer;
    buffer.worldToVoxels = worldToShadow;
    buffer.voxelTreeHeight = log2(tileResolution_);
    buffer.tileSubdivisions = tileSubdivisions();
    
    for(int i = 0; i < totalTiles(); ++i)
    {
        buffer.rootAddresses[i*4] = treePointers_[i];
    }
    
    uniformManager_->updateVoxelBuffer(&buffer, sizeof(VoxelsUniformBuffer));
}

void VoxelTree::updateTreeBuffer()
{
    // Update the uploaded tiles count
    uploadedTiles_ = mergedTiles_;
    
    // Get the current tree data
    const void* treeData = voxelWriter_.data();
    size_t treeSizeBytes = voxelWriter_.dataSizeBytes();
    
    // Create the buffer to hold the tree
    glBufferData(GL_TEXTURE_BUFFER, treeSizeBytes, treeData, GL_STATIC_DRAW);
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
    const vector<MeshInstance*>* instances = scene_->meshInstances();
    for(unsigned int i = 0; i < instances->size(); ++i)
    {
        // Get the mesh instance
        MeshInstance* instance = (*instances)[i];
        
        // Skip objects that are not static
        if(instance->isStatic() == false)
        {
            continue;
        }
        
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

Bounds VoxelTree::tileBoundsLightSpace(int index) const
{
    // Compute the bounds of the entire scene in light space
    Bounds sceneBounds = sceneBoundsLightSpace();
    
    // Compute the light space size of each tile
    float tileSizeX = sceneBounds.size().x / tileSubdivisions();
    float tileSizeY = sceneBounds.size().y / tileSubdivisions();
    
    // Get the x and y position of the tile
    int x = index / tileSubdivisions();
    int y = index % tileSubdivisions();
    
    // Determine the light space bounds of the tile
    float posX = sceneBounds.min().x + (tileSizeX * x);
    float posY = sceneBounds.min().y + (tileSizeY * y);
    Vector3 boundsMin(posX, posY, sceneBounds.min().z);
    Vector3 boundsMax(posX + tileSizeX, posY + tileSizeY, sceneBounds.max().z);

    return Bounds(boundsMin, boundsMax);
}

void VoxelTree::computeDualShadowMaps(const Bounds &bounds, float** entryDepths, float** exitDepths)
{
    // Set the shadow map to cover the correct area
    shadowMap_.setLightSpaceBounds(bounds);
    
    // Render the shadow map with static but not dynamic objects
    shadowMap_.renderCascades(true, false);
    
    // Store the depths as the shadow entry depths
    *entryDepths = new float[tileResolution_ * tileResolution_];
    glReadPixels(0, 0, tileResolution_, tileResolution_, GL_DEPTH_COMPONENT, GL_FLOAT, *entryDepths);
    
    // Render the shadow map back faces
    glCullFace(GL_FRONT);
    shadowMap_.renderCascades(true, false); // Static objects only
    glCullFace(GL_BACK);
    
    // Store the depths as the shadow exit depths
    *exitDepths = new float[tileResolution_ * tileResolution_];
    glReadPixels(0, 0, tileResolution_, tileResolution_, GL_DEPTH_COMPONENT, GL_FLOAT, *exitDepths);
}
