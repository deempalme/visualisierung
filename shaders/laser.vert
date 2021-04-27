#version 130
//laser vertex shader

in vec3 i_position;
in float i_intensity;
in vec3 i_color;
in float i_distance;
in float i_angle;
in float i_layer;

out vec4 o_color;

uniform mat4 u_matrix;
uniform mat4 u_vehicle;
uniform mat4 u_model;
uniform float u_numColors;  //this indicates the number of elements in the color palette
uniform bool u_masterColor; //this is to turn all clouds in grayscale
uniform bool u_range;    //true if the intesity range is from 0 to 255
uniform bool u_solid;       //if true the intensity will not affect the color
uniform bool u_colorize;    //if false it will be grayscale colored
uniform bool u_rgb;
uniform bool u_ignore;
uniform vec3 u_palette[10]; //this is the color palette
uniform float u_angularResolution;
uniform float u_verticalAngle;
uniform bool u_isPointDAI;

void main()
{
 if(u_isPointDAI){
  float o_angle = i_angle * u_angularResolution * 0.0174533;
  vec3 o_position = vec3(-i_distance * sin(o_angle), i_distance * sin(i_layer * u_verticalAngle), -i_distance * cos(o_angle));
  gl_Position = u_matrix * u_vehicle * u_model * vec4(o_position, 1.0f);
 }else{
  gl_Position = u_matrix * u_vehicle * u_model * vec4(-i_position.y, i_position.z, -i_position.x, 1.0f);
 }

 float alpha = 1.0f;
 //the next section will calculate the color depending in the intensity
 float intensity = i_intensity;

 if(intensity > 255.0f) intensity = 255.0f;

 if(u_range)
  intensity /= 255.0f;

 vec3 tempCol;

 if(u_rgb && u_masterColor && !u_ignore){
  tempCol = i_color/255.0f;
  alpha = intensity * 0.7f + 0.3f;
 }else if(u_colorize && u_masterColor && (u_numColors <= 1.0f)){
  tempCol = u_palette[0];
  alpha = intensity * 0.7f + 0.3f;
 }else if(u_colorize && (u_numColors > 1.0f) && u_masterColor){
  int idx1 = 0;// Our desired color will be between these two indexes in "color".
  int idx2 = 0;
  float fractBetween = 0.0f;// Fraction between "idx1" and "idx2" where our value is.

  if(intensity <= 0.0f){
   idx1 = idx2 = 0;
  }else if(intensity >= 1.0f){
   idx1 = idx2 = int(u_numColors) - 1;
  }else{
   intensity = intensity * (u_numColors - 1.0f);
   idx1 = int(floor(intensity));// Our desired color will be after this index.
   idx2 = idx1 + 1;// ... and before this index (inclusive).
   fractBetween = intensity - float(idx1);// Distance between the two indexes (0-1).
  }

  tempCol = (u_palette[idx2] - u_palette[idx1]) * fractBetween + u_palette[idx1];
  alpha = 1.0f;
 }else{
  alpha = intensity * 0.9f + 0.1f;
  tempCol = vec3(1.0f);
 }
 alpha = (u_solid && u_masterColor)? 1.0f : alpha;
 o_color = vec4(tempCol, alpha);
}
