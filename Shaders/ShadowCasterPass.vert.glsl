#version 330

layout(std140) uniform per_object_data
{
    uniform mat4x4 _ModelToWorld;
    uniform mat4x4 _ModelViewProjection;
};

layout(location = 0) in vec4 _position;

#ifdef ALPHA_TEST_ON
    layout(location = 3) in vec2 _texcoord;
    out vec2 texcoord;
#endif

void main()
{
    gl_Position = _ModelViewProjection * _position;
    
#ifdef ALPHA_TEST_ON
    // Texcoord only needed for alpha test texture lookups
    texcoord = _texcoord;
#endif
}
