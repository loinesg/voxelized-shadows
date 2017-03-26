#version 330

// All overlays use a standard sampler
uniform sampler2D _MainTexture;

in vec2 texcoord;

out vec4 fragColor;

void main()
{
#if !defined(DEBUG_SHADOW_MAP_TEXTURE)
    // Discard overlay samples on the left half of the screen
    if(texcoord.x < 0.5) discard;
#endif
    
    // Use the red channel only
    vec4 texel = texture(_MainTexture, texcoord);
    float value = texel.r;
    
#ifdef DEBUG_DEPTH_TEXTURE
    // Raise the depth to a power to spread out the values
    value = pow(value, 45.0);
#endif

    fragColor = vec4(value, value, value, 1.0);
}
