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
    node.expandedChildCount = 0;

    // Determine the shadowing state of each child
    for(int index = 0; index < 8; ++index)
    {
        // Get the child position
        VoxelTile child = children[index];
        
        // Sample the child shadowing
        VoxelShadowing childShadowing = depthMap_->sampleRegionShadow(child.x, child.y, child.z, child.width, child.depth);
        
        // Add to the child mask
        node.childMask |= (childShadowing << (index * 2));
        
        // Increment the childCount if the child will be expanded
        if(childShadowing == VS_Mixed)
        {
            node.expandedChildCount ++;
        }
    }
    
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
    
    // Check the correct number of children were visited
    assert(visitedChildren == node.expandedChildCount);
    
    // Save the node and return its memory address.
    return writer_.writeNode(node);
}

VoxelPointer VoxelBuilder::processLeafTile(const VoxelTile &tile)
{
    // The tile should be of width 8 and depth 1
    assert(tile.width == 8);
    assert(tile.depth == 1);
    
    // Create the leaf node
    VoxelLeafNode leafNode;
    leafNode.leafMask = 0;
    
    // Test each voxel
    for(int x = 0; x < 8; ++x)
    {
        for(int y = 0; y < 8; ++y)
        {
            // Get the voxel position
            int voxelX = tile.x + x;
            int voxelY = tile.y + y;
            int voxelZ = tile.z;
            
            // Get the shadowing state
            VoxelShadowing shadowing = depthMap_->sampleVoxelShadow(voxelX, voxelY, voxelZ);
            assert(shadowing == VS_Shadowed || shadowing == VS_Unshadowed);
            
            // Get the voxel index
            uint64_t index = ((uint64_t)x << 3) | (uint64_t)y;
            
            // Store in the leaf max
            leafNode.leafMask |= ((uint64_t)shadowing << index);
        }
    }
    
    // Save the leaf node and return its memory address.
    return writer_.writeLeaf(leafNode);
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
