
#ifndef _DRAW_H
#define _DRAW_H

#include "glcore.h"
#include "mat.h"


void draw( const GLuint vao, 
    const GLenum primitives, const unsigned count, 
    const Transform& model, const Transform& view, const Transform& projection );

#endif

