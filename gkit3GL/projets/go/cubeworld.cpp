
#include "window.h"
#include "glcore.h"
#include "mesh_io.h"
#include "buffers.h"
#include "program.h"
#include "uniforms.h"
#include "texture.h"
#include <algorithm>
#include "mapgen.h"
#include "orbiter.h"

struct World
{ // TODO. Define structure and camera early + Clean import
};

Orbiter camera;

// Store all the charged program
std::vector<GLuint> m_programs;
// Store all the charged program
std::vector<GLuint> m_vao;
// Store all the charged object annd the associated transform matrices
std::vector<MeshIOData> m_objet;
// Store all the textures in the program
std::vector<GLuint> m_texture;
std::vector<Region> regions;
static int regions_size = 0;

// Store all the translations of the different object
std::vector<std::vector<Vector>> m_objet_translations;
std::vector<unsigned> m_cube_texture_id; // Store the correct texture id for the cube
// Store all the translations of the different object currently visible
std::vector<std::vector<Vector>> m_objet_translations_visibles;
std::vector<unsigned> m_cube_texture_id_visible;
std::vector<GLuint> m_objet_translate_buffer;
GLuint m_objet_text_id_buffer;

bool init()
{

    camera.projection(1024, 576, 50);

    MeshIOData tmp;
    if (!read_meshio_data("./data/cube_world/Blocks/OBJ/Block_Grass.obj", tmp))
        return false;
    m_objet.push_back(tmp);
    tmp = MeshIOData();
    // m_objet.push_back(tmp);

    // LOADING TEXTURES
    std::vector<const char *> m_texture_names;
    // LOADING TEXTURES ARRAY
    m_texture.push_back(read_texture(0, "./data/cube_world/Atlas.png"));
    m_texture_names.push_back("./data/cube_world/Textures/grassy.png");
    m_texture_names.push_back("./data/cube_world/Textures/water.png");
    m_texture_names.push_back("./data/cube_world/Textures/snow2.png");
    m_texture_names.push_back("./data/cube_world/Textures/stone1.png");
    m_texture_names.push_back("./data/cube_world/Textures/granite.png");
    m_texture_names.push_back("./data/cube_world/Textures/bground.png");
    m_texture_names.push_back("./data/cube_world/Textures/grass.png");
    m_texture_names.push_back("./data/cube_world/Textures/red.png");
    m_texture_names.push_back("./data/cube_world/Textures/pearm.png");
    m_texture_names.push_back("./data/cube_world/Textures/weird.png");
    m_texture_names.push_back("./data/cube_world/Textures/weird2.png");
    m_texture_names.push_back("./data/cube_world/Textures/weird2.png");

    m_texture.push_back(read_texture_array(0, m_texture_names));
    // Translations related manipulation
    // m_objet_translations.resize(1);
    m_objet_translate_buffer.resize(1);
    // regions_size = genWorldFromHmap(m_objet_translations[0], "./data/terrain/mymapgr.png", regions, 5);

    // TODO:Test this implementation
    // regions_size = generateTerrain(m_objet_translations[0], "./data/terrain/mymapgr.png", regions, 5);
    /* regions_size = generateTerrain(m_objet_translations, m_cube_texture_id, "./data/terrain/mapfull.png", "./data/terrain/heightmap.png",
                      regions, 1, 10);*/
    regions_size = generateTerrain(m_objet_translations, m_cube_texture_id, "./data/terrain/mapsmall.png", "./data/terrain/hmapsmall.png",
                                   regions, 1, 4);
    GLuint tmp_vao = create_buffers_instancesV(m_objet_translate_buffer[0], m_objet_text_id_buffer, m_objet[0].positions, m_objet[0].indices,
                                               m_objet[0].texcoords, m_objet[0].normals, m_objet_translations[0], m_cube_texture_id);

    m_vao.push_back(tmp_vao);

    // charge et compile les shaders
    tmp_vao = read_program("projets/cube_shading.glsl");

    m_programs.push_back(tmp_vao);
    m_objet_translations_visibles.resize(m_objet_translations.size());
    m_cube_texture_id_visible.resize(m_cube_texture_id.size());

    // affiche les erreurs de maniere lisible.
    for (unsigned &progam : m_programs)
    {
        program_print_errors(progam);
    }

    /*
    // affiche les erreurs de maniere lisible.
    program_print_errors(skyboxprog);
    */
    glBindTexture(GL_TEXTURE_2D, 0);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT /*| GL_STENCIL_BUFFER_BIT*/);
    input();
    camera.prerender();

    const Transform model = Identity();   // placer le modele coordonnées des sommets
    const Transform view = camera.view(); // camera
                                          //  Transform view = Inverse(model * RotationX(yaw) * RotationY(pitch) * Translation(-dist_an_x, dist_an_y, -dist_an_z)); // camera
    const Transform projection = camera.projection();
    // const Transform projection = Perspective(50,1024/576,0.1,100);

    // Model data

    const Transform mvp = projection * view * model;
    const Transform mv = view * model;

    glBindVertexArray(m_vao[0]);
    // Model pass
    glUseProgram(m_programs[0]);

    glViewport(0, 0, 1024, 576);

    // Handling Reegion Boxes
    if (moved)
    { //
        m_objet_translations_visibles[0].clear();
        m_cube_texture_id_visible.clear();

        moved = false;
        std::cout << "New turn" << std::endl;

        for (int i = 0; i < regions.size(); i++)
        {

            if (insideFrustum(regions[i], mvp))
            {
                std::copy(m_objet_translations[0].begin() + (i * regions_size), m_objet_translations[0].begin() + ((i + 1) * regions_size), std::back_inserter(m_objet_translations_visibles[0]));
                std::copy(m_cube_texture_id.begin() + (i * regions_size), m_cube_texture_id.begin() + ((i + 1) * regions_size), std::back_inserter(m_cube_texture_id_visible));
            }
        }

        // Update the buffer with the visible translations
        // TODO: Some adjustment are still needed

        glNamedBufferSubData(m_objet_translate_buffer[0], 0, m_objet_translations_visibles[0].size() * sizeof(Vector),
                             m_objet_translations_visibles[0].data());
        glNamedBufferSubData(m_objet_text_id_buffer, 0, m_cube_texture_id_visible.size() * sizeof(GLuint),
                             m_cube_texture_id_visible.data());

        // glBindBuffer(GL_ARRAY_BUFFER, buffer_translations);
        // glBufferSubData(GL_ARRAY_BUFFER, 0, visibleTranslations.size() * sizeof(glm::vec3), visibleTranslations.data());

        std::cout << m_objet_translations_visibles[0].size() << " visibles object for " << m_objet_translations[0].size() << std::endl;
    }
    /////////////////////////////////////////////////////////////

    // Uniform
    program_uniform(m_programs[0], "modelMatrix", model);
    program_uniform(m_programs[0], "viewMatrix", view);
    program_uniform(m_programs[0], "projectionMatrix", projection);
    program_uniform(m_programs[0], "mvMatrix", mv);
    program_uniform(m_programs[0], "mvpMatrix", mvp);

    // Texture
    unsigned int texture_arr_index = 0;
    program_use_texture(m_programs[0], "texture_samp", 0, m_texture[0]);
    program_use_texture_array(m_programs[0], "texture_samp_arr", 1, m_texture[1]);

    glDrawElementsInstanced(GL_TRIANGLES, m_objet[0].indices.size(), GL_UNSIGNED_INT, 0, m_objet_translations[0].size());
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

    Window window = create_window(1024, 576);
    Context context = create_context(window);

    // etat openGL de base / par defaut
    glClearColor(0.1, 0.1, 0.1, 1);
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
