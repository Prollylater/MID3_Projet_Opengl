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
out vec3 fragment_pos;
out vec2 uv_coordinate;



void main()
{
    fragment_normal = normalize(modelMatrix * vec4(normals,0.0)).xyz;
    gl_Position = projectionMatrix*viewMatrix* ( vec4(positions, 1.0));
    fragment_pos = gl_Position.xyz;
    uv_coordinate = texcoords.xy;
}  
#endif


#ifdef FRAGMENT_SHADER

layout(location= 0) out vec3  gcolors;
layout (location = 1) out vec3 gpositions;
layout (location = 2) out vec3 gnormals;

in vec3 fragment_normal;
in vec3 fragment_pos;
in vec2 uv_coordinate;
out vec4 fragment_color;

void main(){
vec2 normalized_uv = uv_coordinate;


vec3 objet_color = vec3(0.2,0.3,0.6);
//float cos = dot(normalize(fragment_normal),normalize(vec3(1.0,0.0,0.3)));
fragment_color = vec4(objet_color, 1.0) ;


gpositions = fragment_pos;
gnormals = fragment_normal;
gcolors = fragment_color.xyz;
}
#endif



