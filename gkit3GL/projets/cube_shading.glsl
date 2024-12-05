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

out vec3 fragment_normal;
out vec2 uv_coordinate;
flat out uint text_id;



void main()
{
    //fragment_normal = normalize((projectionMatrix*viewMatrix*modelMatrix * vec4(normals,0.0))).xyz;
    gl_Position = projectionMatrix*viewMatrix* ( vec4(positions + translations.xyz, 1.0));
    uv_coordinate = texcoords.xy;
    text_id = uint(translations.w);
}  
#endif


#ifdef FRAGMENT_SHADER

in vec3 fragment_normal;

in vec2 uv_coordinate;
uniform sampler2DArray texture_samp_arr; //3d texture with layer being each level of depth (.z)
uniform sampler2D texture_samp;
flat in uint text_id;


out vec4 fragment_color;

void main(){
vec2 normalized_uv = uv_coordinate;
uint id =text_id;
vec3 objet_color= texture(texture_samp_arr, vec3(normalized_uv,text_id)).rgb ;

 //float cos = dot(normalize(fragment_normal),normalize(vec3(1.0,0.0,0.3)));
 fragment_color = vec4(objet_color, 1.0) ;
}
#endif



