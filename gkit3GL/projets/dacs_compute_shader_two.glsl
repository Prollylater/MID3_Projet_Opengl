#version 430
#define WARP_WIDTH 32
#ifdef COMPUTE_SHADER

float gray(vec3 color) {
  return float(0.21) * color.r + float(0.71) * color.g + float(0.08) * color.b;
}

bool shading_test(vec3 s, vec3 ss) {
  float v = gray(ss - s * s);
  return (v > 0.001);
}

uniform uint phase;
uniform uint block_step;
uniform int pattern_radius;

uniform uint row_pixels;
uniform uint column_pixels;


layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

//32f might be overkill
layout(binding = 0, rgba32f) readonly uniform image2D colors;
layout(binding = 1, r32f) readonly uniform image2D zbuffer;
layout(binding = 2, rgba32f) readonly uniform image2D normals;
layout(binding = 3, rgba32f) writeonly uniform image2D outputtexture;

//stores how many threads are active for the current operation
shared uint op_active;
shared uint col_offset = 0;

void shading(ivec2 curr_pix) {

  /*vec3 light_dir = normalize(vec3(1.0, 1.0, 1.0)); 

 
    // Get G-BuffersTextures's data
    vec4 frag_normal = imageLoad(normals, curr_pix);
    vec4 frag_color = imageLoad(colors, curr_pix);
    float frag_depth = imageLoad(zbuffer, curr_pix).x;
   

    //Color Calculation
    vec3 normal = normalize(frag_normal.xyz); 
    //This step also deal with light
    float cos = max(dot(normal, light_dir), 0.0); 
    vec3 final_color = frag_color.rgb * cos;
    imageStore(outputtexture,curr_pix , vec4(final_color,1.0));*/
  imageStore(outputtexture, curr_pix, vec4(1.0, 0.0, 0.0, 1.0));
}


vec3 clampLoad(ivec2 coord) {
    coord.x = clamp(coord.x, 0, int(column_pixels) - 1);
    coord.y = clamp(coord.y, 0, int(row_pixels) - 1);
    return imageLoad(colors, coord).rgb;
}

 
void processPatternPlusa(ivec2 curr_pix, int sample_radius) {
    int left = curr_pix.x - sample_radius;
    int right = curr_pix.x + sample_radius;
    int up = curr_pix.y + sample_radius;
    int down = curr_pix.y - sample_radius;

    
    vec3 a = clampLoad( ivec2(left,curr_pix.y));
        vec3 b = clampLoad( ivec2(right,curr_pix.y)) ;
        vec3 c = clampLoad( ivec2(curr_pix.x,up));
        vec3 d = clampLoad( ivec2(curr_pix.x,down));
    vec3 s = (a + b + c + d) / 4.0;
    vec3 ss = (a*a + b*b + c*c + d*d) / 4.0;

    if (shading_test(s, ss)) {
        imageStore(outputtexture, curr_pix, vec4(1.0, 0.0, 0.0, 1.0));
    } else {
        imageStore(outputtexture, curr_pix, vec4(0.5, 0.0, 0.0, 1.0));
    }
}

void processPatternPlusb(ivec2 curr_pix, int sample_radius) {
    int left = curr_pix.x - sample_radius;
    int right = curr_pix.x + sample_radius;
    int up = curr_pix.y + sample_radius;
    int down = curr_pix.y - sample_radius;

    
    vec3 a = clampLoad( ivec2(left,curr_pix.y));
        vec3 b = clampLoad( ivec2(right,curr_pix.y)) ;
        vec3 c = clampLoad( ivec2(curr_pix.x,up));
        vec3 d = clampLoad( ivec2(curr_pix.x,down));
    vec3 s = (a + b + c + d) / 4.0;
    vec3 ss = (a*a + b*b + c*c + d*d) / 4.0;

    if (shading_test(s, ss)) {
        imageStore(outputtexture, curr_pix, vec4(0.0, 1.0, 0.0, 1.0));
    } else {
        imageStore(outputtexture, curr_pix, vec4(0.0, 0.0, 0.0, 1.0));
    }
}

void processPatternCrossa(ivec2 curr_pix, int sample_radius) {
    int left = curr_pix.x - sample_radius;
    int right = curr_pix.x + sample_radius;
    int up = curr_pix.y + sample_radius;
    int down = curr_pix.y - sample_radius;

    vec3 a = clampLoad(ivec2(left, up));
    vec3 b = clampLoad(ivec2(right, up));
    vec3 c = clampLoad(ivec2(left, down));
    vec3 d = clampLoad(ivec2(right, down));

    vec3 s = (a + b + c + d) / 4.0;
    vec3 ss = (a*a + b*b + c*c + d*d) / 4.0;

    if (shading_test(s, ss)) {
        imageStore(outputtexture, curr_pix, vec4(1.0, 0.0, 1.0, 1.0));
    } else {
        imageStore(outputtexture, curr_pix, vec4(0.5, 0.0, 0.5, 1.0));
    }
}


void processPatternCrossb(ivec2 curr_pix, int sample_radius) {
    int left = curr_pix.x - sample_radius;
    int right = curr_pix.x + sample_radius;
    int up = curr_pix.y + sample_radius;
    int down = curr_pix.y - sample_radius;

    vec3 a = clampLoad(ivec2(left, up));
    vec3 b = clampLoad(ivec2(right, up));
    vec3 c = clampLoad(ivec2(left, down));
    vec3 d = clampLoad(ivec2(right, down));

    vec3 s = (a + b + c + d) / 4.0;
    vec3 ss = (a*a + b*b + c*c + d*d) / 4.0;

    if (shading_test(s, ss)) {
        imageStore(outputtexture, curr_pix, vec4(0.0, 1.0, 1.0, 1.0));
    } else {
        imageStore(outputtexture, curr_pix, vec4(0.0, 0.5, 0.5, 1.0));
    }
}
      
void main() {

  ivec2 global_id= ivec2(gl_GlobalInvocationID.xy);
  uint group_id = gl_WorkGroupID.x * column_pixels + gl_WorkGroupID.y;
  ivec2 curr_pix= ivec2((global_id.x*block_step),(global_id.y*block_step));
   

 if(phase <3){
    shading(curr_pix);

  //Phase1
       curr_pix.x += pattern_radius;  
       curr_pix.y += pattern_radius;  
       processPatternCrossa(curr_pix,pattern_radius);
    //Phase2
    
        curr_pix.y -= pattern_radius;  
        processPatternPlusa(curr_pix,pattern_radius);
        curr_pix.x -= pattern_radius;  
        curr_pix.y += pattern_radius;  
        processPatternPlusa(curr_pix,pattern_radius);
  }

   if(phase  >= 3){

    //Phase3
        curr_pix.x += pattern_radius;  
       curr_pix.y += pattern_radius;  
       processPatternCrossb(curr_pix,pattern_radius);
    //Phase4
        curr_pix.y -= pattern_radius;  
        processPatternPlusb(curr_pix,pattern_radius);
        curr_pix.x -= pattern_radius;  
        curr_pix.y += pattern_radius;  
        processPatternPlusb(curr_pix,pattern_radius);
   }

}

#endif