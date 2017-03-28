#pragma once

#include <cstdint>
#include <assert.h>
#include <math.h>

#include "Scene.hpp"
#include "VoxelNode.hpp"

// Contains a dual shadow map to determine the shadowing status of voxel regions.
class VoxelDepthMap
{
public:
    VoxelDepthMap(int resolution, float* entryDepths, float* exitDepths);
    ~VoxelDepthMap();

    // Depth resolution
    int resolution() const { return resolution_; }
    
    // Samples a leaf mask.
    // Also outputs depth that the leaf mask next changes.
    uint64_t sampleLeafMask(int x, int y, int z, int* nextChangeZ) const;
    
    // Samples 8 tile children to construct a childmask.
    uint16_t sampleChildMask(const VoxelTile* children) const;
    
private:
    int resolution_;
    int mipHierarchyHeight_;
    
    // Hierarchy of depths
    // Ordered highest resolution -> lowest resolution
    float** entryDepths_;
    float** exitDepths_;
};
