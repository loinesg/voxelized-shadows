#pragma once

#include <cstdint>

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

// Builds a voxel tree structure.
class VoxelBuilder
{
public:
    VoxelBuilder(const VoxelDepthMap* depthMap);
    ~VoxelBuilder();
    
    // Tree data
    const void* tree() const { return writer_.data(); }
    size_t treeSizeWords() const { return writer_.dataSizeWords(); }
    size_t treeSizeBytes() const { return writer_.dataSizeBytes(); }
    
    // Root node position
    VoxelPointer rootAddress() const { return rootAddress_; }
    
    // Builds the tree. Slow.
    void build();
    
private:
    // Shadow map depth intervals
    const VoxelDepthMap* depthMap_;
    VoxelWriter writer_;
    
    // The leafmask cache. One value per 8x8 depth tile.
    VoxelLeafCache* leafCache_;
    
    // The address of the root node.
    VoxelPointer rootAddress_;
    
    // Tile processing. Returns the hash of the tile node
    VoxelPointer processTile(const VoxelTile &tile, VoxelNodeHash* hash);
    VoxelPointer processInnerTile(const VoxelTile &tile, VoxelNodeHash* hash);
    VoxelPointer processLeafTile(const VoxelTile &tile, VoxelNodeHash* hash);
    
    // Computes the location and size of the 8 children of a tile
    void getChildLocations(const VoxelTile &parent, VoxelTile* children) const;
    VoxelTile getInnerChildLocation(const VoxelTile &parent, int index) const;
    VoxelTile getLeafChildLocation(const VoxelTile &parent, int index) const;
};



