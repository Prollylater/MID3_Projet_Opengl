#version 420

#ifdef VERTEX_SHADER

layout(location= 0) in vec3 positions;
layout (location = 3) in vec4 translations;


uniform mat4 lightSpaceMatrix;



void main()
{
    gl_Position = lightSpaceMatrix*( vec4(positions + translations.xyz, 1.0));
   
}  
#endif


#ifdef FRAGMENT_SHADER
out vec4 fragment_color;

void main(){
}
#endif



