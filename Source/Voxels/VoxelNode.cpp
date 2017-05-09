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

VoxelNodeHash computeInnerNodeHash(VoxelNodeHash* childHashes)
{
    VoxelNodeHash hash = 0;
    
    // Add each child hash
    for(int i = 0; i < 8; ++i)
    {
        // Get the child hash
        VoxelNodeHash childHash = childHashes[i];
        
        // Shift the hash and combine the child hash
        hash = (hash >> 10) + (hash << 10) + childHash;
    }
    
    return hash;
}
