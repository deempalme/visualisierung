#version 130
//blinn-phong lighting fragment for objects

in vec3 o_position;
in vec3 o_normal;
in vec3 o_color;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
  // Ambient
  vec3 ambient;
  float alpha = 1.0f;
  ambient = 0.4 * o_color;
  // Diffuse
  //vec3 lightDir = normalize(lightPos - o_position);
  vec3 lightDir = normalize(lightPos);
  float diff = max(dot(lightDir, o_normal), 0.0);
  vec3 diffuse;
  diffuse = diff * o_color;
  // Specular
  vec3 viewDir = normalize(viewPos - o_position);
  float spec = 0.0;

  //blinn reflections
  //vec3 halfwayDir = normalize(lightDir + viewDir);
  //spec = pow(max(dot(o_normal, halfwayDir), 0.0), 32.0);

  //phong reflections
  vec3 reflectDir = reflect(-lightDir, o_normal);
  spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);

  vec3 specular = vec3(0.3f) * spec; // assuming bright white light color
  FragColor = vec4(ambient + specular + diffuse, alpha);
}
