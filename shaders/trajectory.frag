#version 130
//blinn-phong lighting fragment for trajectory

in vec3 o_position;
in vec3 o_color;
in vec2 o_texcoor;
in float o_type;

out vec4 out_color;

uniform vec3 u_light;
uniform vec3 u_camera;
uniform sampler2D u_texture1;
uniform sampler2D u_texture2;

void main()
{
  // Ambient
  vec4 ambient;
  ambient = mix(texture(u_texture1, o_texcoor).rgba, texture(u_texture2, o_texcoor).rgba, o_type);
  ambient.r = ambient.r * o_color.r;
  ambient.g = ambient.g * o_color.g;
  ambient.b = ambient.b * o_color.b;

  // Diffuse
  //vec3 lightDir = normalize(u_light - o_position);
  vec3 lightDir = normalize(u_light);
  vec3 normal;
  normal = vec3(0, 1, 0);
  float diff = max(dot(lightDir, normal), 0.0);
  vec3 diffuse = diff * ambient.rgb;
  // Specular
  vec3 viewDir = normalize(u_camera - o_position);
  float spec = 0.0;

  ambient.rgb *= 0.3f;

  //blinn reflections
  //vec3 halfwayDir = normalize(lightDir + viewDir);
  //spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

  //phong reflections
  vec3 reflectDir = reflect(-lightDir, normal);
  spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);

  vec3 specular = vec3(0.3f) * spec; // assuming bright white light color

  vec3 result = clamp(ambient.rgb + specular + diffuse, 0.0f, 1.0f);
  out_color = vec4(result, ambient.a);
}
