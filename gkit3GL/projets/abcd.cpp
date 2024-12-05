
#include "window.h"
#include "glcore.h"
#include "mesh_io.h"
#include "buffers.h"
#include "program.h"
#include "uniforms.h"
#include "texture.h"
#include <algorithm>
#include "orbiter.h"

Orbiter camera;

// Store all the charged program
std::vector<GLuint> m_programs;
// Store all the charged program
std::vector<GLuint> m_vao;
// Store all the charged object annd the associated transform matrices
std::vector<MeshIOData> m_objet;
// Store all the textures in the program
std::vector<GLuint> m_texture;

std::vector<GLuint> m_objet_translate_buffer;
GLuint m_objet_text_id_buffer;

GLuint read_framebuffer;
constexpr int global_width = 1024;
constexpr int global_height = 576;

// Structure handling the ressource, supportuing the occlusion culling
struct GBufferstruct
{
    // creation des textures pour stocker le GBuffer
    GLuint fbo;
    GLuint zbuffer;
    GLuint position;
    GLuint normal;
    GLuint color;
    GLuint output;

    GLenum buffers_enum[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                              GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};

    GBufferstruct() = default;

    bool initOccCuller()
    {
        // Textures
        zbuffer = make_depth_texture(0, global_width, global_height);
        position = make_vec4_texture(0, global_width, global_height);
        normal = make_vec4_texture(0, global_width, global_height);
        color = make_vec4_texture(0, global_width, global_height);
        output = make_vec4_texture(0, global_width, global_height);

        // Bufffers

        // Create framebuffer
        fbo = 0;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

        glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, zbuffer, 0);
        glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color, 0);
        glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, position, 0);
        glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, normal, 0);
        glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, output, 0);

        glDrawBuffers(4, buffers_enum);

        if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "Errror during FBO for Occlusion culling init" << std::endl;
            return false;
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        return true;
    }
};

