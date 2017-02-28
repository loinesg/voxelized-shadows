#version 330

layout(location = 0) in vec4 _position;

out vec2 texcoord;

void main()
{
    // Fullscreen quad. No need to modify position
    gl_Position = _position;
    
    // Transform the position from [-1,1] to [0,1] for texcoord
    texcoord = _position.xy / 2.0 + 0.5;
}
