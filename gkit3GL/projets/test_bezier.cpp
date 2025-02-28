

#include "bezier.h"

#include "window.h"

#include "mesh_io.h"

#include "buffers.h"

#include "program.h"

#include "uniforms.h"

#include "texture.h"

#include <algorithm>

// Store all the charged program
std::vector<GLuint> m_programs;
// Store all the charged program
std::vector<GLuint> m_vao;
std::vector<Vector> objet1_translations;

// Store all the charged object annd the associated transform matrices
std::vector<MeshIOData> m_objet;
std::vector<GLuint> m_texture;

std::vector<Point> grid_vert;
std::vector<unsigned int> grid_faces;

BezierPatches patch;
unsigned int textureID = 0;
#include <chrono>

bool init()
{
    MeshIOData tmp;
    BezierPatches patch;
    auto start = std::chrono::high_resolution_clock::now();

    // Create a patch of arg * arg control points
    patch.create_patch(5, 5);

    std::vector<Vector> vertices;
    std::vector<Vector> faces;
    std::vector<Vector> normals;
    // 10 30 100
    patch.convert_to_mesh(50, vertices, faces, normals);

    patch.writeMeshBezier("./bezier_patch.obj", vertices, faces, normals);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "Function execution time: " << duration.count() << " ms" << std::endl;

    std::cout << "Saved mesh creation" << std::endl;

    patch.returnPointGrid(grid_vert, grid_faces);

    if (!read_meshio_data("./bezier_patch.obj", tmp))
    {
        return false;
    }
    m_objet.push_back(tmp);

    GLuint tmp_vao = create_buffers(tmp.positions, tmp.indices, std::vector<Point>(), tmp.normals);
    m_vao.push_back(tmp_vao);

    tmp_vao = create_buffers(grid_vert, grid_faces);
    m_vao.push_back(tmp_vao);
    // charge et compile les shaders
    tmp_vao = read_program("projets/test_bezier_shading.glsl");
    m_programs.push_back(tmp_vao);

    // affiche les erreurs de maniere lisible.
    for (unsigned &progam : m_programs)
    {
        program_print_errors(progam);
    }

    for (const unsigned &program : m_programs)
    {
        if (!program_ready(program))
        {
            return false;
        }
        // erreur de compilation / link / chargement des shaders
    }
    return true;
};

// Global cause why not
float dist_an_x = 0;
float dist_an_y = 0;
float dist_an_z = 0;

float pitch = 0.0f;
float yaw = 0.0f;

void input()
{
    // Classic variable

    static float cam_speed = 0.05f;
    ;
    static int i;

    if (key_state('z'))
    {
        pitch += 0.1f;
    }
    if (key_state('s'))
    {
        pitch -= 0.1f;
    }
    if (key_state('a'))
    {
        yaw -= 0.1f;
    }
    if (key_state('e'))
    {
        yaw += 0.1f;
    }

    if (key_state('r'))
    {
        dist_an_x -= 0.05f;
    }
    if (key_state('t'))
    {
        dist_an_x += 0.05f;
    }
    if (key_state('f'))
    {
        dist_an_y -= 0.05f;
    }
    if (key_state('g'))
    {
        dist_an_y += 0.05f;
    }
    if (key_state('v'))
    {
        dist_an_z -= 0.05f;
    }
    if (key_state('b'))
    {
        dist_an_z += 0.05f;
    }
    return;
};

void draw()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT /*| GL_STENCIL_BUFFER_BIT*/);
    // input(toWorldTr, toWorldRt, toWorldrotate, toWorldsc);
    input();
    Transform projection = Perspective(40, 1024.0 / 576.0, 0.1, 100);
    Transform model = Identity(); // placer le modele coordonnées des sommets

    Transform view = Inverse(model * RotationX(yaw) * RotationY(pitch) * Translation(dist_an_x, dist_an_y, dist_an_z + 10.5f)); // camera
    // Model data
    Transform mvp = projection * view * model;
    Transform mv = view * model;

    glBindVertexArray(m_vao[0]);

    // Point de contrôle

    // Model pass
    glUseProgram(m_programs[0]);

    glViewport(0, 0, 1024, 576);
    /////////////////////////////////////////////////////////////
    // Uniform
    program_uniform(m_programs[0], "modelMatrix", model);
    program_uniform(m_programs[0], "viewMatrix", view);
    program_uniform(m_programs[0], "projectionMatrix", projection);
    program_uniform(m_programs[0], "mvMatrix", mv);
    program_uniform(m_programs[0], "mvpMatrix", mvp);

    glBindVertexArray(m_vao[1]);
    glPointSize(5.0f);
    glDrawElements(GL_POINTS, grid_faces.size(), GL_UNSIGNED_INT, 0);
    glPointSize(1.0f);

    glBindVertexArray(m_vao[0]);

    glDrawElements(GL_TRIANGLES, m_objet[0].indices.size(), GL_UNSIGNED_INT, 0);
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
    glClearColor(0.3, 0.3, 0.3, 1);
    glClearDepth(1);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
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
