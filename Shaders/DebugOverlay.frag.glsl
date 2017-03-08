#version 330

#ifdef DEBUG_SHADOW_MAP_TEXTURE
    // Shadow map debugging needs a shadow map sampler
    uniform sampler2DShadow _MainTexture;
#else
    // All other overlays use a standard sampler
    uniform sampler2D _MainTexture;
#endif

in vec2 texcoord;

out vec4 fragColor;

#ifdef DEBUG_SHADOW_MAP_TEXTURE
vec4 SampleShadowMap(vec2 texcoord)
{
    // Sample at multiple depths and add the results
    float sum = 0.0;
    for(float depth = 0.0; depth < 1.0; depth += 0.05)
    {
        sum += textureProj(_MainTexture, vec4(texcoord.xy, depth, 1.0));
    }
    
    float shadow = sum * 0.05;
    return vec4(shadow, shadow, shadow, 1.0);
}
#endif

#ifdef DEBUG_DEPTH_TEXTURE
vec4 SampleDepthTexture(vec2 texcoord)
{
    float depth = texture(_MainTexture, texcoord).r;
    
    // Raise the depth to a power to spread out the values
    depth = pow(depth, 45.0);
    
    return vec4(depth, depth, depth, 0.0);
}
#endif

void main()
{
#if !defined(DEBUG_SHADOW_MAP_TEXTURE)
    // Discard overlay samples on the left half of the screen
    if(texcoord.x < 0.5) discard;
#endif
    
#ifdef DEBUG_SHADOW_MAP_TEXTURE
    fragColor = SampleShadowMap(texcoord);
#elif defined(DEBUG_DEPTH_TEXTURE)
    fragColor = SampleDepthTexture(texcoord);
#else
    fragColor = texture(_MainTexture, texcoord);
    fragColor.rgb = fragColor.rrr;
#endif
}
