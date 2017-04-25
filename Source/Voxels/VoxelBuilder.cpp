#include "VoxelBuilder.hpp"

#include <assert.h>
#include <cstdio>
#include <cstring>

VoxelBuilder::VoxelBuilder(int tileIndex, int resolution, float* entryDepths, float* exitDepths)
    : tileIndex_(tileIndex),
    resolution_(resolution),
    entryDepths_(entryDepths),
    exitDepths_(exitDepths),
    buildState_(VoxelBuilderState::Building),
    depthMap_(NULL),
    writer_(NULL),
    leafCache_(NULL)
{
    // Start the build thread
    buildThread_ = std::thread(&VoxelBuilder::build, this);
}

VoxelBuilder::~VoxelBuilder()
{
    // Ensure the build thread has finished
    buildThread_.join();
    
    // Delete the depth map
    if(depthMap_ != NULL)
    {
        delete depthMap_;
    }
    
    // Delete the writer
    if(writer_ != NULL)
    {
        delete writer_;
    }
    
    // Delete the leaf cache
    if(leafCache_ != NULL)
    {
        delete[] leafCache_;
    }
}

void VoxelBuilder::build()
{
    // Create the building objects
    createDepthMap();
    createWriter();
    createLeafCache();
    
    // The root tile covers the entire region.
    VoxelTile root;
    root.x = 0;
    root.y = 0;
    root.z = 0;
    root.width = depthMap_->resolution();
    root.depth = depthMap_->resolution();
    
    // Process the root tile
    // This recursively processes all tiles
    uint64_t hash;
    rootAddress_ = processTile(root, &hash);
    
    // The depth map is no longer needed
    delete depthMap_;
    depthMap_ = NULL;
    
    // The leaf cache is no longer needed
    delete[] leafCache_;
    leafCache_ = NULL;
    
    // The writer *is* still needed, as it contains the built tree.
    
    // Update the build state
    buildState_ = VoxelBuilderState::Done;
}

void VoxelBuilder::createDepthMap()
{
    // The constructor builds the depth hierarchy.
    // This is slow so should be run from the builder thread.
    depthMap_ = new VoxelDepthMap(resolution_, entryDepths_, exitDepths_);
}

void VoxelBuilder::createWriter()
{
    // The writer will store the created nodes.
    writer_ = new VoxelWriter();
}

void VoxelBuilder::createLeafCache()
{
    // Create the leaf cache.
    // There is one cache per 8x8 tile in the depth map.
    int leafTileCount = (resolution_ / 8) * (resolution_ / 8);
    leafCache_ = new VoxelLeafCache[leafTileCount];
    
    // Set each tile's change distance to 0 so they will be computed on first use
    std::memset(leafCache_, 0, leafTileCount * sizeof(VoxelLeafCache));
}

VoxelPointer VoxelBuilder::processTile(const VoxelTile &tile, VoxelNodeHash* hash)
{
    if(tile.depth == 1)
    {
        // Treat as a leaf tile if it is an 8x8x1 block
        return processLeafTile(tile, hash);
    }
    else
    {
        // Otherwise treat as a normal inner tile
        return processInnerTile(tile, hash);
    }
}

VoxelPointer VoxelBuilder::processInnerTile(const VoxelTile &tile, VoxelNodeHash* hash)
{
    // The tile should be a cube of at least size 8
    assert(tile.width >= 8);
    assert(tile.width == tile.depth);
    
    // Get the child locations
    VoxelTile children[8];
    getChildLocations(tile, children);
    
    // Create the node
    VoxelInnerNode node;
    
    // Get the child mask
    node.childMask = depthMap_->sampleChildMask(children);
    
    // Store the hash for each child node
    VoxelNodeHash childHashes[8];
    
    // Track the number of expanded children
    int visitedChildren = 0;
    
    // Expand any children with Mixed state
    for(int i = 0; i < 8; ++i)
    {
        // Expand the child if it is mixed
        if(node.isChildExpanded(i))
        {
            // Get the position of the child
            VoxelTile child = children[i];
            
            // Process the child
            node.childPositions[visitedChildren] = processTile(child, &childHashes[i]);
            
            // Keep track of how many expanded children have been visited.
            visitedChildren ++;
        }
        else
        {
            // The child is not expanded.
            // For hashing, use the child mask instead.
            childHashes[i] = node.childMask;
        }
    }
    
    // Compute the node hash
    *hash = computeInnerNodeHash(childHashes);
    
    // Save the node and return its memory address.
    return writer_->writeNode(node, visitedChildren, *hash);
}

