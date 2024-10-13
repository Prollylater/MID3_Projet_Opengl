#include <cstdio>

#include <algorithm>

#include "texture.h"

#include "stb_image.h"

int miplevels(const int width,
              const int height)
{
    int w = width;
    int h = height;
    int levels = 1;
    while (w > 1 || h > 1)
    {
        w = std::max(1, w / 2);
        h = std::max(1, h / 2);
        levels = levels + 1;
    }

    return levels;
}

GLuint make_texture(const int unit,
                    const int width,
                    const int height,
                    const GLenum texel_type,
                    const GLenum data_format,
                    const GLenum data_type)
{
    // cree la texture openGL
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);

    // fixe les parametres de filtrage par defaut
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // transfere les donnees dans la texture, 4 float par texel
    glTexImage2D(GL_TEXTURE_2D, 0,
                 texel_type, width, height, 0,
                 data_format, data_type, nullptr);

    // prefiltre la texture / alloue les mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);
    return texture;
}

GLuint make_flat_texture(const int unit,
                         const int width,
                         const int height,
                         const GLenum texel_type,
                         const GLenum data_format,
                         const GLenum data_type)
{
    // cree la texture openGL
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);

    // fixe les parametres de filtrage par defaut
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 1 seul mipmap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    // transfere les donnees dans la texture, 4 float par texel
    glTexImage2D(GL_TEXTURE_2D, 0,
                 texel_type, width, height, 0,
                 data_format, data_type, nullptr);

    // prefiltre la texture / alloue les mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);
    return texture;
}

GLuint make_texture(const int unit,
                    const Image &im,
                    const GLenum texel_type)
{
    if (im.size() == 0)
        return 0;

    // cree la texture openGL
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);

    // fixe les parametres de filtrage par defaut
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // transfere les donnees dans la texture, 4 float par texel
    glTexImage2D(GL_TEXTURE_2D, 0,
                 texel_type, im.width(), im.height(), 0,
                 GL_RGBA, GL_FLOAT, im.data());

    // prefiltre la texture
    glGenerateMipmap(GL_TEXTURE_2D);
    return texture;
}

GLuint read_texture(const int unit,
                    const char *filename,
                    const GLenum texel_type)
{
    stbi_set_flip_vertically_on_load(1);

    int width, height, channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 0);
    if (!data)
    {
        printf("[error] loading '%s'...\n", filename);
        return 0;
    }

    // cree la texture openGL
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);

    // fixe les parametres de filtrage par defaut
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLenum format;
    GLenum type = GL_UNSIGNED_BYTE;
    switch (channels)
    {
    case 1:
        format = GL_RED;
        break;
    case 2:
        format = GL_RG;
        break;
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        break;
    default:
        format = GL_RGBA;
    }

    // transfere les donnees dans la texture
    glTexImage2D(GL_TEXTURE_2D, 0,
                 texel_type, width, height, 0,
                 format, type, data);

    // prefiltre la texture
    glGenerateMipmap(GL_TEXTURE_2D);
    // nettoyage
    stbi_image_free(data);
    return texture;
}
GLuint read_texture_array(const int unit,
                          const std::vector<const char *>& filenames,
                          const GLenum texel_type)
{
    stbi_set_flip_vertically_on_load(1);
    GLuint texture = 0;
    int width, height, channels;
    // Only exist to prefill the data
    unsigned char *temp = stbi_load(filenames[0], &width, &height, &channels, 0);
    GLenum format;
    GLenum type = GL_UNSIGNED_BYTE;

    switch (channels)
    {
    case 1:
        format = GL_RED;
        break;
    case 2:
        format = GL_RG;
        break;
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        break;
    default:
        format = GL_RGBA;
    }

    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, texel_type, width, height, filenames.size(), 0, format, type, 0);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 1);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, temp);
    stbi_image_free(temp);
    for (int i = 1; i < filenames.size(); i++)
    {
        // transfere les donnees dans la texture
        unsigned char *data = (stbi_load(filenames[i], &width, &height, &channels, 0));
        if (!data)
        {
            printf("[error] loading '%s'...\n", filenames[i]);
            return 0;
        }
        else{
            printf("Loaded '%s'...\n", filenames[i]);

        }
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

        //glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        stbi_image_free(data);
    }

    return texture;
}

