#include "VoxelDepthMap.hpp"

#include <math.h>

VoxelDepthMap::VoxelDepthMap(int resolution, float* entryDepths, float* exitDepths)
    : resolution_(resolution)
{
    // Must be a +vs resolution
    assert(resolution_ > 0);
    
    // Compute the number of mip levels needed. There is no
    // level for the last 1x1 mip as it is not needed.
    mipHierarchyHeight_ = log2(resolution);
    
    // Create the hierarchy
    entryDepths_ = new float*[mipHierarchyHeight_];
    exitDepths_ = new float*[mipHierarchyHeight_];
    
    // The top tree level has already been created
    entryDepths_[0] = entryDepths;
    exitDepths_[0] = exitDepths;

    // Create the mip levels
    for(int mip = 1; mip < mipHierarchyHeight_; ++mip)
    {
        // Halve the resolution with each mip level
        int parentResolution = resolution;
        resolution /= 2;
        int mipResolution = resolution;
        
        // Make the arrays
        entryDepths_[mip] = new float[mipResolution * mipResolution];
        exitDepths_[mip] = new float[mipResolution * mipResolution];
        
        // Consider row in the parent mip
        for(int parentRow = 0; parentRow < parentResolution; ++parentRow)
        {
            // Consider each element in the row, in pairs
            for(int i = 0; i < mipResolution; ++i)
            {
                int parentIndex = parentRow * parentResolution + i*2;
                int mipIndex = parentRow/2 * mipResolution + i;
                
                // Get the min exit depth from the pair
                float entry0 = entryDepths_[mip-1][parentIndex];
                float entry1 = entryDepths_[mip-1][parentIndex + 1];
                float entryMax = std::max(entry0, entry1);
                
                // Get the max exit depth from the pair
                float exit0 = exitDepths_[mip-1][parentIndex];
                float exit1 = exitDepths_[mip-1][parentIndex +1];
                float exitMin = std::min(exit0, exit1);
                
                if((parentRow % 2) == 0)
                {
                    // If it is an even row, we haven't computed the other 2 values yet
                    entryDepths_[mip][mipIndex] = entryMax;
                    exitDepths_[mip][mipIndex] = exitMin;
                }
                else
                {
                    // If an odd row, combine the 2 values with the other values computed earlier
                    entryDepths_[mip][mipIndex] = std::max(entryDepths_[mip][mipIndex], entryMax);
                    exitDepths_[mip][mipIndex] = std::min(exitDepths_[mip][mipIndex], exitMin);
                }
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
            float doubleShadowMidpoint = (entryDepth + exitDepth);

            // Depth test
            int shadowed = (z * 2 > doubleShadowMidpoint) ? VS_Shadowed : VS_Unshadowed;
            
            // Add to the leaf mask
            int index = (xOffset << 3) | yOffset;
            leafMask |= ((uint64_t)shadowed << index);
            
            // Check if the midpoint depth limits the distance the leafmask
            // can be reused for.
            if(exitDepth >= z && exitDepth < *nextChangeZ)
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
    int mipResolution = 1 << (mipHierarchyHeight_ - mip);
    
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
        float minDepth = (child.z + 0.5);
        float maxDepth = (child.z + child.depth - 0.5);
        
        // Sample the mips
        int mipIndex = (child.y >> mip) * mipResolution + (child.x >> mip);
        float entryDepth = entryDepths_[mip][mipIndex] * resolution_;
        float exitDepth = exitDepths_[mip][mipIndex] * resolution_;
        
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
