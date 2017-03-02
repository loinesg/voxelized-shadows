#version 330

uniform sampler2D _MainTexture;

in vec2 texcoord;

out vec4 fragColor;

void main()
{
    // Sample the texture
    vec4 col = texture(_MainTexture, texcoord.xy);
    
    // Some textures use rgb, others use a. Combine both.
    col.rgb *= col.a;
    
    // Only red channel contains data for the shadow map / mask.
    // Convert to black and white.
    col.rgb = col.rrr;
    
    // Use the texture for the fragment colour
    fragColor = col;
}
