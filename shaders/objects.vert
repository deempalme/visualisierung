#version 130
//Obstacles vertex shader

in vec3 i_position;
in vec3 i_color;
in vec3 i_normal;

out vec3 o_position;
out vec3 o_normal;
out vec3 o_color;

uniform mat4 u_matrix;
uniform mat4 u_model;
uniform mat4 u_vehicle;

void main()
{
  vec4 m_position = u_vehicle * u_model * vec4(i_position, 1.0f);
  gl_Position = u_matrix * m_position;
  o_position = vec3(m_position);
  o_normal = i_normal;
  o_color = i_color;
}
