#version 130
// basic fragment shader for bling-phong

// data from vertex shader
in vec2	o_uv;
in float	o_diffuse;
in vec3	o_specular;
in float o_solid;

// textures
uniform sampler2D u_diffuse;
uniform sampler2D u_specular;

// color output to the framebuffer
out vec4	FragColor;

void main()
{
  vec4 color = texture(u_diffuse, o_uv).rgba;
  // Ambient
  if(o_solid > 0.0f){
    vec3 ambient = 0.3f * color.rgb;
    vec3 diffuse = 0.8f * o_diffuse * color.rgb;
    vec3 specular = o_specular * texture(u_specular, o_uv).r;

    FragColor = vec4(ambient + specular + diffuse, color.a);
  }else{
    FragColor = vec4(0.8f, 0.8f, 0.8f, 1.0f) * color;
  }
}
