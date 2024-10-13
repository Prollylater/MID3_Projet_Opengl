
#version 330


#ifdef VERTEX_SHADER


uniform mat4 mMatrix;
uniform mat4 vMatrix;
uniform mat4 mvpMatrix;

layout(location= 0) in vec3 position;
//
layout(location = 1) in vec2 texture;
//normal data
layout(location = 2) in vec3 normal;


//Get normal position  
out vec3 fragment_normal;
//out vec3 fragment_normalcamspace;

//Get pixel_pos    
out vec3 fragment_position;
//Get tex or col out vec2 fragment_texture:
//light component
out vec3 ligth_dir;
//out vec3 ligth_dircamspace;

out vec3  view_dir;
//out vec3  view_dircamspace;

out vec2 UV;



void main( )
{   
 
    //fragment_normalcamspace = (normalize(vMatrix * mMatrix *  vec4(normal,0.))).xyz;
    fragment_normal = (normalize(mMatrix *  vec4(normal, 0.))).xyz;


    //Position in worldspace
    fragment_position = (mMatrix *  vec4(position,1)).xyz;

    // Caméra in worldspace/cameraspace(spec is non static)
    view_dir = -fragment_position; //Decent result albeit incorrect algorithm, spec stay static
    //vec3 fragment_positioncamspace =  (normalize(vMatrix * mMatrix *  vec4(position,1.0))).xyz;
    //view_dircamspace = -fragment_positioncamspace;

    // Light direction in worldd space/cameraspace
    ligth_dir = normalize(vec3(.2,.3,1.) - fragment_position);
    //ligth_dircamspace = normalize((normalize(vMatrix* vec4(1.))).xyz - fragment_positioncamspace);

    UV = texture;

    gl_Position= mvpMatrix * vec4(position, 1.);

}
#endif




#ifdef FRAGMENT_SHADER


in vec2 UV;

in vec3 fragment_position;
in vec3 fragment_normal;
// in vec3 fragment_normalcamspace;
in vec3 view_dir;
//in vec3 view_dircamspace;
in vec3 ligth_dir;
//in vec3 ligth_dircamspace;


out vec4 fragment_color;

uniform sampler2D myTextureSampler;

void main( )
{
 
    vec3 lighta = vec3(0.2);
    vec3 lightd = vec3(1.0);
    vec3 lights = vec3(1.0);


    vec3 objectColor = texture( myTextureSampler, UV).rgb;
 
    //Goochthing
    vec3 warm = vec3(0.0,0.0,0.5);
    vec3 cool = vec3(0.5,0.0,0.3);
    vec3 gooch = objectColor * dot(ligth_dir,fragment_normal) ; 
    
    //Try n, -scenelight.dl sometime
    //vec3 diff = objectColor * (clamp(dot(ligth_dir,fragment_normal), 0., 1.)) ;  

    vec3 diff = gooch * warm + (1-gooch)*cool; 
    //Ambiant Light
    vec3 amb = objectColor* lighta;


    //Blinn Phong Specular Light
    vec3 halfwaydir = normalize(view_dir+ligth_dir );
    vec3 spec = objectColor * pow(clamp(dot(halfwaydir, fragment_normal), 0., 1.), 8.0) * lights;
    fragment_color = vec4(amb+diff+spec, 1.0);
    //fragment_color = vec4(texture( myTextureSampler, UV).rgb, 1.0);

}
#endif
