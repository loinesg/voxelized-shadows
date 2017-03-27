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
    
    // Gets the shadow state of a single voxel
    VoxelShadowing sampleVoxelShadow(int x, int y, int z) const;
    
    // Gets the shadow state of a region
    VoxelShadowing sampleRegionShadow(int x, int y, int z, int width, int depth) const;
    
private:
    int resolution_;
    int treeHeight_;
    
    // Hierarchy of depths
    // Ordered highest resolution -> lowest resolution
    float** entryDepths_;
    float** exitDepths_;
    int* midpointDepths_;
};
