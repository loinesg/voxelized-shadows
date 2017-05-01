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
    VoxelWriter();
    ~VoxelWriter();
    
    // The serialized tree data
    const void* data() const { return data_; }
    
    // The size of the written data
    size_t dataSizeBytes() const { return sizeWords_ * 4; }
    size_t dataSizeWords() const { return sizeWords_; }
    
    // Reserves space for the specified number of root node
    // pointers at the start of the buffer.
    void reserveRootNodePointerSpace(int pointerCount);
    
    // Sets a root node pointer to the specified index.
    void setRootNodePointer(int index, VoxelPointer value);
    
    // Writes an inner node to the buffer.
    // Returns its position pointer.
    VoxelPointer writeNode(const VoxelInnerNode &node, int expandedChildCount, VoxelNodeHash hash);
    
    // Writes a leaf node to the buffer.
    // Returns its position pointer.
    VoxelPointer writeLeaf(const VoxelLeafNode &leaf);
    
    // Writes an entire subtree to the buffer.
    // Returns a pointer to the root node.
    VoxelPointer writeTree(const uint32_t* tree, VoxelPointer root, int resolution);
    
private:
    uint32_t* data_;
    uint32_t sizeWords_;
    uint32_t maxSizeWords_;
    
    // Cache of leaf and inner node locations, stored based on hash
    std::unordered_map<VoxelNodeHash, VoxelPointer> innerNodeLocations_;
    std::unordered_map<VoxelNodeHash, VoxelPointer> leafLocations_;
    
    // Writes an entire subtree to the buffer, merging with any
    // existing duplicate nodes that are already in the buffer.
    // Returns the subtree node location.
    // Also outputs the hash of the subtree.
    VoxelPointer writeSubtree(const uint32_t* tree, uint32_t nodeLocation, int height, uint64_t* hash);
    
    // Writes data to the buffer.
    // Returns the word index of the first written word.
    VoxelPointer writeWords(const void* words, int wordCount);
};
