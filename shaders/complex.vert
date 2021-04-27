#version 130
// Basic vertex shader for parallax mapping

// attributes
in vec3	i_position;	// xyz - position
in vec2	i_uv;	// xy - texture coords
in vec3	i_normal;	// xyz - normal
in vec3 i_tangent; // xyz - tangent, w - handedness
in vec3 i_bitangent;

// uniforms
uniform mat4 u_model_mat;
uniform mat4 u_matrix;
uniform mat3 u_normal_mat;
uniform vec3 u_light_position;
uniform vec3 u_camera_position;
uniform mat4 u_vehicle;
uniform mat3 u_vehicle_normal;
uniform bool u_static;

// data for fragment shader
out vec2	o_uv;
out vec3 o_toLightInTangentSpace;
out vec3 o_toCameraInTangentSpace;

///////////////////////////////////////////////////////////////////

void main(void){

  // transform to world space
  mat3 u_normal_m;
  vec4 worldPosition;

  if(u_static){
    u_normal_m = u_normal_mat;
    worldPosition	= u_model_mat * vec4(i_position, 1);
  }else{
    u_normal_m = u_vehicle_normal * u_normal_mat;
    worldPosition	= u_vehicle * u_model_mat * vec4(i_position, 1);
  }
  vec3 worldNormal	= normalize(u_normal_m * i_normal);
  vec3 worldTangent	= normalize(u_normal_m * i_tangent);
  vec3 worldBitangent	= normalize(u_normal_m * i_bitangent);

  // calculate vectors to the camera and to the light
  //vec3 worldDirectionToLight	= normalize(u_light_position - worldPosition.xyz);
  vec3 worldDirectionToLight	= normalize(u_light_position);
  vec3 worldDirectionToCamera	= normalize(u_camera_position - worldPosition.xyz);

  // transform direction to the light to tangent space
  o_toLightInTangentSpace = vec3(dot(worldDirectionToLight, worldTangent),
                                 dot(worldDirectionToLight, worldBitangent),
                                 dot(worldDirectionToLight, worldNormal));

  // transform direction to the camera to tangent space
  o_toCameraInTangentSpace= vec3(dot(worldDirectionToCamera, worldTangent),
                                 dot(worldDirectionToCamera, worldBitangent),
                                 dot(worldDirectionToCamera, worldNormal));

  // pass texture coordinates to fragment shader
  o_uv	= i_uv;

  // calculate screen space position of the vertex
  gl_Position	= u_matrix * worldPosition;
}
