#version 330

layout(location = 0) in vec4 _position;

out vec2 texcoord;

void main()
{
    // Using a full screen quad. No need for transformations.
    gl_Position = _position;
    
    // Derive the texcoord from the screen position.
    texcoord = _position.xy / 2.0 + 0.5;
}
