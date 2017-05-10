#include "VoxelWriter.hpp"

#include <assert.h>
#include <memory.h>
#include <cmath>

VoxelWriter::VoxelWriter()
    : innerNodeLocations_(),
    leafLocations_()
{
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

void VoxelWriter::reserveRootNodePointerSpace(int pointerCount)
{
    // Must be an empty buffer with enough space
    assert(sizeWords_ == 0);
    assert(pointerCount < (int)maxSizeWords_);
    
    // Each pointer occupies 1 word.
    sizeWords_ = pointerCount;
    
    // Create a dummy 100% unshadowed node for the root nodes
    // to point at until the tiles are properly created
    VoxelInnerNode node;
    node.childMask = 21845; // = 0101010101010101 = 8 Unshadowed children
    VoxelPointer nodePtr = writeNode(node, 0, 0);
    
    // Set each of the new pointers to the new address
    for(int i = 0; i < pointerCount; ++i)
    {
        setRootNodePointer(i, nodePtr);
    }
}

void VoxelWriter::setRootNodePointer(int index, VoxelPointer value)
{
    // The pointers are stored in the words at the start of the buffer.
    data_[index] = value;
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

VoxelPointer VoxelWriter::writeTree(const uint32_t* tree, VoxelPointer root, int resolution)
{
    // Compute the tree height from the resolution
    int height = log2(resolution) - 1;
    
    // Check the height is valid
    assert(height > 0);
    assert(height <= 13); // 13 = the height of a 16K tree
    
    // Write the tree to the buffer and return the position of its root
    uint64_t hash;
    return writeSubtree(tree, root, height, &hash);
}

VoxelPointer VoxelWriter::writeSubtree(const uint32_t* tree, uint32_t nodeLocation, int height, uint64_t* hash)
{
    // Check the height is valid
    assert(height > 0);
    assert(height <= 13); // 13 = the height of a 16K tree
    
    // The bottom level in the tree consists of leaf nodes
    if(height == 1)
    {
        // Get the leaf node
        VoxelLeafNode leafNode = *(const VoxelLeafNode*)(tree + nodeLocation);
        
        // The hash is the leaf mask
        *hash = leafNode.leafMask;
        
        // Write to the buffer and return the pointer.
        return writeLeaf(leafNode);
    }
    
    // Otherwise, it is an inner node.
    VoxelInnerNode innerNode = *(const VoxelInnerNode*)(tree + nodeLocation);
    
    // Keep track of child hashes
    uint64_t childHashes[8];
    
    // Check the child mask and write child nodes to the buffer
    int visitedChildren = 0;
    for(int i = 0; i < 8; ++i)
    {
        // Write the child if it is mixed
        if(innerNode.isChildExpanded(i))
        {
            // Get the location of the child
            uint32_t childLocation = innerNode.childPositions[visitedChildren];
            
            // Write the child subtree
            innerNode.childPositions[visitedChildren] = writeSubtree(tree, childLocation, height - 1, &childHashes[i]);
            
            visitedChildren ++;
        }
        else
        {
            // The child is not expanded.
            // For hashing, use the child mask instead.
            childHashes[i] = innerNode.childMask;
        }
    }
    
    // Compute the node hash
    *hash = computeInnerNodeHash(childHashes);
    
    // Write the node and return its address
    return writeNode(innerNode, visitedChildren, *hash);
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
