
#include <cassert>

#include "buffers.h"

template <typename T>
static GLuint create_buffer(const GLenum target, const std::vector<T> &data)
{
    GLuint buffer = 0;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
    return buffer;
}

template <typename T>
static GLuint create_buffer_dynamic(const GLenum target, const std::vector<T> &data)
{
    GLuint buffer = 0;
    glGenBuffers(1, &buffer);
    glBindBuffer(target, buffer);
    glBufferData(target, data.size() * sizeof(T), data.data(), GL_DYNAMIC_DRAW);
    return buffer;
}

GLuint create_buffers(const std::vector<Point> &positions)
{
    assert(positions.size());

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint buffer = create_buffer(GL_ARRAY_BUFFER, positions);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
    glEnableVertexAttribArray(0);

    return vao;
}


GLuint create_buffers(const std::vector<Point> &positions, const std::vector<unsigned> &indices,
                      const std::vector<Point> &texcoords, const std::vector<Vector> &normals)
{
    assert(positions.size());

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint buffer = create_buffer(GL_ARRAY_BUFFER, positions);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
    glEnableVertexAttribArray(0);

    if (texcoords.size() && texcoords.size() == positions.size())
    {
        GLuint buffer = create_buffer(GL_ARRAY_BUFFER, texcoords);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        glEnableVertexAttribArray(1);
    }
    if (normals.size() && normals.size() == positions.size())
    {
        GLuint buffer = create_buffer(GL_ARRAY_BUFFER, normals);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        glEnableVertexAttribArray(2);
    }
    if (indices.size())
    {
        GLuint buffer = create_buffer(GL_ELEMENT_ARRAY_BUFFER, indices);
    }

    return vao;
}

GLuint create_buffers_instancesT(const std::vector<Point> &positions, const std::vector<unsigned> &indices, const std::vector<Point> &texcoords,
                                 const std::vector<Vector> &normals, const std::vector<Transform> &translations)
{
    assert(positions.size());

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint buffer = create_buffer(GL_ARRAY_BUFFER, positions);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
    glEnableVertexAttribArray(0);

    if (texcoords.size() && texcoords.size() == positions.size())
    {
        GLuint buffer = create_buffer(GL_ARRAY_BUFFER, texcoords);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        glEnableVertexAttribArray(1);
    }
    if (normals.size() && normals.size() == positions.size())
    {
        GLuint buffer = create_buffer(GL_ARRAY_BUFFER, normals);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        glEnableVertexAttribArray(2);
    }
    // Instance array
    // As much translations as there is vertices i g
    // Matrice de transaltion homogènes
    if (translations.size())
    {

        std::cout << translations[0] << std::endl;

        // Store  matrix in multiples buffer
        GLuint buffer = create_buffer(GL_ARRAY_BUFFER, translations);
        for (int i = 0; i < 4; i++)
        { // Enable locations 3, 4, 5, and 6 for mat4which should translate to mat 4 loc 3 ?
            glEnableVertexAttribArray(3 + i);
            glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (const void *)((sizeof(Transform) / 4) * i));
            glVertexAttribDivisor(3 + i, 1); // Update per instance
        }
    }

    if (indices.size())
    {
        GLuint buffer = create_buffer(GL_ELEMENT_ARRAY_BUFFER, indices);
    }
    glBindVertexArray(0);

    return vao;
}
GLuint create_buffers_instancesV(GLuint &buffer_translations, const std::vector<Point> &positions, const std::vector<unsigned> &indices,
                                 const std::vector<Point> &texcoords, const std::vector<Vector> &normals, const std::vector<Vector> &translations, const std::vector<unsigned> &texture_id)
{
    assert(positions.size());

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint buffer = create_buffer(GL_ARRAY_BUFFER, positions);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
    glEnableVertexAttribArray(0);

    if (texcoords.size() && texcoords.size() == positions.size())
    {

        GLuint buffer = create_buffer(GL_ARRAY_BUFFER, texcoords);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        glEnableVertexAttribArray(1);
    }
    if (normals.size() && normals.size() == positions.size())
    {
        GLuint buffer = create_buffer(GL_ARRAY_BUFFER, normals);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        glEnableVertexAttribArray(2);
    }
    // Instance array
    // As much translations as there is vertices i g
    if (translations.size())
    {
        buffer_translations = create_buffer_dynamic(GL_ARRAY_BUFFER, translations);
        glEnableVertexAttribArray(3);
        // Notice how we pass a vec4 (4 floats) and specify the offset for each column of the matrix
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        glVertexAttribDivisor(3, 1); // Update per instance
    }
    if (texture_id.size())
    {
        GLuint buffer = create_buffer(GL_ARRAY_BUFFER, texture_id);
        glEnableVertexAttribArray(4);
        //glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        glVertexAttribPointer(4, 1, GL_UNSIGNED_INT, GL_FALSE, 0, (const void *)0);
        glVertexAttribDivisor(4, 1); // Update per instance
   
    }
    if (indices.size())
    {
        GLuint buffer = create_buffer(GL_ELEMENT_ARRAY_BUFFER, indices);
    }
    glBindVertexArray(0);

    return vao;
}
GLuint create_buffers_instancesV(GLuint &buffer_translations, const std::vector<Point> &positions, const std::vector<unsigned> &indices,
                                 const std::vector<Point> &texcoords, const std::vector<Vector> &normals, const std::vector<Vector> &translations)
{
    assert(positions.size());

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint buffer = create_buffer(GL_ARRAY_BUFFER, positions);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
    glEnableVertexAttribArray(0);

    if (texcoords.size() && texcoords.size() == positions.size())
    {

        GLuint buffer = create_buffer(GL_ARRAY_BUFFER, texcoords);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        glEnableVertexAttribArray(1);
    }
    if (normals.size() && normals.size() == positions.size())
    {
        GLuint buffer = create_buffer(GL_ARRAY_BUFFER, normals);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        glEnableVertexAttribArray(2);
    }
    // Instance array
    // As much translations as there is vertices i g
    if (translations.size())
    {
        buffer_translations = create_buffer_dynamic(GL_ARRAY_BUFFER, translations);
        glEnableVertexAttribArray(3);
        // Notice how we pass a vec4 (4 floats) and specify the offset for each column of the matrix
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
        glVertexAttribDivisor(3, 1); // Update per instance
    }
    if (indices.size())
    {
        GLuint buffer = create_buffer(GL_ELEMENT_ARRAY_BUFFER, indices);
    }
    glBindVertexArray(0);

    return vao;
}

