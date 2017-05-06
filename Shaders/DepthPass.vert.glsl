#version 330

// Camera uniform buffer
layout(std140) uniform camera_data
{
    uniform vec2 _ScreenResolution;
    uniform vec3 _CameraPosition;
    uniform mat4x4 _ViewProjectionMatrix;
    uniform mat4x4 _ClipToWorld;
};

layout(std140) uniform per_object_data
{
    uniform mat4x4 _ModelToWorldPerInstance[256];
};

layout(location = 0) in vec4 _position;

#ifdef ALPHA_TEST_ON
    // Use the main texture and texcoord for alpha testing
    layout(location = 3) in vec2 _texcoord;
    out vec2 texcoord;
#endif

void main()
{
    mat4x4 _ModelToWorld = _ModelToWorldPerInstance[gl_InstanceID];
    gl_Position = _ViewProjectionMatrix * (_ModelToWorld * _position);
    
#ifdef ALPHA_TEST_ON
    // Texcoord only needed for alpha test texture lookups
    texcoord = _texcoord;
#endif
}
