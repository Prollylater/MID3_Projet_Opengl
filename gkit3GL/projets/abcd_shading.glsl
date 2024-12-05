#version 420

#ifdef VERTEX_SHADER

layout(location= 0) in vec3 positions;
layout (location = 1) in vec3 texcoords;
layout (location = 2) in vec3 normals;



uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 fragment_normal;
out vec2 uv_coordinate;



void main()
{
    //fragment_normal = normalize((projectionMatrix*viewMatrix*modelMatrix * vec4(normals,0.0))).xyz;
    fragment_normal = normalize(normals);
    gl_Position = projectionMatrix*viewMatrix* ( vec4(positions, 1.0));
    uv_coordinate = texcoords.xy;
}  
#endif


#ifdef FRAGMENT_SHADER

in vec3 fragment_normal;
in vec2 uv_coordinate;
out vec4 fragment_color;
uniform sampler2D texa;
uniform sampler2D texb;
uniform sampler2D texc;
uniform sampler2D texd;

void main(){
vec2 normalized_uv = uv_coordinate;
/*
vec3 a= texture(texa, normalized_uv).rgb ;
vec3 b= texture(texb, normalized_uv).rgb ;
vec3 c= texture(texc, normalized_uv).rgb ;
vec3 d= texture(texd, normalized_uv).rgb ;
vec3 objet_color = vec3(0.2,0.3,0.6) + a +b+c+d;*/
vec3 objet_color = vec3(0.2,0.3,0.6);

float cos = dot(normalize(fragment_normal),normalize(vec3(1.0,0.0,0.3)));
 fragment_color = vec4(objet_color*cos, 1.0) ;
}
#endif