// creation des textures
GLuint make_depth_texture(const int unit,
                          const int width,
                          const int height,
                          const GLenum texel_type)
{
    return make_flat_texture(unit, width, height, texel_type, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
}

GLuint make_uint_texture(const int unit,
                         const int width,
                         const int height,
                         const GLenum texel_type)
{
    return make_flat_texture(unit, width, height, texel_type, GL_RED_INTEGER, GL_UNSIGNED_INT);
}

GLuint make_float_texture(const int unit,
                          const int width,
                          const int height,
                          const GLenum texel_type)
{
    return make_flat_texture(unit, width, height, texel_type, GL_RED, GL_FLOAT);
}

GLuint make_vec2_texture(const int unit,
                         const int width,
                         const int height,
                         const GLenum texel_type)
{
    return make_flat_texture(unit, width, height, texel_type, GL_RG, GL_FLOAT);
}

GLuint make_vec3_texture(const int unit,
                         const int width,
                         const int height,
                         const GLenum texel_type)
{
    return make_flat_texture(unit, width, height, texel_type, GL_RGB, GL_FLOAT);
}

GLuint make_vec4_texture(const int unit,
                         const int width,
                         const int height,
                         const GLenum texel_type)
{
    return make_flat_texture(unit, width, height, texel_type, GL_RGBA, GL_FLOAT);
}

GLuint make_skybox()
{
    std::vector<
        const char *>
        box;

    box.push_back("./data/right.jpg");
    box.push_back("./data/left.jpg");
    box.push_back("./data/top.jpg");
    box.push_back("./data/bottom.jpg");
    box.push_back("./data/front.jpg");
    box.push_back("./data/back.jpg");

    GLuint texture = 0;

    // cree la texture openGL
    glGenTextures(1, &texture);
    // glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    int width, height, nrChannels;

    // Idk what it do but it must be fine, the wiki said so
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Define all 6 faces

    for (unsigned int i = 0; i < box.size(); i++)
    {
        unsigned char *data = stbi_load(box[i], &width, &height, &nrChannels, 0);
        if (data)
        {
            GLenum internalFormat = GL_COMPRESSED_RGB, dataFormat = GL_RGB;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
            // Generate Texture Mipmaps
            //  glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);
        }
        else
        {
            stbi_image_free(data);
        }
    }

    return texture;
}

//
//~ int screenshot( const char *filename )
//~ {
//~ // recupere le contenu de la fenetre / framebuffer par defaut
//~ glFinish();

//~ glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
//~ glReadBuffer(GL_BACK);

//~ // recupere les dimensions de la fenetre
//~ GLint viewport[4];
//~ glGetIntegerv(GL_VIEWPORT, viewport);

//~ // transfere les pixels
//~ ImageData image(viewport[2], viewport[3], 4);
//~ glReadPixels(0, 0, image.width, image.height,
//~ GL_RGBA, GL_UNSIGNED_BYTE, image.data());
//~ // ecrit l'image
//~ return write_image_data(image, filename);
//~ }

//~ int screenshot( const char *prefix, const int id )
//~ {
//~ char tmp[4096];
//~ sprintf(tmp,"%s%02d.png", prefix, id);
//~ return screenshot(tmp);
//~ }

//~ int capture( const char *prefix )
//~ {
//~ static int id= 1;

//~ char tmp[4096];
//~ sprintf(tmp,"%s%04d.bmp", prefix, id);

//~ if(id % 30 == 0)
//~ printf("capture frame '%s'...\n", tmp);

//~ id++;
//~ return screenshot(tmp);
//~ }