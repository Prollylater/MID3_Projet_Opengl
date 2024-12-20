
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
std::vector<vec4> m_cubes_instance_data;
std::vector<vec4> m_cubes_instance_data_visible;

//Should have been used for additionnal thing in the map
std::vector<std::vector<vec4>> m_objet_translations;
std::vector<std::vector<vec4>> m_objet_translations_visibles;

//Buffer
std::vector<GLuint> m_objet_translate_buffer;

shadowMapFBO shadowstuff;

int global_width = 1024;
int global_height = 576;

bool init(int regions_div, int map_type)
{

    camera.projection(global_width, global_height, 50);
    MeshIOData tmp;
    if (!read_meshio_data("./data/cube_world/Blocks/OBJ/Block_Ice.obj", tmp))
        return false;
    m_objet.push_back(tmp);
    tmp = MeshIOData();

    // LOADING TEXTURES
    std::vector<const char *> m_texture_names;
    // LOADING TEXTURES ARRAY
    fillWithTexturesNames(m_texture_names);

    m_texture.push_back(read_texture_array(0, m_texture_names));

    const char *height_map;
    const char *biome_map;
    // Check if the argument is 1 or 2, and modify file paths accordingly
    if (map_type != 3)
    {
        if (map_type == 2)
        {
            height_map = "./data/terrain/hmapfull.png";
            biome_map = "./data/terrain/mapfull.png";
        }
        else if (map_type == 1)
        {
            height_map = "./data/terrain/hmapmedium.png";
            biome_map = "./data/terrain/mapmedium.png";
        }
        else
        {
            height_map = "./data/terrain/hmapsmall.png";
            biome_map = "./data/terrain/mapsmall.png";
        }
        regions_size = generateTerrain(m_cubes_instance_data, biome_map, height_map,
                                       regions, regions_div);
    }
    else
    {
        height_map = "./data/terrain/terrain.png";

        regions_size = genWorldFromHmap(m_cubes_instance_data, height_map,
                                        regions, regions_div);
    }

    m_objet_translate_buffer.resize(1);
    GLuint tmp_vao = create_buffers_instancesV(m_objet_translate_buffer[0], m_objet[0].positions, m_objet[0].indices,
                                               m_objet[0].texcoords, m_objet[0].normals, m_cubes_instance_data);
    std::cout << "Individual regions size:" << regions_size << std::endl;
    std::cout << "Number of regions:" << regions.size() << std::endl;

    m_vao.push_back(tmp_vao);

    // charge et compile les shaders
    tmp_vao = read_program("projets/cube_shading.glsl");
    shadowstuff.init(global_width, global_height, "projets/cube_shading_shad.glsl");

    m_programs.push_back(tmp_vao);
    m_cubes_instance_data_visible.resize(m_cubes_instance_data.size());

    // affiche les erreurs de maniere lisible.
    for (unsigned &progam : m_programs)
    {
        program_print_errors(progam);
    }

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

// Global  for tracking mouse states and movement
float dist_an_x = 0, dist_an_y = 0, dist_an_z = 0;
float pitch, yaw = 0.0f;
// Keyboard control TODO:Decomission after 3rd person mvm
bool moved = true;

void draw()
{

    glViewport(0, 0, global_width, global_height);
    camera.manageCameraLight(moved);

    const Transform model = Identity();   // placer le modele coordonnées des sommets
    const Transform view = camera.view(); // camera
    const Transform projection = camera.projection();

    const Transform mvp = projection * view * model;
    const Transform mv = view * model;
    auto setUniforms = [&](GLuint program)
    {
        program_uniform(program, "modelMatrix", model);           // Set model matrix
        program_uniform(program, "viewMatrix", view);             // Set view matrix
        program_uniform(program, "projectionMatrix", projection); // Set projection matrix
        program_uniform(program, "mvMatrix", mv);                 // Set model-view matrix
        program_uniform(program, "mvpMatrix", mvp);               // Set model-view-projection matrix
    };

    // Handling Reegion Boxes
    if (moved)
    { //
        m_cubes_instance_data_visible.clear();

        moved = false;
        std::cout << "New turn" << std::endl;

        for (int i = 0; i < regions.size(); i++)
        {

            if (insideFrustum(regions[i], mvp))
            {
                std::copy(m_cubes_instance_data.begin() + (i * regions_size), m_cubes_instance_data.begin() + ((i + 1) * regions_size), std::back_inserter(m_cubes_instance_data_visible));
            }
        }

        glNamedBufferSubData(m_objet_translate_buffer[0], 0, m_cubes_instance_data_visible.size() * sizeof(vec4),
                             m_cubes_instance_data_visible.data());

        std::cout << m_cubes_instance_data_visible.size() << " visibles object for " << m_cubes_instance_data.size() << std::endl;
    }
    /////////////////////////////////////////////////////////////

    // Compute the MVP matrix from the light's point of view,
    Vector lightInvDir = normalize(Vector(0., 1., 0.));
    // Point lightPosition = Point(0., 25., 0.);
    Point lightPosition = Point(-50., 55., 100.);
    Point lightInvsDir = Point(1., 1., 1.);

    glBindVertexArray(m_vao[0]);

    // Shadow Pass
    shadowstuff.BindForWriting();

    // Random values till renderdoc output put out a decent shadow

    Transform depthProjectionMatrix = Ortho(-25, 25, -25, 25, 0.1, 1000.0);
    Transform depthViewMatrix = Lookat(lightPosition, lightInvsDir, Vector(0, 1, 0));
    Transform depthModelMatrix = Identity();
    Transform depthmvp = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

    program_uniform(shadowstuff.shadowprog, "lightSpaceMatrix", depthmvp);

    glDrawElementsInstanced(GL_TRIANGLES, m_objet[0].indices.size(), GL_UNSIGNED_INT, 0, m_cubes_instance_data_visible.size());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Model pass
    glUseProgram(m_programs[0]);
    setUniforms(m_programs[0]);
    depthmvp = shadowstuff.returnBiasMat() * depthmvp;

    // Uniform
    program_uniform(m_programs[0], "lightSpaceMatrix", depthmvp);
    program_uniform(m_programs[0], "lightdir", lightInvDir);

    // Texture
    program_use_texture_array(m_programs[0], "texture_samp_arr", 0, m_texture[0]);
    shadowstuff.BindForReading(2, "shadow_texture", m_programs[0]);

    glDrawElementsInstanced(GL_TRIANGLES, m_objet[0].indices.size(), GL_UNSIGNED_INT, 0, m_cubes_instance_data_visible.size());
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
    int regions_divide = 16;
    if (argc > 2)
        regions_divide = atoi(argv[2]);

    int map_type = 0;
    if (argc > 1)
        map_type = atoi(argv[1]);
    Window window = create_window(global_width, global_height);
    Context context = create_context(window);

    // etat openGL de base / par defaut
    glClearColor(0.0, 0.0, 0.f, 1.0f);
    glClearDepth(1);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    if (!init(regions_divide, map_type))
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
