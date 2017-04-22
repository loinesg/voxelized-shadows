#pragma once

#define GL_GLEXT_PROTOTYPES 1 // Enables OpenGL 3 Features
#include <QGLWidget> // Links OpenGL Headers

#include <queue>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

#include "Scene.hpp"
#include "Light.hpp"
#include "MeshInstance.hpp"
#include "Bounds.hpp"
#include "ShadowMap.hpp"
#include "UniformManager.hpp"
#include "VoxelBuilder.hpp"

class VoxelTree
{
    // The maximum resolution supported by a tree
    const static int MaxResolution = 262144;
    
    // The maximum tile count. Each tile is up to 16K.
    const static int MaxTileCount = (MaxResolution / 16384) * (MaxResolution / 16384);
    
    // The maximum number of tiles that are built simultaneously.
    const static int ConcurrentBuilds = 6;
    
public:
    VoxelTree(UniformManager* uniformManager, const Scene* scene, int resolution);

    // The total resolution of the tree
    int resolution() const { return treeResolution_; }
    
    // The number of subdivisions in each axis (x, y)
    int tileSubdivisions() const { return treeResolution_ / tileResolution_; }
    
    // The number of tiles in different states
    int totalTiles() const { return tileSubdivisions() * tileSubdivisions(); }
    int completedTiles() const { return completedTiles_; }
    
    // The size of the tree
    size_t sizeBytes() const;
    size_t sizeMB() const;
    
    // The size of an equivalent shadow map in bytes
    // This assumes the shadow map uses 24 bits per pixel.
    size_t originalSizeBytes() const;
    size_t originalSizeMB() const;
    
    // The voxels buffer texture id
    GLuint treeBufferTexture() const { return bufferTexture_; }
    
    // Carrys out the tree construction process using time slicing.
    // Most of the work is carried out via background threads, but
    // some work (eg openGL rendering) occurs on the main thread
    // inside this function.
    void updateBuild();
    
private:
    UniformManager* uniformManager_;
    const Scene* scene_;
    
    // Resolution of the entire tree and an individual tile
    int treeResolution_;
    int tileResolution_;
    
    // The voxel buffer and containing buffer texture.
    GLuint buffer_;
    GLuint bufferTexture_;
    
    // A shadow map with 1 cascade. Used for creating dual shadow maps.
    ShadowMap shadowMap_;
    
    // The VoxelWriter containing the entire tree.
    VoxelWriter voxelWriter_;
    
    // The pointers to each child tree within the merged structure
    VoxelPointer treePointers_[MaxTileCount];
    
    // The number of completed tiles and the tiles being built
    int startedTiles_;
    int completedTiles_;
    vector<VoxelBuilder*> activeTiles_;
    mutex activeTilesMutex_;
    
    // The number of tiles uploaded to the GPU
    int tilesOnGPU_;
    
    // The thread that merges finished tiles into voxelWriter_
    thread mergingThread_;
    
    // Starts the processing of the next queued tile.
    // Renders the tile's depth maps and starts a builder thread.
    void startTileBuild();
    
    // Runs on the merging thread.
    // Merges finished builders into the
    void mergeTiles();
    
    // Looks for a builder that has finished and is ready
    // to be merged. Removes it from the active tiles vector.
    VoxelBuilder* findFinishedBuilder();
    
    // Updates the uniform buffer and tree texture buffer
    void updateBuffers();
    void updateUniformBuffer();
    void updateTreeBuffer();
    
    // Computes bounds of the scene in light space.
    // The bounds includes *static* objects only.
    Bounds sceneBoundsLightSpace() const;
    Bounds tileBoundsLightSpace(int index) const;
    
    // Renders dual shadow maps for the scene.
    void computeDualShadowMaps(const Bounds &bounds, float** entryDepths, float** exitDepths);
};
