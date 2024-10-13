
#include "bezier.h"

#include "deformations.h"

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

bool init()
{

    BezierPatches patch;

    MeshIOData tmp;

    patch.create_patch(3, 3);

    std::vector<Vector> vertices;
    std::vector<Vector> faces;
    std::vector<Vector> normals;
    

    
    patch.convert_to_mesh(30,vertices,faces, normals);
     
     patch.writeMeshBezier("./ezez.obj",vertices, faces, normals);
    
     if (!read_meshio_data("./ezez.obj", tmp))
    {
        return false;
    }
        m_objet.push_back(tmp);

    
    GLuint tmp_vao = create_buffers(tmp.positions, tmp.indices,std::vector<Point>(),tmp.normals);

    /*patch.convert_to_mesh(10, vertices, faces, normals); */

    // patch.writeMeshBezier("Bez.obj", vertices, faces, normals);
   /* if (!read_meshio_data("./data/hearthspring.obj", tmp))
    {
        return false;
    }
*/
    //m_objet.push_back(tmp);
    
/**/




    // MeshDeform bezier_deform= TwistDeform::twistMesh(m_objet[0].positions,m_objet[0].indices,m_objet[0].normals,1,0.5);
    /*MeshDeform bezier_deform = LocalDeformations::onSphere::warpMesh(m_objet[0].positions, m_objet[0].indices, m_objet[0].normals,
                                                                      Vector(0, 1, 0), m_objet[0].positions[0], 0.2);


     std::cout << tmp.positions.size() << std::endl;
     std::cout << bezier_deform.positions.size() << std::endl;
    
    GLuint tmp_vao = create_buffers(m_objet[0].positions, m_objet[0].indices,
                                    m_objet[0].texcoords, m_objet[0].normals);

     GLuint tmp_vao = create_buffers(bezier_deform.positions, bezier_deform.indices,
                                     m_objet[0].texcoords, bezier_deform.normals);
 

    FreeFormDeform deformer;
    deformer.createGrid(Point(-1,-1,0), 2, 4, 4, 4);
    deformer.randomModif();

    MeshDeform freee = deformer.warpMesh(m_objet[0].positions, m_objet[0].indices, m_objet[0].normals);
   
    //writeMeshDeform("Freeeeeee.obj", freee);
    */
    /*GLuint tmp_vao = create_buffers(freee.positions, freee.indices,
                                    m_objet[0].texcoords, m_objet[0].normals);*/

    m_vao.push_back(tmp_vao);
    // charge et compile les shaders
    tmp_vao = read_program("projets/cube_shading.glsl");
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

    for (float i = 0; i < patch.control_point.size(); i++)
    {
        for (float j = 0; j < patch.control_point[i].size(); j++)
        {
            glBegin(GL_POINTS);
            glColor3f(1.0, 0.0, 0.1);
            glPointSize(100);
            glVertex3f(patch.control_point[i][j].x, patch.control_point[i][j].y, patch.control_point[i][j].z);
            glEnd();
        }
    }

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
    // program_use_texture(program, "shadowsampler", );

    // glBindTexture(GL_TEXTURE_2D, texture);
    // glDrawElements(GL_TRIANGLES, m_objet[0].indices.size(), GL_UNSIGNED_INT, nullptr);
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
