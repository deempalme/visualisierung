// Basic vertex shader for parallax mapping
#version 130

// attributes
in vec3 i_position; // xyz - position
in vec3 i_normal; // xyz - normal
in vec2 i_texcoor; // xy - texture coords
in vec3 i_tangent; // xyz - tangent
in vec3 i_bitangent; // xyz - bitangent

// uniforms
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;
uniform mat3 u_normal;
uniform vec3 u_light;
uniform vec3 u_camera;

// data for fragment shader
out vec2 o_texcoords;
out vec3 o_toLightInTangentSpace;
out vec3 o_toCameraInTangentSpace;

///////////////////////////////////////////////////////////////////

void main(void)
{
  // transform to world space
  vec4 worldPosition = u_model * vec4(i_position, 1);
  vec3 worldNormal = normalize(u_normal * i_normal);
  vec3 worldTangent = normalize(u_normal * i_tangent);
  vec3 worldBitangent = normalize(u_normal * i_bitangent);

  // calculate vectors to the camera and to the light
  vec3 worldDirectionToLight = normalize(u_light - worldPosition.xyz);
  vec3 worldDirectionToCamera = normalize(u_camera - worldPosition.xyz);

  // transform direction to the light to tangent space
  o_toLightInTangentSpace = vec3(
        dot(worldDirectionToLight, worldTangent),
        dot(worldDirectionToLight, worldBitangent),
        dot(worldDirectionToLight, worldNormal)
        );

  // transform direction to the camera to tangent space
  o_toCameraInTangentSpace= vec3(
        dot(worldDirectionToCamera, worldTangent),
        dot(worldDirectionToCamera, worldBitangent),
        dot(worldDirectionToCamera, worldNormal)
        );

  // pass texture coordinates to fragment shader
  o_texcoords = i_texcoor;

  // calculate screen space position of the vertex
  gl_Position = u_proj * u_view * worldPosition;
}
