#pragma once

#if defined(__linux__)

    // Linux stores resources in the root directory
    #define MESHES_DIRECTORY "Meshes/"
    #define SCENES_DIRECTORY "Scenes/"
    #define SHADERS_DIRECTORY "Shaders/"
    #define TEXTURES_DIRECTORY "Textures/"
    
#elif defined(__APPLE__)

    // Mac bundles are disabled, so files are stored
    // in the same place as linux
    #define MESHES_DIRECTORY "Meshes/"
    #define SCENES_DIRECTORY "Scenes/"
    #define SHADERS_DIRECTORY "Shaders/"
    #define TEXTURES_DIRECTORY "Textures/"

#else
#error Platform not supported
#endif
