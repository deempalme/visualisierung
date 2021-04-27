#version 130
//Blinn-phong lighting vertex for trajectory

in vec3 i_position;
in vec3 i_color;
in vec2 i_texcoor;
in float i_type;

out vec3 o_position;
out vec3 o_color;
out vec2 o_texcoor;
out float o_type;

uniform mat4 u_matrix;
uniform mat4 u_model;
uniform mat4 u_model2;

void main(){
  vec4 m_position = u_model * u_model2 * vec4(i_position, 1.0f);
  gl_Position = u_matrix * m_position;
  o_position = vec3(m_position);
  o_type = i_type;
  o_color = i_color/255.0f;
  o_texcoor = i_texcoor;
}
