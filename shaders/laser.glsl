//laser geometry shader
#version 130
in vec3 i_position;
in vec3 i_color;

out vec4 o_color;

uniform mat4 u_matrix;
uniform mat4 u_model;

void main()
{
  gl_Position = u_matrix * u_model * vec4(i_position, 1.0f);
  o_color = vec4(i_color, 1.0f);
}
