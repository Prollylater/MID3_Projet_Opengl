#version 330


#ifdef VERTEX_SHADER

layout(location= 0) in vec3 positions;
layout (location = 1) in vec3 texcoords;
layout (location = 2) in vec3 normals;

    
uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 fragment_normal;

void main()
{
    //fragment_normal = normalize((projectionMatrix*viewMatrix*modelMatrix *  vec4(normals, 0.))).xyz;
    fragment_normal = normals;
    gl_Position =  projectionMatrix*viewMatrix* (modelMatrix )* vec4(positions,1.0);
}  
#endif


#ifdef FRAGMENT_SHADER

in vec3 fragment_normal;

out vec4 fragment_color;
void main(){
 float cos = dot(normalize(fragment_normal),normalize(vec3(0.4,0.0,0.3)));
 fragment_color = vec4(0.8,0.12,0.42,1.0) ;
 //fragment_color = vec4(abs(fragment_normal), 1.0);
}
#endif



