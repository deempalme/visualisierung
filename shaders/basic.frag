// basic fragment shader for Parallax Mapping
#version 130

// data from vertex shader
in vec2 o_texcoords;
in vec3 o_toLightInTangentSpace;
in vec3 o_toCameraInTangentSpace;

// textures
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

// color output to the framebuffer
out vec4 resultingColor;

////////////////////////////////////////

// scale for size of Parallax Mapping effect
uniform float parallaxScale; // ~0.1

//////////////////////////////////////////////////////
// Implements Parallax Mapping technique
// Returns modified texture coordinates, and last used depth
vec2 parallaxMapping(in vec3 V, in vec2 T, out float parallaxHeight)
{
  // determine required number of layers
  const float minLayers = 10;
  const float maxLayers = 15;
  float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0, 0, 1), V)));

  // height of each layer
  float layerHeight = 1.0 / numLayers;
  // depth of current layer
  float currentLayerHeight = 0;
  // shift of texture coordinates for each iteration
  vec2 dtex = parallaxScale * V.xy / V.z / numLayers;

  // current texture coordinates
  vec2 currentTextureCoords = T;

  // depth from heightmap
  float heightFromTexture = texture(depthMap, currentTextureCoords).r;

  // while point is above surface
  while(heightFromTexture > currentLayerHeight){
    // go to the next layer
    currentLayerHeight += layerHeight;
    // shift texture coordinates along V
    currentTextureCoords -= dtex;
    // new depth from heightmap
    heightFromTexture = texture(depthMap, currentTextureCoords).r;
  }

  ///////////////////////////////////////////////////////////
  // Start of Relief Parallax Mapping

  // decrease shift and height of layer by half
  vec2 deltaTexCoord = dtex / 2;
  float deltaHeight = layerHeight / 2;

  // return to the mid point of previous layer
  currentTextureCoords += deltaTexCoord;
  currentLayerHeight -= deltaHeight;

  // binary search to increase precision of Steep Paralax Mapping
  const int numSearches = 5;
  for(int i=0; i<numSearches; i++){
    // decrease shift and height of layer by half
    deltaTexCoord /= 2;
    deltaHeight /= 2;

    // new depth from heightmap
    heightFromTexture = texture(depthMap, currentTextureCoords).r;

    // shift along or agains vector V
    if(heightFromTexture > currentLayerHeight){ // below the surface
      currentTextureCoords -= deltaTexCoord;
      currentLayerHeight += deltaHeight;
    }else{ // above the surface
      currentTextureCoords += deltaTexCoord;
      currentLayerHeight -= deltaHeight;
    }
  }

  // return results
  parallaxHeight = currentLayerHeight;
  return currentTextureCoords;
}

//////////////////////////////////////////////////////
// Implements self-shadowing technique - hard or soft shadows
// Returns shadow factor
float parallaxSoftShadowMultiplier(in vec3 L, in vec2 initialTexCoord, in float initialHeight)
{
  float shadowMultiplier = 1;

  const float minLayers = 15;
  const float maxLayers = 30;

  // calculate lighting only for surface oriented to the light source
  if(dot(vec3(0, 0, 1), L) > 0)
  {
    // calculate initial parameters
    float numSamplesUnderSurface = 0;
    shadowMultiplier = 0;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0, 0, 1), L)));
    float layerHeight = initialHeight / numLayers;
    vec2 texStep = parallaxScale * L.xy / L.z / numLayers;

    // current parameters
    float currentLayerHeight = initialHeight - layerHeight;
    vec2 currentTextureCoords = initialTexCoord + texStep;
    float heightFromTexture = texture(depthMap, currentTextureCoords).r;
    int stepIndex = 1;

    // while point is below depth 0.0 )
    while(currentLayerHeight > 0)
    {
      // if point is under the surface
      if(heightFromTexture < currentLayerHeight)
      {
        // calculate partial shadowing factor
        numSamplesUnderSurface += 1;
        float newShadowMultiplier = (currentLayerHeight - heightFromTexture) *
            (1.0 - stepIndex / numLayers);
        shadowMultiplier = max(shadowMultiplier, newShadowMultiplier);
      }

      // offset to the next layer
      stepIndex += 1;
      currentLayerHeight -= layerHeight;
      currentTextureCoords += texStep;
      heightFromTexture = texture(depthMap, currentTextureCoords).r;
    }

    // Shadowing factor should be 1 if there were no points under the surface
    if(numSamplesUnderSurface < 1){
      shadowMultiplier = 1;
    }else{
      shadowMultiplier = 1.0 - shadowMultiplier;
    }
  }
  return shadowMultiplier;
}

//////////////////////////////////////////////////////
// Calculates lighting by Blinn-Phong model and Normal Mapping
// Returns color of the fragment
vec4 normalMappingLighting(in vec2 T, in vec3 L, in vec3 V, float shadowMultiplier)
{
  // restore normal from normal map
  vec3 N = normalize(texture(normalMap, T).xyz * 2 - 1);
  vec3 D = texture(diffuseMap, T).rgb;

  // diffuse lighting
  float dotNL = dot(N, L);
  float idiff = clamp(dotNL, 0, 1);
  // specular lighting
  float ispec = 0;
  if(dotNL > 0.2){
    vec3 R = reflect(-L, N);
    //ispec = pow(dot(R, V), 32.0) * 0.3;
    //ispec = pow(max(dot(R, V), 0.0), 32) * 0.3;
    //vec3 H = normalize(L + V);
    ispec = pow(dot(N, R), 32.0) * 0.5;
    //ispec = clamp(ispec, -0.05, 0.05);
  }

  vec4 resColor;
  vec3 ambientLighting = vec3(0.3f);
  resColor.rgb = D * (ambientLighting + (idiff + ispec) * pow(shadowMultiplier, 4));
  resColor.a = texture(diffuseMap, T).a;
  //if(dotNL <= 0.2)
    //resColor.rgb = vec3(1.0f);

  return resColor;
}

/////////////////////////////////////////////
// Entry point for Parallax Mapping shader
void main(void)
{
  // normalize vectors after vertex shader
  vec3 V = normalize(o_toCameraInTangentSpace);
  vec3 L = normalize(o_toLightInTangentSpace);

  // get new texture coordinates from Parallax Mapping
  float parallaxHeight;
  vec2 T = parallaxMapping(V, o_texcoords, parallaxHeight);

  // get self-shadowing factor for elements of parallax
  float shadowMultiplier = parallaxSoftShadowMultiplier(L, T, parallaxHeight - 0.05);

  // calculate lighting
  resultingColor = normalMappingLighting(T, L, V, shadowMultiplier);
}
