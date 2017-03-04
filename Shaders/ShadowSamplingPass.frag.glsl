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

// shadow_data uniform buffer
layout(std140) uniform shadow_data
{
    uniform vec4 _CascadeDistancesSqr;
    uniform mat4x4 _WorldToShadow[4];
};

// Scene depth texture
uniform sampler2D _MainTexture;

// Shadow map texture
uniform sampler2DShadow _ShadowMapTexture;

in vec2 texcoord;

// Output color
// Contains (shadow, 0, 0, 0)
out vec4 fragColor;

void main()
{
    // Retrieve screen coordinate and depth.
    float depth = texture(_MainTexture, texcoord).r;

    // Compute the world position.
    vec4 clipPos = vec4(texcoord.xy, depth, 1.0);
    vec4 worldPos = _ClipToWorld * clipPos;
    worldPos /= worldPos.w;
    
    // Calculate the sqr distance to the world position
    vec3 toPoint = worldPos.xyz - _CameraPosition.xyz;
    float sqrDistance = dot(toPoint, toPoint);
    
    // Get the coordinate for each cascade
    vec4 coord0 = _WorldToShadow[0] * worldPos;
    vec4 coord1 = _WorldToShadow[1] * worldPos;
    vec4 coord2 = _WorldToShadow[2] * worldPos;
    vec4 coord3 = _WorldToShadow[3] * worldPos;
    
    // Determine which cascade to use, based on distance.
    // Weights will have one component set to 1 and all others set to 0.
    vec4 weights = vec4(lessThan(_CascadeDistancesSqr, vec4(sqrDistance)));
    weights.xyz -= weights.yzw;
    
    // Compute the final shadow coord
    vec4 shadowCoord = (coord0 * weights.x) + (coord1 * weights.y)
                     + (coord2 * weights.z) + (coord3 * weights.w);
    
    // Sample the shadow map.
    float shadow = textureProj(_ShadowMapTexture, shadowCoord);

    // Return the shadow
    fragColor = vec4(shadow, 0.0, 0.0, 0.0);
}