template <typename T>
static void update_buffer(const int index, const GLenum target, const std::vector<T> &data)
{
    GLint status = 0;
    glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &status);
    if (status != GL_FALSE)
    {
        GLint buffer = 0;
        glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &buffer);
        if (buffer == 0)
            printf("[error] can't update buffer 0...\n");

        glBindBuffer(target, buffer);
        glBufferData(target, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
    }
}

void update_buffers(const GLuint vao, const std::vector<Point> &positions, const std::vector<unsigned> &indices,
                    const std::vector<Point> &texcoords, const std::vector<Vector> &normals)
{
    assert(vao > 0);
    glBindVertexArray(vao);

    // modifie les vertex buffers attaches au vao
    update_buffer(0, GL_ARRAY_BUFFER, positions);

    if (texcoords.size() && texcoords.size() == positions.size())
        update_buffer(1, GL_ARRAY_BUFFER, texcoords);

    if (normals.size() && normals.size() == positions.size())
        update_buffer(2, GL_ARRAY_BUFFER, normals);

    // et l'index buffer
    if (indices.size())
    {
        GLint buffer = 0;
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &buffer);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_BINDING, buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER_BINDING, indices.size() * sizeof(unsigned), indices.data(), GL_STATIC_DRAW);
    }
}

void release_buffers(const GLuint vao)
{
    assert(vao > 0);
    glBindVertexArray(vao);

    // recupere les vertex buffers attaches au vao
    GLint n = 0;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &n);

    // itere sur les buffers
    for (int i = 0; i < n; i++)
    {
        GLint status = 0;
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &status);
        if (status != GL_FALSE)
        {
            GLuint buffer = 0;
            glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, (GLint *)&buffer);
            glDeleteBuffers(1, &buffer);
        }
    }

    // et l'index buffer
    GLuint buffer = 0;
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, (GLint *)&buffer);
    glDeleteBuffers(1, &buffer);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);
}

void check_buffers(const GLuint vao)
{
    assert(vao > 0);
    glBindVertexArray(vao);

    // recupere les vertex buffers attaches au vao
    GLint n = 0;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &n);

    printf("vao %u: %d attribs\n", vao, n);
    // itere sur les buffers
    for (int i = 0; i < n; i++)
    {
        GLint status = 0;
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &status);

        GLint buffer = 0;
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &buffer);

        printf("  [%d] buffer %d %s\n", i, buffer, status == GL_TRUE ? "on" : "off");
    }

    // et l'index buffer
    GLint buffer = 0;
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &buffer);
    printf("  index buffer %d\n", buffer);
}
