#version 330

layout(std140) uniform per_object_data
{
    uniform mat4x4 _ModelToWorld;
    uniform mat4x4 _ModelViewProjection;
};

layout(location = 0) in vec4 _position;
layout(location = 1) in vec3 _normal;
layout(location = 2) in vec2 _texcoord;

out vec3 worldNormal;
out vec2 texcoord;

void main()
{
    gl_Position = _ModelViewProjection * _position;
    worldNormal = normalize(mat3(_ModelToWorld) * _normal);
    texcoord = _texcoord;
}
