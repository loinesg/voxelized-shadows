#include "VoxelNode.hpp"

#include <assert.h>

bool VoxelInnerNode::isChildExpanded(int index) const
{
    // Check the index is valid
    assert(index >= 0);
    assert(index < 8);
    
    // Get the shadowing state of the child
    VoxelShadowing shadowing = (VoxelShadowing)((childMask >> (index * 2)) & 3);
    
    // Expanded if shadowing is mixed
    return (shadowing == VS_Mixed);
}

VoxelNodeHash VoxelInnerNode::hash() const
{
    // xor the child positions
    uint64_t childrenHash = 0;
    for(int i = 0; i < expandedChildCount; ++i)
    {
        int offset = (32 + i * 20) % 64;
        childrenHash ^= ((uint64_t)childPositions[i] << offset);
    }
    
    // Combine with the child mask
    return childrenHash ^ ((uint64_t)childMask << 12) ^ (uint64_t)expandedChildCount;
}

VoxelNodeHash VoxelLeafNode::hash() const
{
    // Directly use the leafmask
    return leafMask;
}
