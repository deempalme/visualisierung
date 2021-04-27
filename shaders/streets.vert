#version 130
//Blinn-phong lighting vertex for streets

in vec3 i_position;
in vec2 i_translation;
in vec2 i_texcoor;
in float i_texture;

out vec3 o_position;
out vec2 o_texcoor;
out float o_texture;

uniform mat4 u_matrix;
uniform mat4 u_model;
uniform mat4 u_rotation;
uniform mat4 u_mapModel;

void main()
{
  vec4 e_position = u_rotation * vec4(i_position, 1.0f);
  vec4 m_position = u_model * u_mapModel * vec4(e_position.x + i_translation.x, e_position.y,
                                   e_position.z + i_translation.y, 1.0f);
  gl_Position = u_matrix * m_position;
  o_position = vec3(m_position);
  o_texture = i_texture;
  o_texcoor = i_texcoor;
}
