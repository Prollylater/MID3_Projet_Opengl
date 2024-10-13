
 
#version 330
//Stencil based outline

#ifdef VERTEX_SHADER

uniform mat4 mbMatrix;
uniform mat4 pbMatrix;
uniform mat4 vbMatrix;
uniform float scalingfactor = 2.0;

layout(location= 0) in vec3 position;
//normal data
layout(location = 2) in vec3 normal;


//Get normal position  
out vec3 fragment_normal;
out vec3 fragment_normalcamspace;

 


void main( )
{   
 
    gl_Position= pbMatrix*vbMatrix*mbMatrix * scalingfactor* vec4(position, 1.);

}
#endif




#ifdef FRAGMENT_SHADER


in vec2 UV;

in vec3 fragment_position;
in vec3 fragment_normal;
in vec3 fragment_normalcamspace;
 
out vec4 fragment_color;


void main( )
{
 
 fragment_color = vec4(0.4,0.5,0.4,1.0);

}
#endif

