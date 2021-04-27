#version 130
//Vehicle vertex shader

in vec3 i_position;
in vec3 i_normal;
in vec2 i_texcoor;

out vec3 o_normal;
out vec2 o_texcoor;

out vec3 o_light;
out vec3 o_camera;

uniform mat4 u_proj;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat4 u_vehicle;
uniform mat3 u_normal;

uniform vec3 u_lightPos;
uniform vec3 u_cameraPos;

void main(){
  vec4 worldPosition = u_model * u_vehicle * vec4(i_position, 1.0f);
  gl_Position = u_proj * u_view * worldPosition;

  o_texcoor = i_texcoor;
  o_normal = normalize(u_normal * i_normal);

  o_light = normalize(u_lightPos);
  o_camera = normalize(u_cameraPos - worldPosition.xyz);
}
