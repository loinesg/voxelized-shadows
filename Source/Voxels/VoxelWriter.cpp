#include "VoxelWriter.hpp"

#include <assert.h>
#include <memory.h>

VoxelWriter::VoxelWriter(int resolution)
    : innerNodeLocations_(),
    leafLocations_()
{
    // Resolution must be +ve
    assert(resolution > 0);
    
    // Define the max buffer size
    const uint32_t bufferSizeMB = 128;
    const uint32_t bufferSizeB = bufferSizeMB * 1024 * 1024;
    maxSizeWords_ = bufferSizeB / 4;
    
    // Allocate the buffer
    data_ = new uint32_t[maxSizeWords_];
    sizeWords_ = 0;
}

VoxelWriter::~VoxelWriter()
{
    delete[] data_;
}

VoxelPointer VoxelWriter::writeNode(const VoxelInnerNode &node, int expandedChildCount, VoxelNodeHash hash)
{
    // Check if a node with the same hash has already been written
    auto cached = innerNodeLocations_.find(hash);
    if(cached != innerNodeLocations_.end())
    {
        return cached->second;
    }
    
    // No existing node. Write a new one and cache.
    VoxelPointer ptr = writeWords(&node, 1 + expandedChildCount);
    innerNodeLocations_.insert(std::pair<VoxelNodeHash, VoxelPointer>(hash, ptr));
    
    // Return the address
    return ptr;
}

VoxelPointer VoxelWriter::writeLeaf(const VoxelLeafNode &leaf)
{
    // Get the leaf hash
    // The hash is identical to the 64 bit leafmask.
    VoxelNodeHash hash = leaf.leafMask;
    
    // Check if a leaf with the same has was already written.
    auto cached = leafLocations_.find(hash);
    if(cached != leafLocations_.end())
    {
        return cached->second;
    }
    
    // No existing leaf. Write a new one and cache.
    VoxelPointer ptr = writeWords(&leaf, 2);
    leafLocations_.insert(std::pair<VoxelNodeHash, VoxelPointer>(hash, ptr));
    
    // Return the location
    return ptr;
}

VoxelPointer VoxelWriter::writeWords(const void* words, int wordCount)
{
    // Check the word count is valid and not too big
    assert(wordCount > 0);
    assert(sizeWords_ + wordCount <= maxSizeWords_);
    
    // Remember the start position
    uint32_t startPos = sizeWords_;
    
    // Write to the buffer
    memcpy(data_ + sizeWords_, words, wordCount * 4);
    sizeWords_ += wordCount;
    
    // Return the location
    return startPos;
}
