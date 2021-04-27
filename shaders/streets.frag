#version 130
//blinn-phong lighting fragment for streets

in vec3 o_position;
in vec2 o_texcoor;
in float o_texture;

out vec4 o_color;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool u_map;
uniform sampler2D diffuseMap1;
uniform sampler2D diffuseMap2;
uniform sampler2D diffuseMap3;
uniform sampler2D diffuseMap4;

void main()
{
  if(u_map){
    // Ambient
    vec4 ambient;
    float alpha = 1.0f;
    ambient = texture(diffuseMap1, o_texcoor).rgba;
    ambient.rgb *= 0.7f;
    // Diffuse
    //vec3 lightDir = normalize(lightPos - o_position);
    vec3 lightDir = normalize(lightPos);
    vec3 normal;
    normal = vec3(0, 1, 0);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse;
    diffuse = diff * ambient.rgb;
    // Specular
    vec3 viewDir = normalize(viewPos - o_position);
    float spec = 0.0;

    //blinn reflections
    //vec3 halfwayDir = normalize(lightDir + viewDir);
    //spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    //phong reflections
    vec3 reflectDir = reflect(-lightDir, normal);
    spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);

    vec3 specular = vec3(0.3f) * spec; // assuming bright white light color
    o_color = vec4(ambient.rgb + specular + diffuse, ambient.a);

  }else{
    if(o_texture == 0.0f)
      o_color = texture(diffuseMap1, o_texcoor).rgba;
    else if(o_texture == 1.0f)
      o_color = texture(diffuseMap2, o_texcoor).rgba;
    else if(o_texture == 2.0f)
      o_color = texture(diffuseMap3, o_texcoor).rgba;
    else if(o_texture == 3.0f)
      o_color = texture(diffuseMap4, o_texcoor).rgba;
  }
}
