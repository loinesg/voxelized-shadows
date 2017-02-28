#version 330

// scene_data uniform buffer
// Used for _ScreenResolution value
layout(std140) uniform scene_data
{
    uniform vec2 _ScreenResolution;
    uniform vec3 _CameraPosition;
    uniform vec3 _AmbientColor;
    uniform vec3 _LightColor;
    uniform vec3 _LightDirection;
};

// shadow_data uniform buffer
// Used for shadow map sampling
layout(std140) uniform shadow_data
{
    uniform mat4x4 _ClipToShadow;
    uniform vec4 _ShadowMapTexelSize;
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

    // Assemble the clip position.
    vec4 clipPos = vec4(texcoord.xy, depth, 1.0);

    // Get the shadow coordinate
    vec4 shadowCoord = _ClipToShadow * clipPos;
    
    // Sample the shadow map.
    float shadow = textureProj(_ShadowMapTexture, shadowCoord);

    // Return the shadow
    fragColor = vec4(shadow, shadow, shadow, 0.0);
}
