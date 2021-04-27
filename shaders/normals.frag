#version 130
// basic fragment shader for normal bump

// data from vertex shader
in vec2	o_uv;
in vec3	o_toLightInTangentSpace;
in vec3	o_toCameraInTangentSpace;

// textures
uniform sampler2D u_diffuse;
uniform sampler2D u_normal;
uniform sampler2D u_specular;

// color output to the framebuffer
out vec4	fragColor;

//////////////////////////////////////////////////////


void main(void){

    // Obtain normal from normal map in range [0,1]
    vec3 normal = texture(u_normal, o_uv).rgb;
    // Transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0f - 1.0f);  // this normal is in tangent space

    // Get diffuse color
    vec4 color = texture(u_diffuse, o_uv).rgba;
    // Ambient
    vec3 ambient = 0.3f * color.rgb;
    // Diffuse
    float diff = max(dot(o_toLightInTangentSpace, normal), 0.0f);
    vec3 diffuse = 0.8f * diff * color.rgb;
    // Specular
    vec3 reflectDir = reflect(-o_toLightInTangentSpace, normal);
    vec3 halfwayDir = normalize(o_toLightInTangentSpace + o_toCameraInTangentSpace);
    float spec = pow(max(dot(normal, halfwayDir), 0.0f), 32.0f);
    vec3 specular = vec3(0.4f) * spec * texture(u_specular, o_uv).r;

    fragColor = vec4(ambient + diffuse + specular, color.a);
    //fragColor = vec4(diffuse, color.a);
}
