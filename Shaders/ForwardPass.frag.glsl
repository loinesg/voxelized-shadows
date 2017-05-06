#version 330

layout(std140) uniform scene_data
{
    uniform vec3 _AmbientColor;
    uniform vec3 _LightColor;
    uniform vec3 _LightDirection;
};

layout(std140) uniform camera_data
{
    uniform vec2 _ScreenResolution;
    uniform vec3 _CameraPosition;
    uniform mat4x4 _ViewProjectionMatrix;
    uniform mat4x4 _ClipToWorld;
};

uniform sampler2D _MainTexture;

#ifdef SPECULAR_ON
    // Direction to the camera, normalized.
    in vec3 viewDir;
#endif

#ifdef FOG_ON
    // Distance to the camera
    in float viewDist;
#endif

#ifdef NORMAL_MAP_ON
    uniform sampler2D _NormalMap;
    in vec3 tangentToWorldX;
    in vec3 tangentToWorldY;
    in vec3 tangentToWorldZ;
#else
    in vec3 worldNormal;
#endif

// Screen space shadow mask texture
uniform sampler2D _ShadowMask;

// Main / normal map texture coordinate
in vec2 texcoord;

// Final colour
out vec4 fragColor;

/*
 * Implementation of the Lambert lighting model.
 * Returns the final surface color.
 */
vec3 LambertLight(vec4 surface, vec3 worldNormal)
{
    return max(0.0, dot(worldNormal, _LightDirection)) * _LightColor * surface.rgb;
}

/*
 * Implementation of the BlinnPhong lighting model.
 * Returns the final surface color.
 */
vec3 BlinnPhongLight(vec4 surface, vec3 worldNormal, vec3 viewDirection)
{
    vec3 h = normalize(_LightDirection + viewDirection);
    float ndoth = max(0.0, dot(worldNormal, h));
    float spec = pow(ndoth, 25.0) * surface.a;
    
    return LambertLight(surface, worldNormal) + spec * _LightColor;
}

#ifdef NORMAL_MAP_ON

/*
 * Unpacks a normal vector stored in a normal map.
 * Returns the world space normal vector.
 */
vec3 UnpackNormalMap(vec4 packedNormal)
{
    // Normal map rgb contains tangent space normal
    // encoded in [-1 - 1] range.
    vec3 tangentNormal = packedNormal.rgb * 2.0 - 1.0;
    
    // Construct world space normal from the world space
    // (tangent, normal, bitangent) basis sent from the vertex shader.
    vec3 worldNormal;
    worldNormal.x = dot(tangentNormal, tangentToWorldX);
    worldNormal.y = dot(tangentNormal, tangentToWorldY);
    worldNormal.z = dot(tangentNormal, tangentToWorldZ);
    
    // Ensure the normal is unit length
    worldNormal = normalize(worldNormal);
    
    return worldNormal;
}

#endif // NORMAL_MAP_ON

/*
 * Gets the shadow mask value for the given coordinate.
 */
float SampleShadow()
{
    // Derive the shadow coord from the screen position.
    vec2 shadowCoord = gl_FragCoord.xy / _ScreenResolution;
    
    // Shadow map already filtered and stored in
    // the shadow mask. Just use the value directly.
    return texture(_ShadowMask, shadowCoord).r;
}

void main()
{
#ifdef TEXTURE_ON
    // Use the main texture for the surface color
    vec4 col = texture(_MainTexture, texcoord);
#else
    // No main texture. Use a constant grey surface color.
    vec4 col = vec4(0.4, 0.4, 0.4, 1.0);
#endif
    
#ifdef ALPHA_TEST_ON
    // Alpha testing on. Discard pixel if the main texture alpha is low.
    if(texture(_MainTexture, texcoord).a < 0.5) discard;
#endif
    
#ifdef NORMAL_MAP_ON
    // Retrieve world space normal from normal map.
    // NORMAL_MAP_OFF => worldNormal supplied by vertex shader
    vec3 worldNormal = UnpackNormalMap(texture(_NormalMap, texcoord));
#endif
    
#ifdef SPECULAR_ON
    // Specular needed. Calculate full BlinnPhong lighting.
    vec3 directLight = BlinnPhongLight(col, worldNormal, viewDir);
#else
    // No specular needed. Use Lambert lighting instead.
    vec3 directLight = LambertLight(col, worldNormal);
#endif 
    
    // Sample the shadow map from the screen space shadow mask
    float shadow = SampleShadow();
    
    // Modify the direct light based on shadow sampling.
    directLight *= shadow;
    
    // Use direct + ambient light for final colour
    vec3 ambientLight = col.rgb * _AmbientColor;
    vec3 finalColor = directLight + ambientLight;
    fragColor = vec4(finalColor.rgb, 1.0);
    
#ifdef FOG_ON
    // Compute the fog density using exponential squared
    float fogDensity = 0.0075 * viewDist;
    fogDensity = 1.0 - exp2(-fogDensity * fogDensity);
    
    // Apply fog to the fragColor
    vec3 fogColor = vec3(0.5, 0.5, 0.5);
    fragColor.rgb = (fogColor * fogDensity) + (fragColor.rgb * (1.0 - fogDensity));
#endif
}
