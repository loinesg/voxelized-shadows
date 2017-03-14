#pragma once

#if defined(__linux__)

    // Linux stores resources in the root directory
    #define MESHES_DIRECTORY "Meshes/"
    #define SCENES_DIRECTORY "Scenes/"
    #define SHADERS_DIRECTORY "Shaders/"
    #define TEXTURES_DIRECTORY "Textures/"
    
#elif defined(__APPLE__)

    // Mac stores resources in the Contents/Resources directory
    #define MESHES_DIRECTORY "voxelized-shadows.app/Contents/Resources/Meshes/"
    #define SCENES_DIRECTORY "voxelized-shadows.app/Contents/Resources/Scenes/"
    #define SHADERS_DIRECTORY "voxelized-shadows.app/Contents/Resources/Shaders/"
    #define TEXTURES_DIRECTORY "voxelized-shadows.app/Contents/Resources/Textures/"
    
#else
#error Platform not supported
#endif
