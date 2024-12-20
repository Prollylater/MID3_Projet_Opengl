#version 420

#ifdef VERTEX_SHADER

layout(location= 0) in vec3 positions;
layout (location = 1) in vec3 texcoords;
layout (location = 2) in vec3 normals;
layout (location = 3) in vec4 translations;


uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 lightSpaceMatrix;

out vec3 fragment_normal;
out vec4 fragment_ligth_pos;
out vec2 uv_coordinate;
flat out uint text_id;



void main()
{
    fragment_normal = normalize((modelMatrix * vec4(normals,0.0))).xyz;
    fragment_ligth_pos = lightSpaceMatrix*( vec4(positions + translations.xyz, 1.0));

    gl_Position = projectionMatrix*viewMatrix* ( vec4(positions + translations.xyz, 1.0));
    //Test des positions du point de vue de la lumière
    //gl_Position = lightSpaceMatrix*viewMatrix* ( vec4(positions + translations.xyz, 1.0));

    uv_coordinate = texcoords.xy;
    text_id = uint(translations.w);
}  
#endif


#ifdef FRAGMENT_SHADER

in vec4 fragment_ligth_pos;
in vec3 fragment_normal;
in vec2 uv_coordinate;
flat in uint text_id;

uniform sampler2DArray texture_samp_arr; //3d texture with layer being each level of depth (.z)
uniform sampler2D shadow_texture;

uniform vec3 lightdir;


float shadowCalculation(vec4 fragment_pos) {

    vec3 proj_coords = fragment_pos.xyz / fragment_pos.w;
if (proj_coords.x < 0.0 || proj_coords.y < 0.0 || proj_coords.x > 1.0 || proj_coords.y > 1.0 || proj_coords.z > 1.0) {
        return 1.0; 
    }
   float bias = 0.00001;  

        float shadow_map_depth = texture(shadow_texture, proj_coords.xy).r;
    float shadow = proj_coords.z - bias > shadow_map_depth ? 0.0 : 1.0;
    return shadow;
}
 


out vec4 fragment_color;

void main(){
     // Lights colour
    vec3 lighta = vec3(0.2);
    vec3 lightd = vec3(0.5,0.0,0.3);
    vec3 lights = vec3(1.0);
//Texture
vec2 normalized_uv = uv_coordinate;
uint id =text_id;
vec3 objet_color= texture(texture_samp_arr, vec3(normalized_uv,text_id)).rgb ;

float shadow = shadowCalculation(fragment_ligth_pos);       


 vec3 diff = objet_color * (clamp(dot(normalize(lightdir),normalize(fragment_normal)), 0., 1.)) ;  
vec3 final_color = lighta*objet_color + diff*shadow;
fragment_color = vec4(final_color , 1.0);
}
#endif





   


   


