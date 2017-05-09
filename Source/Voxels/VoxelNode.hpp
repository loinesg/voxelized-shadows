#pragma once

#include <cstdint>

// Pointers are 32-bit word indexes
typedef uint32_t VoxelPointer;

// Use 64-bit hashes
typedef uint64_t VoxelNodeHash;

// Subsection of the voxel structure
struct VoxelTile
{
    int x;
    int y;
    int z;
    
    int width; // x and y size
    int depth; // z size
};

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
    
    // Returns true if the specified child index is expanded.
    bool isChildExpanded(int index) const;
};

// Computes the hash of an inner node from the nodes of its children.
// The childHashes array is size 8 regardless of the number of expanded child nodes.
// Non-expanded child hashes are filled with the parent's childmask.
VoxelNodeHash computeInnerNodeHash(VoxelNodeHash* childHashes);

// Leaf node.
// Contains an 8x8 voxel plane.
struct VoxelLeafNode
{
    // 1 bit per voxel
    // 64 voxels = 64 bits
    uint64_t leafMask;
};
