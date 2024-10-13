
#include <cassert>

#include "glcore.h"

#include "mat.h"
#include "program.h"
#include "uniforms.h"
#include "default_program.h"


static unsigned check_buffer( const int index )
{
    GLint buffer= 0;
    GLint status= 0;
    glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &status);
    if(status != GL_FALSE)
        glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &buffer);
    
    return buffer;
}


void draw( const GLuint vao, 
    const GLenum primitives, const unsigned count, 
    const Transform& model, const Transform& view, const Transform& projection )
{
    assert(vao > 0);
    glBindVertexArray(vao);
    
    // un peu de gymnastique, analyse le mesh pour recuperer les attributs des sommets
    unsigned positions= check_buffer(0);
    unsigned texcoords= check_buffer(1);
    unsigned normals= check_buffer(2);
    
    // si les buffers existent, les attributs des sommets aussi, cf create_buffers()...
    GLuint program= create_default_program(primitives, positions ? 1 : 0, texcoords ? 1 : 0, normals ? 1 : 0);
    glUseProgram(program);
    
    // transformations
    Transform mv= view * model;
    Transform normal= view * model;
    Transform mvp= projection * mv;
    program_uniform(program, "mvpMatrix", mvp);
    program_uniform(program, "mvMatrix", mv);
    program_uniform(program, "normalMatrix", normal);
    
    // mesh indexe, ou pas ?
    GLuint index_buffer= 0;
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, (GLint *) &index_buffer);
    
    // draw !!
    if(index_buffer)
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    else
        glDrawArrays(GL_TRIANGLES, 0, count);    
}

