#version 330

layout(std140) uniform scene_data
{
    uniform vec3 _CameraPosition;
    uniform vec3 _AmbientColor;
    uniform vec3 _LightColor;
    uniform vec3 _LightDirection;
};

layout(std140) uniform per_object_data
{
    uniform mat4x4 _ModelToWorld;
    uniform mat4x4 _ModelViewProjection;
};

layout(location = 0) in vec4 _position;
layout(location = 1) in vec3 _normal;
layout(location = 2) in vec4 _tangent;
layout(location = 3) in vec2 _texcoord;

#ifdef SPECULAR_ON
    out vec3 viewDir;
#endif

#ifdef NORMAL_MAP_ON
    out vec3 tangentToWorldX;
    out vec3 tangentToWorldY;
    out vec3 tangentToWorldZ;
#else
    out vec3 worldNormal;
#endif

out vec2 texcoord;

void main()
{
    gl_Position = _ModelViewProjection * _position;
    
#ifdef SPECULAR_ON
    vec4 worldPosition = _ModelToWorld * _position;
    viewDir = normalize(_CameraPosition - worldPosition.xyz);
#endif
    
#ifdef NORMAL_MAP_ON
    vec3 worldNormal = normalize(mat3(_ModelToWorld) * _normal);
    vec3 worldTangent = normalize(mat3(_ModelToWorld) * _tangent.xyz);
    vec3 worldBitangent = cross(worldNormal, worldTangent) * _tangent.w * -1.0;
    tangentToWorldX = vec3(worldTangent.x, worldBitangent.x, worldNormal.x);
    tangentToWorldY = vec3(worldTangent.y, worldBitangent.y, worldNormal.y);
    tangentToWorldZ = vec3(worldTangent.z, worldBitangent.z, worldNormal.z);
#else
    worldNormal = normalize(mat3(_ModelToWorld) * _normal);
#endif
    
    texcoord = _texcoord;
}
