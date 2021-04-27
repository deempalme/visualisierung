#version 130
//Ground vertex shader

in vec3 i_position;
in vec3 i_color;
in vec3 i_normal;

out vec3 o_color;
out vec3 o_normal;
out vec4 o_position;

out float o_lines;

out vec3 o_light;
out vec3 o_camera;

uniform mat4 u_proj;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat4 u_model2;
uniform mat3 u_normal;
uniform mat3 u_normal2;

uniform bool u_lines;

uniform vec3 u_lightPos;
uniform vec3 u_cameraPos;


void main()
{
  vec4 worldPosition = u_model * u_model2 * vec4(i_position, 1.0f);
  o_position = u_view * worldPosition;
  gl_Position = u_proj * o_position;

  if(u_lines)
    o_color = vec3(0.3f, 0.3f, 0.3f);
  else{
    o_normal = normalize(u_normal * u_normal2 * i_normal);
    //o_light = normalize(u_lightPos - worldPosition.xyz);
    o_light = normalize(u_lightPos);
    o_camera = normalize(u_cameraPos - worldPosition.xyz);
    o_color = i_color/255.0f;
  }
  o_lines = float(u_lines);
}
