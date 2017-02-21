#version 330

layout(std140) uniform scene_data
{
    uniform vec3 _AmbientColor;
    uniform vec3 _LightColor;
    uniform vec3 _LightDirection;
};

uniform sampler2D _MainTexture;

in vec3 worldNormal;
in vec2 texcoord;

out vec4 fragColor;

vec3 LambertLight(vec3 worldNormal)
{
    vec3 lightDir = normalize(vec3(1.0, 1.0, 0.2));
    vec3 lightCol = vec3(0.95, 0.88, 0.88) * 2.5;
    
    return max(0.0, dot(worldNormal, _LightDirection)) * _LightColor;
}

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
    
    vec3 light = LambertLight(worldNormal);
    light += _AmbientColor;
    col.rgb *= light;
    
    fragColor = col;
}
