#version 330

// scene_data uniform buffer
layout(std140) uniform scene_data
{
    uniform vec2 _ScreenResolution;
    uniform vec3 _CameraPosition;
    uniform mat4x4 _ClipToWorld;
    uniform vec3 _AmbientColor;
    uniform vec3 _LightColor;
    uniform vec3 _LightDirection;
};

// voxel_data uniform buffer
layout(std140) uniform voxel_data
{
    uniform mat4x4 _WorldToVoxel;
    uniform uint _VoxelTreeHeight;
    uniform int _TileSubdivisions;
    uniform int _RootNodeAddresses[64];
};

// Scene depth texture
uniform sampler2D _MainTexture;

// Voxelized Shadow Map data
uniform usamplerBuffer _VoxelData;

in vec2 texcoord;

// Output color
// Contains (shadow, 0, 0, 0)
out vec4 fragColor;

/*
 * Computes the child index at a given depth from the x,y,z coordinate bits.
 * Must be consistent with the cpp builder code.
 */
uint getChildIndex(uint depth, uint coordX, uint coordY, uint coordZ)
{
    // The last inner node before the leaf nodes is treated differently.
    if(depth == _VoxelTreeHeight - 3u)
    {
        // Nodes are in a vertical stack.
        // Recover directly from the last z coord bits.
        return coordZ & 7u;
    }
    
    // Get the 0/1 index for each axis
    uint childIndexX = (coordX >> (_VoxelTreeHeight - 1u - depth)) & 1u;
    uint childIndexY = (coordY >> (_VoxelTreeHeight - 1u - depth)) & 1u;
    uint childIndexZ = (coordZ >> (_VoxelTreeHeight - 1u - depth)) & 1u;
    
    // Combine them
    return (childIndexX << 2) | (childIndexY << 1) | childIndexZ;
}

/*
 * Computes the child pointer index for a given child and child mask.
 */
int getChildPointerIndex(uint childMask, uint childIndex)
{
    int ptr = 0;
    
    for(uint i = 0u; i < childIndex; ++i)
    {
        if(((childMask >> (i * 2u)) & 3u) > 1u)
        {
            ptr ++;
        }
    }
    
    return ptr;
}

/*
 * Computes the index of a voxel within its leaf node.
 */
uint getVoxelLeafIndex(uint coordX, uint coordY)
{
    // Get the last 3 x and y bits
    uint xIndex = coordX & 7u;
    uint yIndex = coordY & 7u;
    
    // Combine to form the index
    return (xIndex << 3) | yIndex;
}

float sampleShadowTree(vec4 coord)
{
    // Retrive the coord bits
    uint coordBitsX = uint(coord.x);
    uint coordBitsY = uint(coord.y);
    uint coordBitsZ = uint(coord.z);
    
    // Determine which root node to use, based on the position
    // within the voxel forest
    int rootNodeX = int(coordBitsX >> _VoxelTreeHeight);
    int rootNodeY = int(coordBitsY >> _VoxelTreeHeight);
    int rootNodeIndex = (rootNodeX * _TileSubdivisions) | rootNodeY;
    
    // Start at the root node
    int memAddress = _RootNodeAddresses[rootNodeIndex];

    // Traverse inner nodes
    for(uint depth = 0u; depth <= _VoxelTreeHeight - 3u; ++depth)
    {
        // Determine the child index from the coord bits
        uint childIndex = getChildIndex(depth, coordBitsX, coordBitsY, coordBitsZ);
        
        // Fetch the node's child mask
        uint childMask = texelFetch(_VoxelData, memAddress).r >> 16;
        uint childState = (childMask >> (childIndex * 2u)) & 3u;
        
        // If uniform shadow, exit early
        if(childState < 2u)
        {
#ifdef DEBUG_SHOW_VOXEL_TREE_DEPTH
            // Output the depth visualization
            return float(depth) / (float(_VoxelTreeHeight));
#endif
            
            // Output the shadow
            return float(childState);
        }
        
        // Mixed shadow
        // Retrive the child node memory location
        int childPtrIndex = getChildPointerIndex(childMask, childIndex);
        int childPtr = memAddress + 1 + childPtrIndex;
        memAddress = int(texelFetch(_VoxelData, childPtr).r);
    }
    
#ifdef DEBUG_SHOW_VOXEL_TREE_DEPTH
    return 1.0;
#endif
    
    // No more inner nodes to traverse.
    // Sample the leaf node.
    uint leafIndex = getVoxelLeafIndex(coordBitsX, coordBitsY);
    
    // Offset if in the second leafmask byte
    if(leafIndex > 31u)
    {
        leafIndex -= 32u;
        memAddress ++;
    }
    
    uint partialLeafMask = texelFetch(_VoxelData, memAddress).r;
    return float((partialLeafMask >> leafIndex) & 1u);
}

void main()
{
    // Retrieve screen coordinate and depth.
    float depth = texture(_MainTexture, texcoord).r;

    // Compute the world position.
    vec4 clipPos = vec4(texcoord.xy, depth, 1.0);
    vec4 worldPos = _ClipToWorld * clipPos;
    worldPos /= worldPos.w;
    
    // Get the coordinate for the voxel tree
    vec4 voxelCoord = _WorldToVoxel * worldPos;
 
    float shadow = sampleShadowTree(voxelCoord);
    
#ifdef DEBUG_SHOW_VOXEL_TREE_DEPTH
    // Output the depth visualization
    fragColor = vec4(shadow, 0.0, 1.0 - shadow, 0.5);
#else
    // Output shadow
    fragColor = vec4(shadow, 0, 0, 1);
#endif
}
