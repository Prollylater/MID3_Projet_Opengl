
 
#version 330


#ifdef VERTEX_SHADER


uniform mat4 mMatrix;
uniform mat4 vMatrix;
uniform mat4 mvpMatrix;
uniform mat4 lightspaceMatrix;

uniform vec3 lightInvDir;


layout(location= 0) in vec3 position;
//
layout(location = 1) in vec2 texture;
//normal data
layout(location = 2) in vec3 normal;


//Get normal position  
out vec3 fragment_normal;
out vec3 fragment_normalcamspace;

//Get pixel_pos    
out vec3 fragment_position;
out vec4 fragment_ligthspace;

//Get tex or col out vec2 fragment_texture:
//light component
out vec3 ligth_dir;
out vec3 ligth_dircamspace;

//out vec3  view_dir;
out vec3  view_dircamspace;

out vec2 UV;



void main( )
{   
 
    fragment_normalcamspace = (normalize(vMatrix * mMatrix *  vec4(normal,0.))).xyz;
    fragment_normal = (normalize(mMatrix *  vec4(normal, 0.))).xyz;


    //Position in worldspace
    fragment_position = (mMatrix *  vec4(position,1)).xyz;
    //Position in worldspace
    fragment_ligthspace = (lightspaceMatrix *  vec4(position,1));


    // Caméra in worldspace/cameraspace(spec is non static)
    vec3 fragment_positioncamspace =  (normalize(vMatrix * mMatrix *  vec4(position,1.0))).xyz;
    view_dircamspace = -fragment_positioncamspace;

    // Light direction in worldd space/cameraspace
    ligth_dir = normalize(vec3(1.) - fragment_position);
    ligth_dircamspace = normalize((normalize(vMatrix* vec4(lightInvDir,1.0))).xyz - fragment_positioncamspace);

    UV = texture;

    gl_Position= mvpMatrix * vec4(position, 1.);

}
#endif




#ifdef FRAGMENT_SHADER


in vec2 UV;

in vec3 fragment_position;
in vec3 fragment_normal;
in vec3 fragment_normalcamspace;
//in vec3 view_dir;
in vec3 view_dircamspace;
in vec3 ligth_dir;
in vec3 ligth_dircamspace;
in vec4 fragment_lightspace;


out vec4 fragment_color;

uniform sampler2D hisTextureSampler;
uniform sampler2D shadowsampler;


float shadowCalculation(vec4 fragment_lightspace)
{
    // SOmething about perspective division
    vec3 proj_coords = fragment_lightspace.xyz / fragment_lightspace.w;
    proj_coords = proj_coords * 0.5 + 0.5;

    float depth = texture(shadowsampler, proj_coords.xy).z;

     // If the fragment is in shadow, darken its color
    // You may want to adjust the bias and comparison method for better results
    float bias = 0.005; // Bias to avoid self-shadowing artifacts
    float visibility = proj_coords.z - bias > depth ? 0.0 : 1.0; // Adjust this comparison as needed
    // Sample the diffuse texture
   return visibility;


return depth;
    float z = proj_coords.z; 
    // Check whether current frag pos is in shadow
    float shadow = z > depth  ? 1.0 : 0.0;
    return shadow;
}


void main( )
{
    // Lights colour
    vec3 lighta = vec3(0.2);
    vec3 lightd = vec3(0.5,0.0,0.3);
    vec3 lights = vec3(1.0);


    // Fresnel edgje detector
    float fresnel = clamp(dot(fragment_normalcamspace, view_dircamspace),0.,1.);

    // 2nd version, less pronounced edge
    //vec3 fresnel2 = vec3( 0.17+ (1.0 - 0.17) * pow(1.0 - fresnel, 5.0));
    
    //Make edge white if uncommented
    //fresnel = 1.0 -fresnel;
    // Apply threshold
    float fresnedgeFactor = smoothstep(0.1, 1.0, fresnel);

    // Thresholding for Cell Shading like thing

    float quant = dot(fragment_normalcamspace, ligth_dircamspace);
    if (quant > 0.9)
        quant = 1.0;
    else if (quant > 0.7)
        quant = 0.8;
    else if (quant > 0.4)
        quant = 0.7;
    else if (quant > 0.3)
        quant = 0.3;
    else
        quant = 0.1;
    // End    
        

    //vec3 objectColor = vec3(0.2,0.4,0.7); //Color
    vec3 objectColor = texture( hisTextureSampler, UV).rgb ; //Texture

    //objectColor = objectColor * quant; // Applying thresholding
    //objectColor = mix(objectColor, vec3(0.0), fresnel2); //fresnel edge implem2
    //objectColor = mix(objectColor, vec3(0.0), fresnedgeFactor); //fresnel edge implem1
    

    //Gooch colour shading parameter
    vec3 warm = vec3(0.7,0.0,0.7);
    vec3 cool = vec3(0.3,0.3,0.9); 

    //World space for camera and light to be able to clearly see the effect from camera POV
    vec3 gooch = objectColor * dot(ligth_dir,fragment_normal) ; 


    vec3 diff = objectColor * (clamp(dot(ligth_dircamspace,fragment_normalcamspace), 0., 1.)) ;  
    //vec3 diff = gooch * warm + (1-gooch)*cool; 
    diff = diff * shadowCalculation(fragment_lightspace);

 
    //Ambiant Light
    vec3 amb = objectColor* lighta;


    //Blinn Phong Specular Light
    vec3 halfwaydir = normalize(view_dircamspace+ligth_dircamspace );
    vec3 spec = objectColor * pow(clamp(dot(halfwaydir, fragment_normalcamspace), 0., 1.), 8.0) * lights;
    
       fragment_color = vec4(amb+diff+spec, 1.0);

     
     
     //Texture without ligth, can multiply with quant, fresnel2 and edge Factor
fragment_color =  (fragment_color  * vec4(texture(shadowsampler, UV).rgb, 1.0)) ;

}
#endif

