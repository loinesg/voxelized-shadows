#pragma once

#include <cstdint>

// Pointers are 32-bit word indexes
typedef uint32_t VoxelPointer;

// Use 64-bit hashes
typedef uint64_t VoxelNodeHash;

// First bit = Mixed / Uniform flag
// Second bit = Shadowed / Unshadowed flag
enum VoxelShadowing : int
{
    VS_Shadowed = 0,
    VS_Unshadowed = 1,
    VS_Mixed = 2,
};

// Inner node.
// May contain children.
struct VoxelInnerNode
{
    // 16 bits padding
    uint16_t paddingBits;
    
    // 2 bits per child
    uint16_t childMask;
    
    // Variable length
    // 32 bits per child
    VoxelPointer childPositions[8];
    
    // The number of children that were VS_Mixed and expanded.
    // Not serialized.
    int expandedChildCount;
    
    // Returns true if the specified child index is expanded.
    bool isChildExpanded(int index) const;
    
    // Compute a hash based on the child information
    VoxelNodeHash hash() const;
};

// Leaf node.
// Contains an 8x8 voxel plane.
struct VoxelLeafNode
{
    // 1 bit per voxel
    // 64 voxels = 64 bits
    uint64_t leafMask;
    
    // Compute a hash based on voxel values
    VoxelNodeHash hash() const;
};
