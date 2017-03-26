#include "VoxelDepthMap.hpp"

#include <math.h>

VoxelDepthMap::VoxelDepthMap(int resolution, float* entryDepths, float* exitDepths)
    : resolution_(resolution)
{
    // Must be a +vs resolution
    assert(resolution_ > 0);
    
    // Allocate the main arrays for the depth hierarchy
    treeHeight_ = log2(resolution_) + 2;
    entryDepths_ = new float*[treeHeight_];
    exitDepths_ = new float*[treeHeight_];
    
    // The top tree level has already been created
    entryDepths_[0] = entryDepths;
    exitDepths_[0] = exitDepths;

    // Create the mip levels
    for(int i = 1; i < treeHeight_; ++i)
    {
        // Halve the resolution
        resolution /= 2;
        
        // Make the arrays
        entryDepths_[i] = new float[resolution * resolution];
        exitDepths_[i] = new float[resolution * resolution];
        
        // Create each element
        for(int x = 0; x < resolution; ++x)
        {
            for(int y = 0; y < resolution; ++y)
            {
                int index = y * resolution + x;
                int parent0 = (y*2) * (resolution * 2) + (x*2);
                int parent1 = (y*2 + 1) * (resolution * 2) + (x*2);
                int parent2 = (y*2) * (resolution * 2) + (x*2 + 1);
                int parent3 = (y*2 + 1) * (resolution * 2) + (x*2 + 1);
                
                // Get the biggest entry depth
                entryDepths_[i][index] = std::max(std::max(entryDepths_[i-1][parent0], entryDepths_[i-1][parent1]), std::max(entryDepths_[i-1][parent2], entryDepths_[i-1][parent3]));
                
                // Get the smallest exit depth
                exitDepths_[i][index] = std::min(std::min(exitDepths_[i-1][parent0], exitDepths_[i-1][parent1]), std::min(exitDepths_[i-1][parent2], exitDepths_[i-1][parent3]));
            }
        }
    }
}

VoxelDepthMap::~VoxelDepthMap()
{
    // Delete the mip levels
    for(int i = 0; i < treeHeight_; ++i)
    {
        delete[] entryDepths_[i];
        delete[] exitDepths_[i];
    }
    
    // Delete the main arrays
    delete[] entryDepths_;
    delete[] exitDepths_;
}

VoxelShadowing VoxelDepthMap::sampleVoxelShadow(int x, int y, int z) const
{
    // Check the voxel is within the bounds
    assert(x >= 0 && x < resolution_);
    assert(y >= 0 && y < resolution_);
    assert(z >= 0 && z < resolution_);
    
    // Get the midpoint of the shadow caster
    int index = y * resolution_ + x;
    float shadowEntry = entryDepths_[0][index];
    float shadowExit = exitDepths_[0][index];
    float shadowMidpoint = (shadowEntry + shadowExit) * 0.5;
    
    // Get the voxel depth
    float voxelDepth = z + 0.5;
    
    // Depth test
    return (voxelDepth > shadowMidpoint * resolution_) ? VS_Shadowed : VS_Unshadowed;
}

VoxelShadowing VoxelDepthMap::sampleRegionShadow(int x, int y, int z, int width, int depth) const
{
    // Check the region is within the bounds
    assert(width > 0);
    assert(depth > 0);
    assert(x >= 0 && x + width <= resolution_);
    assert(y >= 0 && y + width <= resolution_);
    assert(z >= 0 && z + depth <= resolution_);
    
    // Compute the depth bounds of the region
    float minDepth = (z + 0.5) / (float)resolution_;
    float maxDepth = (z + depth - 0.5) / (float)resolution_;
    
    // Calculate the correct mip level to sample
    int mip = log2(width);
    
    // Sample the mips
    int mipResolution = resolution_ / width;
    int mipIndex = (y / width) * mipResolution + (x / width);
    float entryDepth = entryDepths_[mip][mipIndex];
    float exitDepth = exitDepths_[mip][mipIndex];
    
    // If the whole region is after the entry depth, it is shadowed
    if(minDepth > entryDepth)
    {
        return VS_Shadowed;
    }
    
    // If the whole region is before the exit depth, it is unshadowed
    if(maxDepth < exitDepth)
    {
        return VS_Unshadowed;
    }
    
    // Otherwise the region is mixed
    return VS_Mixed;
}
