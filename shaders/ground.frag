#version 130
//Ground fragment shader

in vec4 o_position;
in vec3 o_color;
in vec3 o_normal;

in float o_lines;

in vec3 o_light;
in vec3 o_camera;

out vec4 l_FragColor;

const float FogDensity = 0.1f;

/////////////////////////////////////////////////////////

void main()
{
  float dist = 0;
  float fogFactor = 0;

  //range based
  dist = length(o_position) - 45.0f;
  dist = (dist < 0.0f)? 0 : dist;

  //exponential fog
  fogFactor = 1.0 /exp(dist * FogDensity);
  fogFactor = clamp(fogFactor, 0.0, 1.0);

  if(o_lines == 0.0f){

    // get Blinn-Phong reflectance components
    float Iamb = 0.3f;

    // calculation as for Lambertian reflection
    float Idif = 0.7f * max(dot(o_normal, o_light), 0.0f);

    float Ispe = 0.0f;
    // calculate specular reflection only if
    // the surface is oriented to the light source
    //if(dot(o_normal, o_light) > 0)  {
      // half vector
      vec3 H = normalize(o_light + o_camera);
      Ispe = 0.5f * pow(max(dot(o_normal, H), 0.0f), 32.0f);
    //}

    // combination of all components and diffuse color of the object
    //if((Iamb + Idif + Ispe) > 1)
      //l_FragColor.rgb = o_color;
    //else
      l_FragColor.rgb = o_color * (Iamb + Idif + Ispe);
  }else{
    l_FragColor.rgb = o_color;
  }
/*
  if(dist > 40.0f)
    l_FragColor.rgb = vec3(1.0f);
  else
    l_FragColor.rgb = vec3(0.2f);
    */
  l_FragColor.a = fogFactor;
}
