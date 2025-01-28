#version 430

#ifdef COMPUTE_SHADER

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

//positions is not really useful + Easily inferable
layout(binding= 0, rgba32f) readonly uniform image2D positions;
//32f might be overkill
layout(binding= 1, rgba32f) readonly uniform image2D colors;
layout(binding= 2, r32f) readonly uniform image2D zbuffer;
layout(binding= 3, rgba32f) readonly uniform image2D normals;
layout(binding= 4, rgba32f) writeonly uniform image2D outputtexture;
uniform mat4 mvpMatrix;

void main( )
{  
    
    ivec2 curr_pix= ivec2(gl_GlobalInvocationID.xy);
    vec3 light_dir = normalize(vec3(1.0, 1.0, 1.0)); 

 
    // Get G-BuffersTextures's data
    //vec4 frag_position = imageLoad(positions, curr_pix);
    vec4 frag_normal = imageLoad(normals, curr_pix);
    vec4 frag_color = imageLoad(colors, curr_pix);
    float frag_depth = imageLoad(zbuffer, curr_pix).x;
   

    //Color Calculation
    vec3 normal = normalize(frag_normal.xyz); 
    //This step also deal with light
    float cos = max(dot(normal, light_dir), 0.0); 
    vec3 final_color = frag_color.rgb * cos;
    imageStore(outputtexture,curr_pix , vec4(final_color,1.0));
 }


#endif
