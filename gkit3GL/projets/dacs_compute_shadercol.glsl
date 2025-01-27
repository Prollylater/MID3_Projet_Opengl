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
uniform uint placement_step;

uniform uint row_pixels;
uniform uint column_pixels;


//Idea: One workgroup for each row or not
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
  imageStore(outputtexture, curr_pix, vec4(1.0, 1.0, 0.0, 1.0));
}

 
      
void main() {

  //uint local_index = gl_LocalInvocationIndex.x;
  ivec2 global_id= ivec2(gl_GlobalInvocationID.xy);

  
  uint group_id = gl_WorkGroupID.x * column_pixels + gl_WorkGroupID.y;
 

  if(phase == 0){
   ivec2 curr_pix= ivec2((global_id.x*block_step),(global_id.y*block_step));
     //ivec2 curr_pix = ivec2(block_step * (local_index),  block_step* group_id);
      shading(curr_pix);
  }
  if(phase == 1){
       ivec2 curr_pix = ivec2((block_step *global_id.x)+2,(block_step*global_id.y)+2);
        uint left = curr_pix.x-2;
        uint right = curr_pix.x+2;
        uint down = curr_pix.y-2;
        uint up = curr_pix.y+2;
        vec3 a = imageLoad(colors, ivec2(left,up)).xyz;
        vec3 b = imageLoad(colors, ivec2(right,up)).xyz ;
        vec3 c = imageLoad(colors, ivec2(left,down)).xyz;
        vec3 d = imageLoad(colors, ivec2(right,down)).xyz;
        vec3 s= ( a+b+c+d ) / 4;
        vec3 ss= ( a*a+b*b+c*c+d*d ) / 4;
        if (shading_test(s,ss)){
           shading(curr_pix);
        }
        else{
  imageStore(outputtexture, curr_pix, vec4(0.0, 1.0, 1.0, 1.0));
        }
  }

  if(phase == 2){
        ivec2 curr_pix = ivec2((block_step *global_id.x)+2,(block_step*global_id.y));

        uint left =curr_pix.x-2;
        uint right = curr_pix.x+2;
        uint down = clamp(curr_pix.y-2,0,row_pixels - 1);
        uint up = clamp(curr_pix.y+2,0,row_pixels - 1);
         vec3 a = imageLoad(colors, ivec2(left,curr_pix.y)).xyz;
        vec3 b = imageLoad(colors, ivec2(right,curr_pix.y)).xyz ;
        vec3 c = imageLoad(colors, ivec2(curr_pix.x,up)).xyz;
        vec3 d = imageLoad(colors, ivec2(curr_pix.x,down)).xyz;
        vec3 s= ( a+b+c+d ) / 4;
        vec3 ss= ( a*a+b*b+c*c+d*d ) / 4;
        if (shading_test(s,ss)){
           shading(curr_pix);
        }
        else{
  imageStore(outputtexture, curr_pix, vec4(0.0, 1.0, 1.0, 1.0));
        }

        //Second part
        curr_pix = ivec2((block_step *global_id.x),(block_step*global_id.y)+2);

       left = clamp(curr_pix.x-2,0,column_pixels - 1);
         right = clamp(curr_pix.x+2,0,column_pixels - 1);
         down = curr_pix.y-2;
         up = curr_pix.y+2;
          a = imageLoad(colors, ivec2(left,curr_pix.y)).xyz;
         b = imageLoad(colors, ivec2(right,curr_pix.y)).xyz ;
         c = imageLoad(colors, ivec2(curr_pix.x,up)).xyz;
         d = imageLoad(colors, ivec2(curr_pix.x,down)).xyz;
            s= ( a+b+c+d ) / 4;
         ss= ( a*a+b*b+c*c+d*d ) / 4;
        if (shading_test(s,ss)){
           shading(curr_pix);
        }
        else{
  imageStore(outputtexture, curr_pix, vec4(0.0, 1.0, 1.0, 1.0));
        }
  }
  if(phase == 3){
          ivec2 curr_pix = ivec2((block_step *global_id.x)+1,(block_step*global_id.y)+1);
        uint left = curr_pix.x-1;
        uint right = curr_pix.x+1;
        uint down = curr_pix.y-1;
        uint up = curr_pix.y+1;
        vec3 a = imageLoad(colors, ivec2(left,up)).xyz;
        vec3 b = imageLoad(colors, ivec2(right,up)).xyz ;
        vec3 c = imageLoad(colors, ivec2(left,down)).xyz;
        vec3 d = imageLoad(colors, ivec2(right,down)).xyz;
        vec3 s= ( a+b+c+d ) / 4;
        vec3 ss= ( a*a+b*b+c*c+d*d ) / 4;
      if (shading_test(s,ss)){
           shading(curr_pix);
        }
        else{
  imageStore(outputtexture, curr_pix, vec4(0.0, 1.0, 1.0, 1.0));
        }
  }
  if(phase == 4){

        ivec2 curr_pix = ivec2((block_step *global_id.x)+1,(block_step*global_id.y));

        uint left =curr_pix.x-1;
        uint right = curr_pix.x+1;
        uint down = clamp(curr_pix.y-1,0,row_pixels - 1);
        uint up = clamp(curr_pix.y+1,0,row_pixels - 1);
         vec3 a = imageLoad(colors, ivec2(left,curr_pix.y)).xyz;
        vec3 b = imageLoad(colors, ivec2(right,curr_pix.y)).xyz ;
        vec3 c = imageLoad(colors, ivec2(curr_pix.x,up)).xyz;
        vec3 d = imageLoad(colors, ivec2(curr_pix.x,down)).xyz;
        vec3 s= ( a+b+c+d ) / 4;
        vec3 ss= ( a*a+b*b+c*c+d*d ) / 4;
      if (shading_test(s,ss)){
           shading(curr_pix);
        }
        else{
  imageStore(outputtexture, curr_pix, vec4(0.0, 1.0, 1.0, 1.0));
        }

        //Second part
        curr_pix = ivec2((block_step *global_id.x),(block_step*global_id.y)+1);

       left = clamp(curr_pix.x-1,0,column_pixels - 1);
         right = clamp(curr_pix.x+1,0,column_pixels - 1);
         down = curr_pix.y-1;
         up = curr_pix.y+1;
          a = imageLoad(colors, ivec2(left,curr_pix.y)).xyz;
         b = imageLoad(colors, ivec2(right,curr_pix.y)).xyz ;
         c = imageLoad(colors, ivec2(curr_pix.x,up)).xyz;
         d = imageLoad(colors, ivec2(curr_pix.x,down)).xyz;
            s= ( a+b+c+d ) / 4;
         ss= ( a*a+b*b+c*c+d*d ) / 4;
       if (shading_test(s,ss)){
           shading(curr_pix);
        }
        else{
  imageStore(outputtexture, curr_pix, vec4(0.0, 1.0, 1.0, 1.0));
        }

  }
   

 
}

#endif