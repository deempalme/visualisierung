#version 130
// Basic vertex shader for skybox

// attributes
in vec3 i_position; // xyz - position

// uniforms
uniform mat4 u_pvm;

// data for fragment shader
out vec3 o_texcoords;

///////////////////////////////////////////////////////////////////

void main(void)
{
  // calculate screen space position of the vertex
  gl_Position = u_pvm * vec4(i_position, 1);
  o_texcoords = i_position;
}
