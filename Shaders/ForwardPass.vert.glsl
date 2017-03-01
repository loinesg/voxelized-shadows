#version 330

// Scene uniform buffer
// Used for camera position.
layout(std140) uniform scene_data
{
    uniform vec2 _ScreenResolution;
    uniform vec3 _CameraPosition;
    uniform vec3 _AmbientColor;
    uniform vec3 _LightColor;
    uniform vec3 _LightDirection;
};

// Per-object uniform buffer.
layout(std140) uniform per_object_data
{
    uniform mat4x4 _ModelToWorld;
    uniform mat4x4 _ModelViewProjection;
};

// Vertex attributes
layout(location = 0) in vec4 _position;
layout(location = 1) in vec3 _normal;
layout(location = 2) in vec4 _tangent;
layout(location = 3) in vec2 _texcoord;

#ifdef SPECULAR_ON
    out vec3 viewDir;
#endif

#ifdef NORMAL_MAP_ON
    // Tangent to world space basis
    // Contains (wTangent, wBiTangent, wNormal)
    out vec3 tangentToWorldX;
    out vec3 tangentToWorldY;
    out vec3 tangentToWorldZ;
#else
    // No normal mapping. Calculate the world normal directly
    out vec3 worldNormal;
#endif

out vec2 texcoord;

void main()
{
    gl_Position = _ModelViewProjection * _position;
    
#ifdef SPECULAR_ON
    // Send the view direction to the fragment shader
    // Used for BlinnPhong specular lighting.
    vec4 worldPosition = _ModelToWorld * _position;
    viewDir = normalize(_CameraPosition - worldPosition.xyz);
#endif
    
#ifdef NORMAL_MAP_ON
    // Get the normal, tangent and bitangent in world space
    vec3 worldNormal = normalize(mat3(_ModelToWorld) * _normal);
    vec3 worldTangent = normalize(mat3(_ModelToWorld) * _tangent.xyz);
    vec3 worldBitangent = cross(worldNormal, worldTangent) * _tangent.w;
    
    // Construct a (worldtangent, worldnormal, worldbitangent) basis
    // Used in the fragment shader to change the tangent space normal to world space.
    tangentToWorldX = vec3(worldTangent.x, worldBitangent.x, worldNormal.x);
    tangentToWorldY = vec3(worldTangent.y, worldBitangent.y, worldNormal.y);
    tangentToWorldZ = vec3(worldTangent.z, worldBitangent.z, worldNormal.z);
#else
    // No normal mapping. Send the world space normal directly to the fragment shader.
    worldNormal = normalize(mat3(_ModelToWorld) * _normal);
#endif
    
    // Texcoord does not need to be modified.
    texcoord = _texcoord;
}
