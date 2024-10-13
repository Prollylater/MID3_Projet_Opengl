
#version 330

#ifdef VERTEX_SHADER
layout(location= 0) in vec3 position;
layout(location= 1) in vec3 texcoord;
layout(location= 2) in vec3 normal;
layout(location= 4) in uint material;


#ifdef USE_COLOR
    layout(location= 3) in vec4 color;
    out vec4 vertex_color;
#endif

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat4 modelMatrix;
out vec3 fragment_normal;

flat out uint vertex_material;
void main( )
{	
    fragment_normal = (normalize(modelMatrix *  vec4(normal, 0.))).xyz;
    gl_Position= mvpMatrix * vec4(position, 1);
    vertex_material = material;
#ifdef USE_COLOR
    vertex_color= color;
#endif
}
#endif




#ifdef FRAGMENT_SHADER //HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH

#ifdef USE_COLOR
    in vec4 vertex_color;
#endif
flat in uint vertex_material;
in vec3 fragment_normal;


uniform vec4 mesh_color = vec4(0.32,0.3,0.2,1);


uniform vec3 lightdir  = vec3(0.5,1,1);


out vec4 fragment_color;
void main( )
{
    vec4 col_table[10]; 
 
    col_table[0]  = vec4(0.9, 0.6, 0.5, 1.0);  
    col_table[1]  = vec4(0.4, 0.2, 0.6, 1.0); 
    col_table[2]  = vec4(0.5, 0.7, 0.2, 1.0);  
    col_table[3]  = vec4(0.3, 0.5, 0.9, 1.0);  
    col_table[4]  = vec4(0.8, 0.3, 0.5, 1.0);  
    col_table[5]  = vec4(0.6, 0.8, 0.6, 1.0);  
    col_table[6]  = vec4(0.9, 0.5, 0.3, 1.0);  
    col_table[7]  = vec4(0.7, 0.1, 0.4, 1.0);  


    vec4 color= mesh_color;
    uint mat_color= vertex_material+uint(4);
#ifdef USE_COLOR
    color= vertex_color;
#endif
    
    float seuillage = dot(normalize(fragment_normal),normalize(lightdir));
    float passeuillage = dot(normalize(fragment_normal),normalize(lightdir));
    if (seuillage > 0.9)
        seuillage = 1.0;
    else if (seuillage > 0.7)
        seuillage = 0.8;
    else if (seuillage > 0.4)
        seuillage = 0.7;
    else if (seuillage > 0.3)
        seuillage = 0.3;
    else
        seuillage = 0.1;
    // End    
        
    fragment_color= col_table[mat_color] *passeuillage;
}
#endif
