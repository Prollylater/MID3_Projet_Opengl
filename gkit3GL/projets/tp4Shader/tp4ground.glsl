#version 330


#ifdef VERTEX_SHADER


uniform mat4 mvpMatrix;
uniform mat4 lightspaceMatrix;

layout(location= 0) in vec3 position;
//
layout(location = 1) in vec2 texture;
//normal data
layout(location = 2) in vec3 normal;


//Get normal position  
out vec3 fragment_normal;

//Get pixel_pos    
out vec3 fragment_position;
out vec4 fragment_ligthspace;



//Get tex or col out vec2 fragment_texture:
//light component
out vec3 ligth_dir;
out vec3  view_dir;



void main( )
{   
 
    fragment_normal = (normalize( vec4(normal, 0.))).xyz;
    fragment_position = (vec4(position,1)).xyz;
  
      fragment_ligthspace = (lightspaceMatrix *  vec4(position,1));

    // Caméra in worldspace/cameraspace(spec is non static)
    view_dir = -fragment_position;

    // Light direction in worldd space/cameraspace
    ligth_dir = normalize(vec3(1.) - fragment_position);


    gl_Position= mvpMatrix * vec4(position, 1.);

}
#endif


#ifdef FRAGMENT_SHADER



in vec3 fragment_position;
in vec3 fragment_normal;
in vec3 view_dir;
in vec3 ligth_dir;


out vec4 fragment_color;


in vec4 fragment_lightspace;

uniform sampler2DShadow shadowsampler;


float shadowCalculation(vec4 fragment_lightspac)
{
    // SOmething about perspective division
    vec3 proj_coords = fragment_lightspac.xyz /( fragment_lightspac.w ) ;
    
    if(proj_coords.z < 1.0){
         proj_coords = proj_coords * 0.5 + 0.5;



 float closestdepth = texture(shadowsampler, proj_coords);
 float currentdetph = proj_coords.z;
return closestdepth;


    }

}

void main( )
{
    // Lights colour
    vec3 lighta = vec3(0.2);
    vec3 lightd = vec3(0.5,0.0,0.3);
    vec3 lights = vec3(1.0);

    
        

    vec3 objectColor = vec3(0.2,0.4,0.7); //Color


    vec3 diff = objectColor * (clamp(dot(ligth_dir,fragment_normal), 0., 1.)) ;  
    float shadow = (1.0-shadowCalculation(fragment_lightspace));

 
    //Ambiant Light
    vec3 amb = objectColor* lighta;


    //Blinn Phong Specular Light
    vec3 halfwaydir = normalize(view_dir+ligth_dir );
    vec3 spec = objectColor * pow(clamp(dot(halfwaydir, fragment_normal), 0., 1.), 8.0) * lights ;
    fragment_color = shadow*vec4(amb+(diff+spec), 1.0);


}
#endif

