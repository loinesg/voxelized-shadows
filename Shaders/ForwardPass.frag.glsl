#version 330

uniform sampler2D _MainTexture;

in vec2 texcoord;

out vec4 fragColor;

void main()
{
#ifdef TEXTURE_ON
    vec4 col = texture(_MainTexture, texcoord);
#else
    vec4 col = vec4(1.0, 1.0, 1.0, 1.0);
#endif
    
#ifdef ALPHA_TEST_ON
    #ifdef TEXTURE_ON
        float opacity = col.a;
    #else
        float opacity = texture(_MainTexture, texcoord).a;
    #endif
    
    if(opacity < 0.5) discard;
#endif
    
    fragColor = col;
}
