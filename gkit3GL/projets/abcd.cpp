
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
int global_width = 1024;
int global_height = 576;

int compute_shader_type = 2;

// Structure handling the ressource, supportuing the occlusion culling
struct GBufferstruct
{
    // creation des textures pour stocker le GBuffer
    GLuint fbo;
    GLuint zbuffer;
    // GLuint position;
    GLuint normal;
    GLuint color;
    GLuint output;

    GLenum buffers_enum[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                              GL_COLOR_ATTACHMENT2};

    GBufferstruct() = default;

    bool initOccCuller()
    {
        // Textures
        zbuffer = make_depth_texture(0, global_width, global_height);
        // position = make_vec4_texture(0, global_width, global_height);
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
        // glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, position, 0);
        glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, normal, 0);
        glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, output, 0);

        glDrawBuffers(3, buffers_enum);

        if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "Errror during FBO for Occlusion culling init" << std::endl;
            return false;
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        return true;
    }
};

bool bindcomputeTexture(GLuint program, GLuint color, GLuint zbuffer, GLuint normal, GLuint output, GLuint position = {})
{
    // Positions
    // glBindImageTexture(0, position, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);

    glBindImageTexture(0, color, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);

    // Zbuffer
    glBindImageTexture(1, zbuffer, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32UI);

    // normal
    glBindImageTexture(2, normal, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);

    // output texture
    glBindImageTexture(3, output, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    return true;
}

GBufferstruct culler;

bool init()
{
    auto padd_width = [](int num)
    {
        if (num % 4 != 0)
        {
            return num + (4 - num % 4);
        }
    };
    // global_height=padd_width(global_height);
    // global_width = padd_width(global_width);

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
    tmp_vao = read_program("projets/dacs_compute_shader.glsl");
    m_programs.push_back(tmp_vao);
    tmp_vao = read_program("projets/dacs_compute_shader_one.glsl");
    m_programs.push_back(tmp_vao);
    tmp_vao = read_program("projets/dacs_compute_shader_mem.glsl");
    m_programs.push_back(tmp_vao);
    tmp_vao = read_program("projets/dacs_compute_shader_two.glsl");
    m_programs.push_back(tmp_vao);
    //tmp_vao = read_program("projets/dacs_compute_shade_color.glsl");
   // m_programs.push_back(tmp_vao);

    culler.initOccCuller();
    ///

    glGenFramebuffers(1, &read_framebuffer);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, read_framebuffer);
    glFramebufferTexture(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, culler.output, 0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

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

void draw_normal()
{

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Model pass
    curr_program = 0;
    glUseProgram(m_programs[curr_program]);
    setUniforms(m_programs[curr_program]);

    glDrawElements(GL_TRIANGLES, m_objet[0].indices.size(), GL_UNSIGNED_INT, 0);
}

void draw()
{

    camera.prerender();
    int curr_program = 0;

    const Transform model = Identity();
    const Transform view = camera.view();
    const Transform projection = camera.projection();
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

    auto dispatchDacs = [&](int curr_phase, int curr_program)
    {
        program_uniform(m_programs[curr_program], "column_pixels", (uint)global_width); // Set model matrix
        program_uniform(m_programs[curr_program], "row_pixels", (uint)global_height);   // Set model matrix

        int pattern_radius = 2;
        int block_size_x = 0;
        if (curr_phase < 3)
        {
            block_size_x = 4;
            pattern_radius = 2;
        }
        // Notes: This gave an illegal instruction
        // TODO: Lookup Why
        // else if (curr_phase >= 3 && curr_phase < 5)

        else if (curr_phase >= 3)
        {
            block_size_x = 2;
            pattern_radius = 1;
        }

        if (curr_phase < 10)
        {

            program_uniform(m_programs[curr_program], "phase", (uint)curr_phase); // Set model matrix
            program_uniform(m_programs[curr_program], "block_step", (uint)block_size_x);
            program_uniform(m_programs[curr_program], "pattern_radius", pattern_radius);
        }
        else if (curr_phase == 11)
        {
            const int pattern_radiusb = 2;
            const int block_size_xb = 4;
            program_uniform(m_programs[curr_program], "block_step", (uint)block_size_xb);
            program_uniform(m_programs[curr_program], "pattern_radius", pattern_radiusb);

            program_uniform(m_programs[curr_program], "block_stepb", (uint)block_size_x);
            program_uniform(m_programs[curr_program], "pattern_radiusb", pattern_radius);
        }

        // x Pattern
        //+ Pattern
        // Pour phase 0 on a

        // Get the number of block each thread should deal with hence the number of group
        int num_thread_x = (global_width + block_size_x - 1) / block_size_x;
        int num_thread_y = (global_height + block_size_x - 1) / block_size_x;

        //
        glDispatchCompute((num_thread_x + 7) / 8, (num_thread_y + 7) / 8, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
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

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Compute shader
    curr_program = compute_shader_type;
    glUseProgram(m_programs[curr_program]);
    bindcomputeTexture(m_programs[curr_program], culler.color, culler.zbuffer, culler.normal, culler.output);
    // glDispatchCompute(global_width / 8, global_height / 8, 1);
    // 16 by 16 group
    if (curr_program == 2 ||  curr_program == 6)
    {
        std::cout<<"Classic 5 Dispatch"<<std::endl;

        dispatchDacs(0, curr_program);
        dispatchDacs(1, curr_program);
        dispatchDacs(2, curr_program);
        dispatchDacs(3, curr_program);
        dispatchDacs(4, curr_program);
    }
    else if (curr_program == 3)
    {
        std::cout<<"One Dispatch Naive"<<std::endl;

        dispatchDacs(10, curr_program);
    }
    else if (curr_program == 4)
    {
        std::cout<<"One Dispatch Mem Opti"<<std::endl;

        dispatchDacs(11, curr_program);
    }
    else if (curr_program == 5)
    {
        std::cout<<"Dual Dispatch"<<std::endl;

        dispatchDacs(0, curr_program);
        dispatchDacs(4, curr_program);
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, read_framebuffer);
    // Blit from the currenlty bound ReadFrameBuffer to the currently bound DRaw  buffer
    glBlitFramebuffer(/* source */ 0, 0, global_width, global_height, /* destination */ 0, 0, global_width, global_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
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

    if (argc > 1)
    {
        compute_shader_type = atoi(argv[1]);
        if (!(compute_shader_type >= 2 && compute_shader_type < 6))
        {
            compute_shader_type = 2;
        }
    }

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
