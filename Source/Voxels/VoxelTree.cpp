#include "VoxelTree.hpp"

#include <assert.h>
#include <math.h>

#include <QElapsedTimer>

VoxelTree::VoxelTree(UniformManager* uniformManager, const Scene* scene, int resolution)
    : uniformManager_(uniformManager),
    scene_(scene),
    sceneBoundsLightSpace_(computeSceneBoundsLightSpace()),
    buildTimer_(),
    pcfKernelSize_(9),
    startedTiles_(0),
    mergedTiles_(0),
    uploadedTiles_(0),
    treeResolution_(resolution),
    shadowMap_(scene, uniformManager, 1, 4),
    voxelWriter_(),
    activeTiles_(),
    activeTilesMutex_()
{
    buildTimer_.start();
    
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
    
    // Create the root pointers in the buffer
    voxelWriter_.reserveRootNodePointerSpace(totalTiles());
    
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
    
    // Add every tile to the list of tiles to build
    for(int tile = 0; tile < totalTiles(); ++tile)
    {
        notStartedTiles_.push_back(tile);
    }
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
    return (size_t)treeResolution_ * (size_t)treeResolution_ * 3;
}

size_t VoxelTree::originalSizeMB() const
{
    return originalSizeBytes() / (1024 * 1024);
}

void VoxelTree::setPCFFilterSize(int kernelSize)
{
    // Must be either 9 or 17
    assert(kernelSize == 9 || kernelSize == 17);
    
    pcfKernelSize_ = kernelSize;
    
    // Recompute PCF kernel data
    updateUniformBuffer();
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
        
        // Output build stats if now finished
        if(uploadedTiles_ == totalTiles())
        {
            auto time = buildTimer_.elapsed();
            printf("Tree construction finished in %lld ms \n", time);
        }
    }
}

void VoxelTree::startTileBuild()
{
    int tileIndex = getNextTileToStart();
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

int VoxelTree::getNextTileToStart()
{
    // Check there are tiles waiting to be started
    assert(notStartedTiles_.empty() == false);
    
    // Get the world to light space transformation matrix (without translation)
    Matrix4x4 worldToLight = scene_->mainLight()->worldToLocal();
    worldToLight.set(0, 3, 0.0);
    worldToLight.set(1, 3, 0.0);
    worldToLight.set(2, 3, 0.0);
    
    // Get the camera position in light space
    Vector4 cameraPosWorld = Vector4(scene_->mainCamera()->position(), 1.0);
    Vector3 cameraPosLight = (worldToLight * cameraPosWorld).vec3();
    
    // Keep track of the best tile
    float closestDistance = 1000000000000.0;
    int highestTileIndex = 0;
    
    // Check each tile
    for(unsigned int i = 0; i < notStartedTiles_.size(); ++i)
    {
        // Get the tile centre in light space
        Bounds tileBounds = tileBoundsLightSpace(notStartedTiles_[i]);
        Vector3 tileCentre = tileBounds.centre();
        
        // Get the camera to tile centre sqr distance
        float distance = (cameraPosLight - tileCentre).sqrMagnitude();
        
        // Check if this tile is the new closest one
        if(distance < closestDistance)
        {
            closestDistance = distance;
            highestTileIndex = i;
        }
    }
    
    // Remove the tile from the queue + return it
    int tileIndex = notStartedTiles_[highestTileIndex];
    std::swap(notStartedTiles_[highestTileIndex], notStartedTiles_.back());
    notStartedTiles_.pop_back();
    return tileIndex;
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
        voxelWriter_.setRootNodePointer(tile, ptr);
        
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
    shadowMap_.setLightSpaceBounds(sceneBoundsLightSpace_);
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
    buffer.pcfSampleCount = pcfKernelSize_ * pcfKernelSize_;
    buffer.pcfLookups = ((pcfKernelSize_ + 7) / 8) * ((pcfKernelSize_ + 7) / 8);
    
    // Precompute PCF offsets and bitmasks
    for(int i = 0; i < 64; ++i)
    {
        // Get the x and y coords
        int x = i / 8;
        int y = i % 8;
        
        // Find the leaves used in the lookup
        int lookupIndex = 0;
        
        // Consider a range of possible offsets
        // Always offset by a full leaf tile (8 places).
        for(int xOffset = -32; xOffset <= 32; xOffset+= 8)
        {
            for(int yOffset = -32; yOffset <= 32; yOffset+= 8)
            {
                // Get the bitmask at this offset
                uint64_t bitmask = pcfBitmask(x - xOffset, y - yOffset);
                
                if(bitmask != 0)
                {
                    // Bitmask is non-zero. Include it in the filter kernel.
                    auto offsetData = &buffer.pcfOffsets[i*9 + lookupIndex];
                    offsetData->xOffset = xOffset + 20;
                    offsetData->yOffset = yOffset + 20;
                    offsetData->bitmaskHigh = bitmask >> 32;
                    offsetData->bitmaskLow = (uint32_t)bitmask;
                    
                    lookupIndex ++;
                }
            }
        }
        
        // There should be the same number of lookups for each leaf index.
        assert(lookupIndex == (int)buffer.pcfLookups);
    }
    
    uniformManager_->updateVoxelBuffer(&buffer, sizeof(VoxelsUniformBuffer));
}

uint64_t VoxelTree::pcfBitmask(int kernelX, int kernelY) const
{
    uint64_t bitmask = 0;
    for(int i = 0; i < 64; ++i)
    {
        // Get the x and y coords
        int coordX = i / 8;
        int coordY = i % 8;
        
        // Compute whether the coord is in the filter kernel
        if(coordX >= kernelX - pcfKernelSize_/2 && coordX <= kernelX + pcfKernelSize_/2
           && coordY >= kernelY - pcfKernelSize_/2 && coordY <= kernelY + pcfKernelSize_/2)
        {
            bitmask |= ((uint64_t)1) << i;
        }
    }
    
    return bitmask;
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

Bounds VoxelTree::computeSceneBoundsLightSpace() const
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
    // Get the bounds of the entire scene in light space
    Bounds sceneBounds = sceneBoundsLightSpace_;
    
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
    // Do not use depth biasing.
    shadowMap_.renderCascades(true, false, false);
    
    // Store the depths as the shadow entry depths
    *entryDepths = new float[tileResolution_ * tileResolution_];
    glReadPixels(0, 0, tileResolution_, tileResolution_, GL_DEPTH_COMPONENT, GL_FLOAT, *entryDepths);
    
    // Render the shadow map back faces
    glCullFace(GL_FRONT);
    shadowMap_.renderCascades(true, false, false); // Static objects only
    glCullFace(GL_BACK);
    
    // Store the depths as the shadow exit depths
    *exitDepths = new float[tileResolution_ * tileResolution_];
    glReadPixels(0, 0, tileResolution_, tileResolution_, GL_DEPTH_COMPONENT, GL_FLOAT, *exitDepths);
}