bool bindcomputeTexture(GLuint program, GLuint position, GLuint color, GLuint zbuffer, GLuint normal, GLuint output)
{
    // Positions
    glBindImageTexture(0, position, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
    // GLint location = glGetUniformLocation(program, "positions");
    // glUniform1i(location, 0);
    // Color
    glBindImageTexture(1, color, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
    // GLint location = glGetUniformLocation(program, "colors");
    // glUniform1i(location, 1);

    // Zbuffer
    glBindImageTexture(2, zbuffer, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32UI);
    // GLint location = glGetUniformLocation(program, "zbuffer");
    // glUniform1i(location, 2);

    // normal
    glBindImageTexture(3, normal, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
    // GLint location = glGetUniformLocation(program, "normals");
    // glUniform1i(location, 3);

    // output texture
    glBindImageTexture(4, output, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    // GLint location = glGetUniformLocation(program, "output_texture");
    // glUniform1i(location, 4);
    return true;
}

GBufferstruct culler;

bool init()
{

    camera.projection(global_width, global_height, 50);

    MeshIOData tmp;
    if (!read_meshio_data("./data/hearthspring.obj", tmp))
        return false;
    m_objet.push_back(tmp);
    // LOADING TEXTURES ARRAY
    m_texture.push_back(read_texture(0, "./data/cube_world/Atlas.png"));

    GLuint tmp_vao = create_buffers(tmp.positions, tmp.indices, tmp.texcoords, tmp.normals);

    m_vao.push_back(tmp_vao);

    // charge et compile les shaders
    tmp_vao = read_program("projets/abcd_shading.glsl");
    m_programs.push_back(tmp_vao);
    tmp_vao = read_program("projets/abcd_shadingdef.glsl");
    m_programs.push_back(tmp_vao);
    tmp_vao = read_program("projets/abcd_compute.glsl");
    m_programs.push_back(tmp_vao);
    //  tmp_vao = read_program("projets/abcd_dshading.glsl");
    //  m_programs.push_back(tmp_vao);

    culler.initOccCuller();
    ///

    glGenFramebuffers(1, &read_framebuffer);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, read_framebuffer);
    glFramebufferTexture(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, culler.output,  0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    ///

    std::cout << "Sort of here" << std::endl;
    // affiche les erreurs de maniere lisible.
    for (unsigned &progam : m_programs)
    {
        program_print_errors(progam);
    }
    std::cout << "Sort of here" << std::endl;

    /*
    // affiche les erreurs de maniere lisible.
    program_print_errors(skyboxprog);
    */
    for (const unsigned &program : m_programs)
    {
        if (!program_ready(program))
        {
            return false;
        }
        // erreur de compilation / link / chargement des shaders
    }
    std::cout << "Initialization finished !" << std::endl;

    return true;
};
// TODO: Rare CRASH tracing back to an assert at mat.cpp 86

// Global  for tracking mouse states and movement
float dist_an_x = 0, dist_an_y = 0, dist_an_z = 0;
float pitch, yaw = 0.0f;
// Keyboard control TODO:Decomission after 3rd person mvm
bool moved = true;
void input()
{
    // Classic variable
    static float cam_speed = 0.05f;

    static int i;
    if (key_state('1'))
    {
        cam_speed += 0.02f;
    }
    if (key_state('2'))
    {
        cam_speed -= 0.02f;
        cam_speed = cam_speed > 0 ? cam_speed : 0.02f;
    }

    if (key_state('e'))
    {
        pitch += 0.7f;
        moved = true;
    }
    if (key_state('a'))
    {
        pitch -= 0.7f;
        moved = true;
    }
    if (key_state('w'))
    {
        yaw -= 0.7f;
        moved = true;
    }
    if (key_state('x'))
    {
        yaw += 0.7f;
        moved = true;
    }
    if (key_state('q'))
    {
        dist_an_x -= cam_speed;
        moved = true;
    }
    if (key_state('d'))
    {
        dist_an_x += cam_speed;
        moved = true;
    }
    if (key_state('r'))
    {
        dist_an_y -= cam_speed;
        moved = true;
    }
    if (key_state('f'))
    {
        dist_an_y += cam_speed;
        moved = true;
    }
    if (key_state('z'))
    {
        dist_an_z -= cam_speed;
        moved = true;
    }
    if (key_state('s'))
    {
        dist_an_z += cam_speed;
        moved = true;
    }

    return;
}

void draw()
{

    // input();
    camera.prerender();
    int curr_program = 0;

    const Transform model = Identity();
    const Transform view = camera.view();
    const Transform projection = camera.projection();
    // const Transform projection = Perspective(50,1024/576,0.1,100);
    const Transform mvp = projection * view * model;
    const Transform mv = view * model;
    // Lambda to set shader uniforms
    auto setUniforms = [&](GLuint program)
    {
        program_uniform(program, "modelMatrix", model);           // Set model matrix
        program_uniform(program, "viewMatrix", view);             // Set view matrix
        program_uniform(program, "projectionMatrix", projection); // Set projection matrix
        program_uniform(program, "mvMatrix", mv);                 // Set model-view matrix
        program_uniform(program, "mvpMatrix", mvp);               // Set model-view-projection matrix
    };

    // Filling G-buffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, culler.fbo);

    // Model data
    glBindVertexArray(m_vao[0]);

    // Texture
    // program_use_texture(m_programs[0], "texture_samp", 0, m_texture[0]);
    // program_use_texture_array(m_programs[0], "texture_samp_arr", 1, m_texture[1]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Model pass
    curr_program = 1;
    glUseProgram(m_programs[curr_program]);
    setUniforms(m_programs[curr_program]);

    glDrawElements(GL_TRIANGLES, m_objet[0].indices.size(), GL_UNSIGNED_INT, 0);

    // Deffered pass
    /*glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    curr_program = 0;
    glUseProgram(m_programs[curr_program]);
    setUniforms(m_programs[curr_program]);
    program_use_texture(m_programs[curr_program], "texd", 3, culler.color);

    glDrawElements(GL_TRIANGLES, m_objet[0].indices.size(), GL_UNSIGNED_INT, 0);*/

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Compute shader
    curr_program = 2;
    glUseProgram(m_programs[curr_program]);
    // setUniforms(m_programs[curr_program]);
    bindcomputeTexture(m_programs[curr_program], culler.position, culler.color, culler.zbuffer, culler.normal, culler.output);
    glDispatchCompute(global_width / 16, global_height / 16, 1);
    // glDrawElements(GL_TRIANGLES, m_objet[0].indices.size(), GL_UNSIGNED_INT, 0);

////
    glBindFramebuffer(GL_READ_FRAMEBUFFER, read_framebuffer);
    glBlitFramebuffer(/* source */ 0,0, global_width,global_height,  /* destination */ 0,0,  global_width,global_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    // Pass it to the screen i guess
}

void quit()
{
    for (const GLuint &vao : m_vao)
    {
        release_buffers(vao);
    }
}

int main(int argc, char **argv)
{

    Window window = create_window(global_width, global_height);
    Context context = create_context(window);

    // etat openGL de base / par defaut
    glClearColor(0.1, 0.6, 0.1, 1);
    glClearDepth(1);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // glEnable(GL_STENCIL_TEST);
    // sfail: action to take if the stencil test fails.
    // dpfail: action to take if the stencil test passes, but the depth test fails.
    // dppass: action to take if both the stencil and the depth test pass.
    // glStencilOp(GL_KEEP,GL_KEEP, GL_REPLACE);

    if (!init())
        return 1;

    while (events(window))
    {
        // dessiner
        draw();

        // presenter / montrer le resultat
        SDL_GL_SwapWindow(window);
    }

    quit();

    release_context(context);
    release_window(window);
    return 0;
}
