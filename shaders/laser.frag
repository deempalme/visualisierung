#version 130
//laser shader

in vec4 o_color;

out vec4 l_FragColor;

void main()
{
  l_FragColor = o_color;
}
