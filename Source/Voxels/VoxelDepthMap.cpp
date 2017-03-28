#include "VoxelDepthMap.hpp"

#include <math.h>

VoxelDepthMap::VoxelDepthMap(int resolution, float* entryDepths, float* exitDepths)
    : resolution_(resolution)
{
    // Must be a +vs resolution
    assert(resolution_ > 0);
    
    // Allocate the main arrays for the depth hierarchy
    mipHierarchyHeight_ = log2(resolution) + 1;
    
    // Dont generate the last 1x1 mip. It isn't used.
    mipHierarchyHeight_ --;
    
    entryDepths_ = new float*[mipHierarchyHeight_];
    exitDepths_ = new float*[mipHierarchyHeight_];
    
    // The top tree level has already been created
    entryDepths_[0] = entryDepths;
    exitDepths_[0] = exitDepths;

    // Create the mip levels
    for(int i = 1; i < mipHierarchyHeight_; ++i)
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
}

VoxelDepthMap::~VoxelDepthMap()
{
    // Delete the mip levels
    for(int i = 0; i < mipHierarchyHeight_; ++i)
    {
        delete[] entryDepths_[i];
        delete[] exitDepths_[i];
    }
    
    // Delete the main arrays
    delete[] entryDepths_;
    delete[] exitDepths_;
}

uint64_t VoxelDepthMap::sampleLeafMask(int x, int y, int z, int* nextChangeZ) const
{
    // Check the voxel is within the bounds
    assert(x >= 0 && x < resolution_);
    assert(y >= 0 && y < resolution_);
    assert(z >= 0 && z < resolution_);
    
    // Create the leaf mask
    uint64_t leafMask = 0;
    
    // Assume the leafmask is valid for all following z values
    *nextChangeZ = INT_MAX;
    
    // Sample the 8x8 voxel grid
    for(int yOffset = 0; yOffset < 8; ++yOffset)
    {
        for(int xOffset = 0; xOffset < 8; ++xOffset)
        {
            // Sample row by row to increase cache coherency
            int voxelX = x + xOffset;
            int voxelY = y + yOffset;
            int voxelIndex = voxelY * resolution_ + voxelX;
            
            // Get the midpoint of the shadow caster
            float entryDepth = entryDepths_[0][voxelIndex] * resolution_;
            float exitDepth = exitDepths_[0][voxelIndex] * resolution_;
            float shadowMidpoint = (entryDepth + exitDepth) * 0.5;

            // Depth test
            int shadowed = (z > shadowMidpoint) ? VS_Shadowed : VS_Unshadowed;
            
            // Add to the leaf mask
            int index = (xOffset << 3) | yOffset;
            leafMask |= ((uint64_t)shadowed << index);
            
            // Check if the midpoint depth limits the distance the leafmask
            // can be reused for.
            if(shadowed == VS_Unshadowed && exitDepth < *nextChangeZ && exitDepth >= z)
            {
                *nextChangeZ = exitDepth;
            }
        }
    }
    
    return leafMask;
}

uint16_t VoxelDepthMap::sampleChildMask(const VoxelTile* children) const
{
    // Create the child mask
    uint16_t childMask = 0;
    
    // All the children sample from the same mip
    int mip = log2(children[0].width);
    int mipResolution = resolution_ / children[0].width;
    
    // Determine the shadowing state of each child
    for(int index = 0; index < 8; ++index)
    {
        // Get the child position
        VoxelTile child = children[index];
        
        // Check the child region is within the bounds
        assert(child.width > 0);
        assert(child.depth > 0);
        assert(child.x >= 0 && child.x + child.width <= resolution_);
        assert(child.y >= 0 && child.y + child.width <= resolution_);
        assert(child.z >= 0 && child.z + child.depth <= resolution_);
        
        // Compute the depth bounds of the child region
        float minDepth = (child.z + 0.5) / (float)resolution_;
        float maxDepth = (child.z + child.depth - 0.5) / (float)resolution_;
        
        // Sample the mips
        int mipIndex = (child.y / child.width) * mipResolution + (child.x / child.width);
        float entryDepth = entryDepths_[mip][mipIndex];
        float exitDepth = exitDepths_[mip][mipIndex];
        
        // Determine shadowing state
        VoxelShadowing childShadowing =
            minDepth > entryDepth ? VS_Shadowed // Whole region after shadow entry depth
            : (maxDepth < exitDepth) ? VS_Unshadowed // Whole region before exit depth
            : VS_Mixed; // Mixed shadowing

        // Add to the child mask
        childMask |= (childShadowing << (index * 2));
    }
    
    return childMask;
}
