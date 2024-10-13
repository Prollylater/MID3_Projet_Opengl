#version 330

#ifdef VERTEX_SHADER



uniform mat4 mvpMatrix;

layout (location = 0) in vec3 position;

out vec3 TexCoords;
 

void main( )
{   
 
    TexCoords = position;
    gl_Position = mvpMatrix*  vec4(position, 1.0);

}
#endif




#ifdef FRAGMENT_SHADER

out vec4 fragment_color;
in vec3 TexCoords;
uniform samplerCube skybox;


void main( )
{
      fragment_color = texture(skybox, TexCoords);
	//fragment_color = vec4(0.3,0.2,0.5,1.0);
}
#endif

