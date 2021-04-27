#version 130
//Vehicle fragment shader

in vec3 o_normal;
in vec2 o_texcoor;

in vec3 o_light;
in vec3 o_camera;

out vec4 FragColor;

uniform int u_type;
uniform sampler2D diffusePEM;
uniform sampler2D diffuseDHL;
uniform sampler2D diffuseNOONE;

void main(){
  // Ambient
  vec3 o_color;
  float alpha = 0.5f;

  if(u_type == 0)
    o_color = vec3(0.067, 0.325, 0.557);
  else if(u_type == 1)
    o_color = texture(diffuseDHL, o_texcoor).rgb;
  else
    o_color = vec3(0.349, 0.4, 0.447);

  // get Blinn-Phong reflectance components
  float Iamb = 0.3f;

  // calculation as for Lambertian reflection
  float Idif = 0.7f * max(dot(o_normal, o_light), 0.0f);

  float Ispe = 0.0f;
  // calculate specular reflection only if
  // the surface is oriented to the light source
  vec3 H = normalize(o_light + o_camera);
  Ispe = 0.5f * pow(max(dot(o_normal, H), 0.0f), 32.0f);

  FragColor.rgb = o_color * (Iamb + Idif + Ispe);
  FragColor.a = alpha;
}
