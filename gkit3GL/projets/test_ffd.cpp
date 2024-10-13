// exemple : lecture et affichage d'un mesh, avec un shader de base + gestion des evenements par window.h

#include "vec.h"

#include "bezier.h"

#include "deformations.h"

#include "mat.h"

#include "window.h"

#include "glcore.h"

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

BezierPatches patch;
unsigned int textureID = 0;

static Point point_id;
// FFD Box Grid
std::vector<Point> vertices_grid;
std::vector<unsigned> ffd_grid_indices;

FreeFormDeform deformer;

bool init()
{
    BezierPatches patch;

    MeshIOData tmp;

    std::vector<Vector> vertices;
    std::vector<Vector> faces;
    std::vector<Vector> normals;

    if (!read_meshio_data("./data/hearthspring.obj", tmp))
    {
        return false;
    }

    m_objet.push_back(tmp);

    deformer.createBoundingGrid(m_objet[0].positions, 3);
    int point_id_val = deformer.control_points.size();
    point_id = Point(point_id_val, point_id_val, point_id_val);
    //  deformer.createGrid(Point(0, 0, 0), 2, 3, 3, 3);

    MeshDeform freee = deformer.warpMesh(m_objet[0].positions, m_objet[0].indices, m_objet[0].normals);

    // writeMeshDeform("Freeeeeee.obj", freee);

    // deformer.randomModif();
    // freee = deformer.warpMesh(m_objet[0].positions, m_objet[0].indices, m_objet[0].normals);
    GLuint tmp_vao = create_buffers(freee.positions, freee.indices,
                                    m_objet[0].texcoords, m_objet[0].normals);

    m_vao.push_back(tmp_vao);
    // charge et compile les shaders

    std::cout << vertices_grid.size() << std::endl;
    std::cout << ffd_grid_indices.size() << std::endl;

    deformer.outputGrid(vertices_grid, ffd_grid_indices);
    std::cout << vertices_grid.size() << std::endl;
    std::cout << ffd_grid_indices.size() << std::endl;

    tmp_vao = create_buffers(vertices_grid, ffd_grid_indices,
                             m_objet[0].texcoords, m_objet[0].normals);
    m_vao.push_back(tmp_vao);

    tmp_vao = read_program("projets/test_ffd_shading.glsl");
    m_programs.push_back(tmp_vao);

    // affiche les erreurs de maniere lisible.
    for (unsigned &progam : m_programs)
    {
        program_print_errors(progam);
    }

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
    return true;
};

// Global cause why not
float dist_an_x = 0;
float dist_an_y = 0;
float dist_an_z = 0;

float pitch = 0.0f;
float yaw = 0.0f;

//TODO MAkeshift solution till the app is build more solidly around interaction
void input()
{
    // Classic variable

    static float cam_speed = 0.1f;
    ;
    static int i;

    if (key_state('a'))
    {
        pitch += 0.6f;
    }
    if (key_state('e'))
    {
        pitch -= 0.6f;
    }
    if (key_state('z'))
    {
        yaw -= 0.3f;
    }
    if (key_state('s'))
    {
        yaw += 0.3f;
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
    /*
    if (key_state('0'))
    {
        point_id.x = point_id.x + 1 % deformer.control_points.size();
    }
    if (key_state('9'))
    {
        point_id.y = point_id.y + 1 % deformer.control_points.size();
    }
    if (key_state('8'))
    {
        point_id.z = point_id.z + 1 % deformer.control_points.size();
    }
    if (key_state('1'))
    {
        deformer.control_points[point_id.x][point_id.y][point_id.z].x += cam_speed / 5;
       // deformer.warpMesh(m_objet[0].positions, m_objet[0].indices, m_objet[0].normals);

    }
    if (key_state('2'))
    {
        deformer.control_points[point_id.x][point_id.y][point_id.z].x -= cam_speed / 5;
        //deformer.warpMesh(m_objet[0].positions, m_objet[0].indices, m_objet[0].normals);

    }
    if (key_state('3'))
    {
        deformer.control_points[point_id.x][point_id.y][point_id.z].y += cam_speed / 5;
        //deformer.warpMesh(m_objet[0].positions, m_objet[0].indices, m_objet[0].normals);

    }
    if (key_state('4'))
    {
        deformer.control_points[point_id.x][point_id.y][point_id.z].y -= cam_speed / 5;
        //deformer.warpMesh(m_objet[0].positions, m_objet[0].indices, m_objet[0].normals);
    }
*/
    return;
};

static int count = 0;
void draw()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT /*| GL_STENCIL_BUFFER_BIT*/);
    input();
    Transform projection = Perspective(40, 1024.0 / 576.0, 0.1, 100);
    Transform model = Identity(); // placer le modele coordonnées des sommets

    Transform view = Inverse(model * RotationX(yaw) * RotationY(pitch) * Translation(dist_an_x, dist_an_y, dist_an_z + 10.5f)); // camera
    // Model data
    Transform mvp = projection * view * model;
    Transform mv = view * model;

    // Point de contrôle

    // Model pass
    glUseProgram(m_programs[0]);

    glViewport(0, 0, 1024, 576);
    ////////////////////////////////////////////////////////////
    // Switching the point
    count = count +1 %20;
    //deformer.randomModif();
    //deformer.warpMesh(m_objet[0].positions, m_objet[0].indices, m_objet[0].normals);

    //update_buffers(m_vao[0], m_objet[0].positions, m_objet[0].indices);
    /////////////////////////////////////////////////////////////
    // Uniform
    program_uniform(m_programs[0], "modelMatrix", model);
    program_uniform(m_programs[0], "viewMatrix", view);
    program_uniform(m_programs[0], "projectionMatrix", projection);
    program_uniform(m_programs[0], "mvMatrix", mv);
    program_uniform(m_programs[0], "mvpMatrix", mvp);
    // program_use_texture(program, "shadowsampler", );

    // glBindTexture(GL_TEXTURE_2D, texture);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

    glBindVertexArray(m_vao[1]);
    glPointSize(5.0f);
    glDrawElements(GL_POINTS, ffd_grid_indices.size(), GL_UNSIGNED_INT, 0);
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
    glClearColor(0.1, 0.1, 0.1, 1);
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
