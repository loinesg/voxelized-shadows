#version 330

#ifdef ALPHA_TEST_ON
    uniform sampler2D _MainTexture;
    in vec2 texcoord;
#endif

void main()
{
#ifdef ALPHA_TEST_ON
    // Discard fragment if main texture alpha is too low.
    if(texture(_MainTexture, texcoord).a < 0.5) discard;
#endif
}
