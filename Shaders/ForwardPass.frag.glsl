#version 330

layout(std140) uniform scene_data
{
    uniform vec3 _CameraPosition;
    uniform vec3 _AmbientColor;
    uniform vec3 _LightColor;
    uniform vec3 _LightDirection;
};

uniform sampler2D _MainTexture;

#ifdef SPECULAR_ON
    in vec3 viewDir;
#endif

#ifdef NORMAL_MAP_ON
    uniform sampler2D _NormalMap;
    in vec3 tangentToWorldX;
    in vec3 tangentToWorldY;
    in vec3 tangentToWorldZ;
#else
    in vec3 worldNormal;
#endif

in vec2 texcoord;

out vec4 fragColor;

/*
 * Implementation of the Lambert lighting model.
 * Returns the final surface color.
 */
vec3 LambertLight(vec4 surface, vec3 worldNormal)
{
    vec3 diff = max(0.0, dot(worldNormal, _LightDirection)) * _LightColor;
    vec3 ambient = _AmbientColor;
    
    return (diff + ambient) * surface.rgb;
}

/*
 * Implementation of the BlinnPhong lighting model.
 * Returns the final surface color.
 */
vec3 BlinnPhongLight(vec4 surface, vec3 worldNormal, vec3 viewDirection)
{
    vec3 h = normalize(_LightDirection + viewDirection);
    float ndoth = max(0.0, dot(worldNormal, h));
    float spec = pow(ndoth, 25.0) * 0.2;
    
    return LambertLight(surface, worldNormal) + spec * _LightColor;
}

void main()
{
#ifdef TEXTURE_ON
    vec4 col = texture(_MainTexture, texcoord);
#else
    vec4 col = vec4(0.4, 0.4, 0.4, 1.0);
#endif
    
#ifdef ALPHA_TEST_ON
    if(texture(_MainTexture, texcoord).a < 0.5) discard;
#endif
    
#ifdef NORMAL_MAP_ON
    vec3 tangentNormal = texture(_NormalMap, texcoord).rgb * 2.0 - 1.0;
    vec3 worldNormal;
    worldNormal.x = dot(tangentNormal, tangentToWorldX);
    worldNormal.y = dot(tangentNormal, tangentToWorldY);
    worldNormal.z = dot(tangentNormal, tangentToWorldZ);
    worldNormal = normalize(worldNormal);
#endif
    
#ifdef SPECULAR_ON
    fragColor = vec4(BlinnPhongLight(col, worldNormal, viewDir), 0.0);
#else
    fragColor = vec4(LambertLight(col, worldNormal), 0.0);
#endif
}
