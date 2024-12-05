
#ifndef _BUFFERS_H
#define _BUFFERS_H

#include <vector>

#include "glcore.h"
#include "vec.h"
#include "mat.h"

GLuint create_buffers(const std::vector<Point> &positions, const std::vector<unsigned> &indices = {},
                      const std::vector<Point> &texcoords = {}, const std::vector<Vector> &normals = {});

GLuint create_buffers_instancesV( GLuint& buffer_translations ,const std::vector<Point> &positions, const std::vector<unsigned> &indices = {},
                                 const std::vector<Point> &texcoords = {}, const std::vector<Vector> &normals = {}, const std::vector<Vector> &translations = {});

GLuint create_buffers_instancesV(GLuint &buffer_translations, const std::vector<Point> &positions, const std::vector<unsigned> &indices,
                                 const std::vector<Point> &texcoords, const std::vector<Vector> &normals, const std::vector<vec4> &translations);



GLuint create_buffers_instancesT(const std::vector<Point> &positions, const std::vector<unsigned> &indices = {},
                                 const std::vector<Point> &texcoords = {}, const std::vector<Vector> &normals = {}, const std::vector<Transform> &translations = {});

void update_buffers(const GLuint vao,
                    const std::vector<Point> &positions, const std::vector<unsigned> &indices = {},
                    const std::vector<Point> &texcoords = {}, const std::vector<Vector> &normals = {});

void release_buffers(const GLuint vao);

void check_buffers(const GLuint vao);


#endif
