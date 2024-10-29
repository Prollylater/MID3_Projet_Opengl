#version 420

#ifdef VERTEX_SHADER

layout(location= 0) in vec3 positions;
layout (location = 1) in vec3 texcoords;
layout (location = 2) in vec3 normals;
layout (location = 3) in vec3 translations;
layout (location = 4) in uint text_uid;


uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 fragment_normal;
out vec2 uv_coordinate;
flat out uint text_id;



void main()
{
    //fragment_normal = normalize((projectionMatrix*viewMatrix*modelMatrix * vec4(normals,0.0))).xyz;
    gl_Position = projectionMatrix*viewMatrix* ( vec4(positions + translations, 1.0));
    uv_coordinate = texcoords.xy;
    text_id = text_uid;
}  
#endif


#ifdef FRAGMENT_SHADER

in vec3 fragment_normal;

in vec2 uv_coordinate;
flat in uint elevations;
uniform sampler2DArray texture_samp_arr; //3d texture with layer being each level of depth (.z)
uniform sampler2D texture_samp;
uniform uint texture_arr_index;
flat in uint text_id;


out vec4 fragment_color;

void main(){
vec2 normalized_uv = uv_coordinate;
vec3 objet_color0= texture(texture_samp,  normalized_uv).rgb ;
vec3 objet_color1= texture(texture_samp_arr, vec3(normalized_uv,text_id)).rgb ;
vec3 objet_color2= texture(texture_samp_arr, vec3(normalized_uv,text_id)).rgb ;

vec3[3] objet_color = {objet_color0,objet_color1,objet_color2 };
 //float cos = dot(normalize(fragment_normal),normalize(vec3(1.0,0.0,0.3)));
 fragment_color = vec4(objet_color0, 1.0) ;
}
#endif



