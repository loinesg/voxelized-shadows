#pragma once

#include <cstdint>
#include <cstdio>
#include <unordered_map>

#include "VoxelNode.hpp"

// Writes tree nodes into a buffer.
// Prevents duplicate nodes from being stored more than once.
class VoxelWriter
{
public:
    VoxelWriter(int resolution);
    ~VoxelWriter();
    
    // The serialized tree data
    const void* data() const { return data_; }
    
    // The size of the written data
    size_t dataSizeBytes() const { return sizeWords_ * 4; }
    size_t dataSizeWords() const { return sizeWords_; }
    
    // Writes an inner node to the buffer.
    // Returns its position pointer.
    VoxelPointer writeNode(const VoxelInnerNode &node, int expandedChildCount, VoxelNodeHash hash);
    
    // Writes a leaf node to the buffer.
    // Returns its position pointer.
    VoxelPointer writeLeaf(const VoxelLeafNode &leaf);
    
private:
    uint32_t* data_;
    uint32_t sizeWords_;
    uint32_t maxSizeWords_;
    
    // Cache of leaf and inner node locations, stored based on hash
    std::unordered_map<VoxelNodeHash, VoxelPointer> innerNodeLocations_;
    std::unordered_map<VoxelNodeHash, VoxelPointer> leafLocations_;
    
    // Writes data to the buffer.
    // Returns the word index of the first written word.
    VoxelPointer writeWords(const void* words, int wordCount);
};