VoxelPointer VoxelBuilder::processLeafTile(const VoxelTile &tile, VoxelNodeHash* hash)
{
    // The tile should be of width 8 and depth 1
    assert(tile.width == 8);
    assert(tile.depth == 1);
    
    // Get the cache for this tile.
    int leafIndex = (tile.y / 8) * (depthMap_->resolution() / 8) + (tile.x / 8);
    VoxelLeafCache* cachedLeaf = &leafCache_[leafIndex];
    
    // Check if the cached leaf node is still valid at this depth
    if(tile.z < cachedLeaf->changeZ)
    {
        // Reuse the cached tile
        *hash = cachedLeaf->hash;
        return cachedLeaf->location;
    }
    
    // Create a new leaf node
    VoxelLeafNode leafNode;
    
    // Sample the depth map to create the leaf mask
    leafNode.leafMask = depthMap_->sampleLeafMask(tile.x, tile.y, tile.z, &cachedLeaf->changeZ);
    
    // The leafmask is the hash
    *hash = leafNode.leafMask;
    
    // Save the leaf node and return its memory address.
    VoxelPointer ptr = writer_->writeLeaf(leafNode);
    cachedLeaf->location = ptr;
    cachedLeaf->hash = *hash;
    
    return ptr;
}

void VoxelBuilder::getChildLocations(const VoxelTile &parent, VoxelTile* children) const
{
    if(parent.width == 8)
    {
        // 8x8x8 blocks make leaf tiles
        for(int i = 0; i < 8; ++i)
            children[i] = getLeafChildLocation(parent, i);
    }
    else
    {
        // Bigger blocks make inner child tiles
        for(int i = 0; i < 8; ++i)
            children[i] = getInnerChildLocation(parent, i);
    }
}

VoxelTile VoxelBuilder::getInnerChildLocation(const VoxelTile &parent, int index) const
{
    // The parent must be a cube of size > 8
    assert(parent.width > 8);
    assert(parent.depth == parent.width);
    
    // Inner children are half the size of their parents
    int childWidth = parent.width / 2;
    int childDepth = parent.depth / 2;
    
    // The x,y,z are bits from the index
    int xOffset = index >> 2;
    int yOffset = (index >> 1) & 1;
    int zOffset = index & 1;
    
    assert(xOffset == 0 || xOffset == 1);
    assert(yOffset == 0 || yOffset == 1);
    assert(zOffset == 0 || zOffset == 1);
    
    // Create the tile
    VoxelTile child;
    child.x = parent.x + (childWidth * xOffset);
    child.y = parent.y + (childWidth * yOffset);
    child.z = parent.z + (childDepth * zOffset);
    child.width = childWidth;
    child.depth = childDepth;
    return child;
}

VoxelTile VoxelBuilder::getLeafChildLocation(const VoxelTile &parent, int index) const
{
    // The parent must be an 8x8x8 cube
    assert(parent.width == 8);
    assert(parent.depth == 8);
    
    // Leaf children are the same width as their
    // parent, but have a depth of 1.
    int childWidth = parent.width;
    int childDepth = 1;
    
    // Get the z offset from the index
    int zOffset = index;
    
    // Create the tile
    VoxelTile child;
    child.x = parent.x;
    child.y = parent.y;
    child.z = parent.z + zOffset;
    child.width = childWidth;
    child.depth = childDepth;
    return child;
}
