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
    uniform uint _TileSubdivisions;
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

// The result of querying the voxel tree
struct VoxelQuery
{
    uint treeDepthReached;
    float shadowAttenuation;
};

struct LeafNodeQuery
{
    uint treeDepthReached;
    uint highBits;
    uint lowBits;
};

/*
 * Get the voxel-space coordinate corresponding to a world-space position.
 * The computed coordinate can be used to look up a voxel in the tree.
 */
uvec3 getVoxelCoord(vec4 worldSpacePosition)
{
    // worldSpacePosition.w must be 1
    return uvec3((_WorldToVoxel * worldSpacePosition).xyz);
}

/*
 * Computes the child index at a given depth for the specified coord.
 * Must be consistent with the cpp builder code.
 */
uint getChildIndex(uint depth, uvec3 coord)
{
    // The last inner node before the leaf nodes is treated differently.
    if(depth == _VoxelTreeHeight - 3u)
    {
        // Nodes are in a vertical stack.
        // Recover directly from the last z coord bits.
        return coord.z & 7u;
    }
    
    // Get the 0/1 index for each axis
    uint childIndexX = (coord.x >> (_VoxelTreeHeight - 1u - depth)) & 1u;
    uint childIndexY = (coord.y >> (_VoxelTreeHeight - 1u - depth)) & 1u;
    uint childIndexZ = (coord.z >> (_VoxelTreeHeight - 1u - depth)) & 1u;
    
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
uint getVoxelLeafIndex(uvec3 coord)
{
    // Get the last 3 x and y bits
    uint xIndex = coord.x & 7u;
    uint yIndex = coord.y & 7u;
    
    // Combine to form the index
    return (xIndex << 3) | yIndex;
}

LeafNodeQuery getLeafNode(uvec3 coord)
{
    // Compute which tile the coord is in
    uint tileX = coord.x >> _VoxelTreeHeight;
    uint tileY = coord.y >> _VoxelTreeHeight;
    uint tileIndex = (tileX * _TileSubdivisions) + tileY;
    
    // Get the memory address of the first node to visit
    int memAddress = _RootNodeAddresses[tileIndex];

    // Traverse inner nodes
    for(uint depth = 0u; depth <= _VoxelTreeHeight - 3u; ++depth)
    {
        // Fetch the node's child mask
        uint childIndex = getChildIndex(depth, coord);
        uint childMask = texelFetch(_VoxelData, memAddress).r >> 16;
        uint childState = (childMask >> (childIndex * 2u)) & 3u;
        
        // If uniform shadow, exit early
        if(childState < 2u)
        {
            LeafNodeQuery q;
            q.treeDepthReached = depth;
            q.highBits = 4294967295u * childState;
            q.lowBits = 4294967295u * childState;
            return q;
        }
        
        // Mixed shadow
        // Retrieve the child node memory location
        int childPtrIndex = getChildPointerIndex(childMask, childIndex);
        int childPtr = memAddress + 1 + childPtrIndex;
        memAddress = int(texelFetch(_VoxelData, childPtr).r);
    }
    
    // No more inner nodes. Sample the leaf node.
    LeafNodeQuery q;
    q.treeDepthReached = _VoxelTreeHeight;
    q.highBits = texelFetch(_VoxelData, memAddress).r;
    q.lowBits = texelFetch(_VoxelData, memAddress + 1).r;
    return q;
}

VoxelQuery sampleShadowTree(uvec3 coord)
{
    // Get the leaf node
    LeafNodeQuery leaf = getLeafNode(coord);
    
    // Extract the correct location
    uint leafIndex = getVoxelLeafIndex(coord);
    uint shadowing = leafIndex > 31u
        ? (leaf.lowBits >> (leafIndex-32u)) & 1u
        : (leaf.highBits >> leafIndex) & 1u;
    
    // Return the query result
    VoxelQuery q;
    q.treeDepthReached = _VoxelTreeHeight;
    q.shadowAttenuation = float(shadowing);
    return q;
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
    uvec3 voxelCoord = getVoxelCoord(worldPos);
    
    // Sample the shadow tree
    VoxelQuery result = sampleShadowTree(voxelCoord);
    
#ifdef DEBUG_SHOW_VOXEL_TREE_DEPTH
    
    // Determine the % of the tree traversed
    float traversalDepth = float(result.treeDepthReached) / (float(_VoxelTreeHeight));
    
    // Determine the resulting colour
    vec3 rootColour = vec3(0.0, 0.0, 1.0); // blue
    vec3 leafColour = vec3(1.0, 0.0, 0.0); // red
    vec3 depthColour = rootColour * (1.0 - traversalDepth) + leafColour * traversalDepth;
    
    // Output the depth visualization
    fragColor = vec4(depthColour, 0.5);
    
#else
    
    // Output shadow
    fragColor = vec4(result.shadowAttenuation, 0, 0, 1);
    
#endif
}
