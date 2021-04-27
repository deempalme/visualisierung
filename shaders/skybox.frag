#version 130
// basic fragment shader for Skybox

in vec3 o_texcoords;
uniform samplerCube skybox;
out vec4 FragColor;

void main(void)
{
  FragColor = texture(skybox, o_texcoords).rgba;
}
