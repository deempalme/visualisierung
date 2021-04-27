//Obstacles geometry shader
#version 150
in vec3 g_dimension;
in vec3 g_position;
in vec3 g_orientation;
in vec3 g_color;
in float g_width;
in mat4 g_matrix;

out vec3 o_position;
out vec3 o_normal;
out vec3 o_color;

uniform mat4 u_matrix;
uniform mat4 u_model;

void main()
{
  //color never changes per cube, so it is posible to define it from here
  o_color = g_color;

  //cube vertices (this cube has a size of 1 cubic meter)
  vec3 nP[8];
  nP[0] = vec3(-0.5f,-0.5f, 0.5f);
  nP[1] = vec3(-0.5f, 0.5f, 0.5f);
  nP[2] = vec3( 0.5f, 0.5f, 0.5f);
  nP[3] = vec3( 0.5f,-0.5f, 0.5f);
  nP[4] = vec3( 0.5f,-0.5f,-0.5f);
  nP[5] = vec3( 0.5f, 0.5f,-0.5f);
  nP[6] = vec3(-0.5f, 0.5f,-0.5f);
  nP[7] = vec3(-0.5f,-0.5f,-0.5f);

  //this is a initial vector to calculate the line width
  //there are three different kind of faces: parallel to plane X, to plane Y and to plane Z
  vec3 nL[12];
  nL[0] = vec3( 1.0f,  1.0f, 0.0f);
  nL[1] = vec3( 1.0f, -1.0f, 0.0f);
  nL[2] = vec3(-1.0f, -1.0f, 0.0f);
  nL[3] = vec3(-1.0f,  1.0f, 0.0f);

  nL[4] = vec3(0.0f,  1.0f, -1.0f);
  nL[5] = vec3(0.0f, -1.0f, -1.0f);
  nL[6] = vec3(0.0f, -1.0f,  1.0f);
  nL[7] = vec3(0.0f,  1.0f,  1.0f);

  nL[8] = vec3(-1.0f, 0.0f, -1.0f);
  nL[9] = vec3( 1.0f, 0.0f, -1.0f);
  nL[10] = vec3(1.0f, 0.0f,  1.0f);
  nL[11] = vec3(-1.0f,0.0f,  1.0f);

  //creation of an extra array to indicate which vertices constitute face corners for a cube
  //       6 ------------- 5
  //      /|              /|
  //     / |             / |
  //    /  |            /  |
  //   /   |           /   |
  //  1 ------------- 2    |
  //  |    |          |    |
  //  |    7 ---------|--- 4
  //  |   /           |   /
  //  |  /            |  /
  //  | /             | /
  //  |/              |/
  //  0 ------------- 3
  float nF1[24];
  nF1[0] = 0.0f;  nF1[4] = 7.0f;   nF1[8] = 3.0f;  nF1[12] = 0.0f;  nF1[16] = 2.0f;  nF1[20] = 3.0f;
  nF1[1] = 1.0f;  nF1[5] = 6.0f;   nF1[9] = 2.0f;  nF1[13] = 1.0f;  nF1[17] = 1.0f;  nF1[21] = 0.0f;
  nF1[2] = 2.0f;  nF1[6] = 5.0f;  nF1[10] = 5.0f;  nF1[14] = 6.0f;  nF1[18] = 6.0f;  nF1[22] = 7.0f;
  nF1[3] = 3.0f;  nF1[7] = 4.0f;  nF1[11] = 4.0f;  nF1[15] = 7.0f;  nF1[19] = 5.0f;  nF1[23] = 4.0f;

  //this array defines which points will serve to create normal, tangent and bitangent lines for each face
  //int nF2[18] = {0,1,3, 4,5,7, 3,2,4, 7,6,0, 2,1,5, 0,3,7};
  float nF2[18];
  nF2[0] = 0.0f;  nF2[3] = 4.0f;  nF2[6] = 3.0f;   nF2[9] = 7.0f;  nF2[12] = 2.0f;  nF2[15] = 0.0f;
  nF2[1] = 3.0f;  nF2[4] = 7.0f;  nF2[7] = 2.0f;  nF2[10] = 6.0f;  nF2[13] = 1.0f;  nF2[16] = 3.0f;
  nF2[2] = 1.0f;  nF2[5] = 5.0f;  nF2[8] = 4.0f;  nF2[11] = 0.0f;  nF2[14] = 5.0f;  nF2[17] = 7.0f;

  //creation of an extra array to indicate which vertices create an entire face
  //4 extra vertices will be created per face, there are 6 faces.
  //the next vertices are per face, 8 vertices each face, this will be use for GL_TRIANGLE_STRIP
  //vertices: 0 - 3 are external, 4 - 7 are internal
  //  1 ------------------ 2
  //  | 5 -------------- 6 |
  //  | |                | |
  //  | |                | |
  //  | |                | |
  //  | |                | |
  //  | |                | |
  //  | |                | |
  //  | 4 -------------- 7 |
  //  0 ------------------ 3
  //the first and last point shall be repeated to indicate beginning and end of GL_TRIANGLE_STRIP
  float nF3[12];
  nF3[0] = 0.0f;  nF3[6] = 6.0f;
  nF3[1] = 0.0f;  nF3[7] = 3.0f;
  nF3[2] = 4.0f;  nF3[8] = 7.0f;
  nF3[3] = 1.0f;  nF3[9] = 0.0f;
  nF3[4] = 5.0f;  nF3[10] = 4.0f;
  nF3[5] = 2.0f;  nF3[11] = 4.0f;

  vec3 nN2[6];
  nN2[0] = vec3( 0.0f, 0.0f, 1.0f);
  nN2[1] = vec3( 1.0f, 0.0f, 0.0f);
  nN2[2] = vec3( 0.0f, 0.0f,-1.0f);
  nN2[3] = vec3(-1.0f, 0.0f, 0.0f);
  nN2[4] = vec3( 0.0f, 1.0f, 0.0f);
  nN2[5] = vec3( 0.0f,-1.0f, 0.0f);

  // ------------------------------------------------------------------------------------ //
  // ----------------------- Mathematical operations start here ------------------------- //
  // ------------------------------------------------------------------------------------ //

  for(int i = 0; i < 6; i++){
    int p1 = i * 4;
    int p4 = i * 8;

    //creates normal line for each face
    o_normal = nN2[i];
    o_normal = vec3(g_matrix * vec4(o_normal, 1.0f));

    //normalizing vectors
    o_normal = normalize(o_normal);

    //before this part only exists eight points per cube, it is necessary to have eight points per face
    //4 points represent the exterior vertices and 4 the interior vertices for a line width of variable "g_width"
    //the line thickness is represented by polygons, because of this it must exist 8 points per face
    //there are six faces per cube, therefore 48 points are created.
    for(int e = 0; e < 8; e++){
      int pe = p4 + e;
      int pf = p1 + e;
      pf = (e < 4)? pf : pf-4;
      o_position = nP[nF1[pf]];
      if(e >= 4){
        if(i < 2){
          o_position += (nL[e - 4] * g_width)/g_dimension;
        }else if(i < 4){
          o_position += (nL[e] * g_width)/g_dimension;
        }else{
          o_position += (nL[e + 4] * g_width)/g_dimension;
        }
      }
      //point scaling
      o_position *= g_dimension;
      //point rotation and transformation matrix
      o_position = vec3(g_matrix * vec4(o_position, 1.0f)) + g_position;
      gl_Position = u_matrix * u_model * vec4(o_position, 1.0f);
      EmitVertex();
    }
  }
  EndPrimitive();
}























//
