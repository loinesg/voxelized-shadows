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
        for(int y = 0; y < resolution; ++y)
        {
            for(int x = 0; x < resolution; ++x)
            {
                int mipTexel = y * resolution + x;
                
                // Use the corresponding texel, and the 3 adjacent ones
                int parent0 = (y * 2) * (resolution * 2) + (x * 2);
                int parent1 = parent0 + 1;
                int parent2 = parent0 + (resolution * 2);
                int parent3 = parent2 + 1;
                
                // Get the biggest entry depth
                float entry0 = entryDepths_[i-1][parent0];
                float entry1 = entryDepths_[i-1][parent1];
                float entry2 = entryDepths_[i-1][parent2];
                float entry3 = entryDepths_[i-1][parent3];
                entryDepths_[i][mipTexel] = std::max(std::max(entry0, entry1), std::max(entry2, entry3));
                
                // Get the smallest exit depth
                float exit0 = exitDepths_[i-1][parent0];
                float exit1 = exitDepths_[i-1][parent1];
                float exit2 = exitDepths_[i-1][parent2];
                float exit3 = exitDepths_[i-1][parent3];
                exitDepths_[i][mipTexel] = std::min(std::min(exit0, exit1), std::min(exit2, exit3));
            }
        }
    }
    
    // Create the midpoints
    midpointDepths_ = new int[resolution_ * resolution_];
    for(int i = 0; i < resolution_ * resolution_; ++i)
    {
        float entryDepth = entryDepths[i];
        float exitDepth = exitDepths[i];
        float midpointDepth = (entryDepth + exitDepth) * 0.5;
        
        midpointDepths_[i] = (midpointDepth * resolution_) - 0.5;
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
    
    // Delete the midpoint depths
    delete[] midpointDepths_;
}

VoxelShadowing VoxelDepthMap::sampleVoxelShadow(int x, int y, int z) const
{
    // Check the voxel is within the bounds
    assert(x >= 0 && x < resolution_);
    assert(y >= 0 && y < resolution_);
    assert(z >= 0 && z < resolution_);
    
    // Get the midpoint of the shadow caster
    int shadowMidpoint = midpointDepths_[y * resolution_ + x];
    
    // Get the voxel depth
    int voxelDepth = z;
    
    // Depth test
    return (voxelDepth > shadowMidpoint) ? VS_Shadowed : VS_Unshadowed;
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
