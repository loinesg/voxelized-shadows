#include "VoxelBuilder.hpp"

#include <assert.h>
#include <cstdio>

VoxelBuilder::VoxelBuilder(const VoxelDepthMap* depthMap)
    : depthMap_(depthMap),
    writer_(depthMap->resolution())
{
    // Resolution must be at least 8
    // Otherwise leaf nodes cannot be made.
    assert(depthMap_->resolution() >= 8);
    
    // Create the leaf cache.
    // There is one cache per 8x8 tile in the depth map.
    int leafTileCount = (depthMap_->resolution() / 8) * (depthMap_->resolution() / 8);
    leafCache_ = new VoxelLeafCache[leafTileCount];
    
    // Set each tile's change distance to 0 so they will be computed on first use
    std::memset(leafCache_, 0, leafTileCount * sizeof(VoxelLeafCache));
}

VoxelBuilder::~VoxelBuilder()
{
    // Delete the leaf cache
    delete[] leafCache_;
}

void VoxelBuilder::build()
{
    // The root tile covers the entire region.
    VoxelTile root;
    root.x = 0;
    root.y = 0;
    root.z = 0;
    root.width = depthMap_->resolution();
    root.depth = depthMap_->resolution();
    
    // Process the root tile
    // This recursively processes all tiles
    rootAddress_ = processTile(root);
}

VoxelPointer VoxelBuilder::processTile(const VoxelTile &tile)
{
    if(tile.depth == 1)
    {
        // Treat as a leaf tile if it is an 8x8x1 block
        return processLeafTile(tile);
    }
    else
    {
        // Otherwise treat as a normal inner tile
        return processInnerTile(tile);
    }
}

VoxelPointer VoxelBuilder::processInnerTile(const VoxelTile &tile)
{
    // The tile should be a cube of at least size 8
    assert(tile.width >= 8);
    assert(tile.width == tile.depth);
    
    // Get the child locations
    VoxelTile children[8];
    getChildLocations(tile, children);
    
    // Create the node
    VoxelInnerNode node;
    node.paddingBits = 0;
    node.childMask = 0;
    
    // Get the child mask
    node.childMask = depthMap_->sampleChildMask(children);

    // Expand any children with Mixed state
    int visitedChildren = 0;
    for(int index = 0; index < 8; ++index)
    {
        // Expand the child if it is mixed
        if(node.isChildExpanded(index))
        {
            // Process the child
            VoxelTile child = children[index];
            node.childPositions[visitedChildren] = processTile(child);
            
            // Keep track of how many expanded children have been visited.
            visitedChildren ++;
        }
    }
    
    node.expandedChildCount = visitedChildren;
    
    // Save the node and return its memory address.
    return writer_.writeNode(node);
}

VoxelPointer VoxelBuilder::processLeafTile(const VoxelTile &tile)
{
    // The tile should be of width 8 and depth 1
    assert(tile.width == 8);
    assert(tile.depth == 1);
    
    // Get the cache for this tile.
    int leafIndex = (tile.y / 8) * (depthMap_->resolution() / 8) + (tile.x / 8);
    VoxelLeafCache* cachedLeaf = &leafCache_[leafIndex];
    if(tile.z < cachedLeaf->changeZ)
    {
        // Cached tile is valid at this z depth
        // Reuse the cached tile
        return cachedLeaf->location;
    }
    
    // Create a new leaf node
    VoxelLeafNode leafNode;
    
    // Sample the depth map to create the leaf mask
    leafNode.leafMask = depthMap_->sampleLeafMask(tile.x, tile.y, tile.z, &cachedLeaf->changeZ);
    
    // Save the leaf node and return its memory address.
    VoxelPointer ptr = writer_.writeLeaf(leafNode);
    cachedLeaf->location = ptr;
    
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
