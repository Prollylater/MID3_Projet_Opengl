
#version 330


#ifdef VERTEX_SHADER


uniform mat4 mMatrix;
uniform mat4 vMatrix;
uniform mat4 mvpMatrix;

layout(location= 0) in vec3 position;
//layout(location = 1) in vec3 texture;
//normal data
layout(location = 2) in vec3 normal;
//Get normal position  
out vec3 fragment_normal;
//Get pixel_pos    
out vec3 fragment_position;
//Get tex or col out vec3 fragment_texture:

//light component
out vec3 ligthdir;
out vec3  viewdir;

void main( )
{   
    //Normal in worldspace
    fragment_normal = (normalize(mMatrix *  vec4(position, 1))).xyz;
    //Position in worldspace
    fragment_position = (mMatrix *  vec4(normal,0)).xyz;
    // Caméra in worldspace
    viewdir = normalize((vMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz - fragment_position);
    // Light direction in wod space
    ligthdir = normalize((0.4,0.5,0.0) - fragment_position);
    //Could calculate the mvp here
    gl_Position= mvpMatrix * vec4(position, 1);

}
#endif




#ifdef FRAGMENT_SHADER

uniform vec3 objectColor= vec3(0.6,0.3,0.2);

in vec3 fragment_normal;
in vec3 fragment_position;
in vec3 view_dir;
in vec3 ligth_dir;

out vec4 fragment_color;


void main( )
{
 
    vec3 lighta = vec3(0.2);
    vec3 lightd = vec3(0.4);
    vec3 lights = vec3(0.7);



    //Try n, -scenelight.dl sometime
    vec3 diff = objectColor * (clamp(dot(fragment_normal,ligth_dir), 0., 1.)) ;  

    //Ambiant Light
    vec3 amb = objectColor* lighta;


    //Blinn Phong Specular Light
    vec3 halfwaydir = normalize(ligth_dir + view_dir);
    vec3 spec = objectColor * pow(clamp(dot(halfwaydir, fragment_normal), 0., 1.), 30.0);
    fragment_color = vec4(amb + diff + spec, 1.0);
}
#endif
