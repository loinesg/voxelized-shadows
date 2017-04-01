#pragma once

#include <cstdint>
#include <thread>

#include "VoxelDepthMap.hpp"
#include "VoxelWriter.hpp"
#include "VoxelNode.hpp"

struct VoxelLeafCache
{
    // The pointer to the leaf node
    VoxelPointer location;
    
    // The z depth where the leaf next changes
    int changeZ;
    
    // The hash of the cached leaf node
    VoxelNodeHash hash;
};

// State of the builder
enum class VoxelBuilderState
{
    // Building is underway on a background thread
    Building,
    
    // Building is finished
    Done
};

// Builds a voxel tree structure.
class VoxelBuilder
{
public:
    VoxelBuilder(int tileIndex, int resolution, float* entryDepths, float* exitDepths);
    ~VoxelBuilder();
    
    // The index of the tile being built
    int tileIndex() const { return tileIndex_; }
    
    // The current build state
    VoxelBuilderState buildState() const { return buildState_; }
    
    // Tree data
    const void* tree() const { return writer_->data(); }
    size_t treeSizeWords() const { return writer_->dataSizeWords(); }
    size_t treeSizeBytes() const { return writer_->dataSizeBytes(); }
    
    // Root node position
    VoxelPointer rootAddress() const { return rootAddress_; }
    
    // Pauses the calling thread until building is complete
    void waitUntilDone();
    
private:
    
    // The index of the tile being built
    int tileIndex_;
    
    // The input depth values
    int resolution_;
    float* entryDepths_;
    float* exitDepths_;

    // The thread used for building and current state
    std::thread buildThread_;
    VoxelBuilderState buildState_;
    
    // Objects used during building
    VoxelDepthMap* depthMap_;
    VoxelWriter* writer_;
    VoxelLeafCache* leafCache_;
    
    // The address of the root node.
    VoxelPointer rootAddress_;

    // Builds the tree. Called from the background thread.
    void build();
    
    // Creates objects used for tree construction
    void createDepthMap();
    void createWriter();
    void createLeafCache();
    
    // Tile processing. Returns the hash of the tile node
    VoxelPointer processTile(const VoxelTile &tile, VoxelNodeHash* hash);
    VoxelPointer processInnerTile(const VoxelTile &tile, VoxelNodeHash* hash);
    VoxelPointer processLeafTile(const VoxelTile &tile, VoxelNodeHash* hash);
    
    // Computes the location and size of the 8 children of a tile
    void getChildLocations(const VoxelTile &parent, VoxelTile* children) const;
    VoxelTile getInnerChildLocation(const VoxelTile &parent, int index) const;
    VoxelTile getLeafChildLocation(const VoxelTile &parent, int index) const;
};



