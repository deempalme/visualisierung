#version 130
// Basic vertex shader for bling-phong

// attributes
in vec3	i_position;	// xyz - position
in vec3	i_normal;	// xyz - normal
in vec2	i_uv;	// xy - texture coords

// uniforms
uniform mat4 u_model_mat;
uniform mat4 u_model_mat2;
uniform mat4 u_matrix;
uniform mat3 u_normal_mat;
uniform mat3 u_normal_mat2;
uniform vec3 u_light_position;
uniform vec3 u_camera_position;
uniform mat4 u_vehicle;
uniform mat3 u_vehicle_normal;

// data for fragment shader
out vec2	o_uv;
out float o_diffuse;
out vec3 o_specular;
out float o_solid;

///////////////////////////////////////////////////////////////////

void main(void){

  // transform to world space
  mat3 u_normal_m;
  vec4 worldPosition;

  u_normal_m = u_vehicle_normal * u_normal_mat * u_normal_mat2;
  worldPosition	= u_vehicle * u_model_mat * u_model_mat2 * vec4(i_position, 1);
  vec3 normal	= normalize(u_normal_m * i_normal);

  // calculate vectors to the camera and to the light
  vec3 lightDirection	= normalize(u_light_position);
  vec3 cameraDirection	= normalize(u_camera_position - worldPosition.xyz);

  o_diffuse = max(dot(lightDirection, normal), 0.0);
  o_specular = vec3(0.7) * pow(max(dot(cameraDirection, reflect(-lightDirection, normal)), 0.0), 8.0);

  // pass texture coordinates to fragment shader
  o_uv	= i_uv;

//  if(i_uv.x >= 0.49f && i_uv.x <= 0.75 && i_uv.y >= 0.82 && i_uv.y <= 0.91)
//    o_solid = -1.0f;
//  else
    o_solid  = 1.0f;

  // calculate screen space position of the vertex
  gl_Position	= u_matrix * worldPosition;
}
