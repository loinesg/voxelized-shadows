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
    // The maximum tile count. Each tile is up to 16K.
    const static int MaxTileCount = 32*32;
    
    // The maximum number of tiles that are built simultaneously.
    const static int ConcurrentBuilds = 6;
    
public:
    VoxelTree(UniformManager* uniformManager, const Scene* scene, int resolution);

    // The size of the PCF filter kernel.
    // Either 9 or 17.
    int pcfFilterSize() const { return pcfKernelSize_; }
    
    // The total resolution of the tree
    int resolution() const { return treeResolution_; }
    
    // The number of subdivisions in each axis (x, y)
    int tileSubdivisions() const { return treeResolution_ / tileResolution_; }
    
    // The number of tiles in different states
    int totalTiles() const { return tileSubdivisions() * tileSubdivisions(); }
    int completedTiles() const { return uploadedTiles_; }
    
    // The size of the tree
    size_t sizeBytes() const;
    size_t sizeMB() const;
    
    // The size of an equivalent shadow map in bytes
    // This assumes the shadow map uses 24 bits per pixel.
    size_t originalSizeBytes() const;
    size_t originalSizeMB() const;
    
    // The voxels buffer texture id
    GLuint treeBufferTexture() const { return bufferTexture_; }
    
    // Sets the size of the PCF filter kernel.
    // Must be either 1, 9 or 17.
    void setPCFFilterSize(int kernelSize);
    
    // Carrys out the tree construction process using time slicing.
    // Most of the work is carried out via background threads, but
    // some work (eg openGL rendering) occurs on the main thread
    // inside this function.
    void updateBuild();
    
private:
    UniformManager* uniformManager_;
    const Scene* scene_;
    Bounds sceneBoundsLightSpace_;
    
    // The size of the PCF filter kernel
    int pcfKernelSize_;
    
    // The building status
    int startedTiles_;
    int mergedTiles_;
    int uploadedTiles_;
    
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
    
    // The tiles that are not started yet and those being built
    vector<int> notStartedTiles_;
    vector<VoxelBuilder*> activeTiles_;
    mutex activeTilesMutex_;
    
    // The thread that merges finished tiles into voxelWriter_
    thread mergingThread_;
    
    // Starts the processing of the next queued tile.
    // Renders the tile's depth maps and starts a builder thread.
    void startTileBuild();
    int getNextTileToStart();
    
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
    
    // Computes the bitmask to use on a leaf for the with
    // the specified PCF kernel centre coordinates
    uint64_t pcfBitmask(int kernelX, int kernelY) const;
    
    // Computes bounds of the scene in light space.
    // The bounds includes *static* objects only.
    Bounds computeSceneBoundsLightSpace() const;
    Bounds tileBoundsLightSpace(int index) const;
    
    // Renders dual shadow maps for the scene.
    void computeDualShadowMaps(const Bounds &bounds, float** entryDepths, float** exitDepths);
};
