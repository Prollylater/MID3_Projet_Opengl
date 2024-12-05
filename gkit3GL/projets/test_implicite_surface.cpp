#include "sdf.h"

#include "window.h"
#include <chrono>
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

unsigned int textureID = 0;

bool init()
{
  MeshIOData tmp;

  //////////////////////////////////////////////////CSG DEF
  //////////////////////////////////////////////////CSG DEF

  std::cout << "Creating NOde" << std::endl;
  std::cout << "Sculpture creation" << std::endl;

  // VIOLET
  // Violet Leaf
  // ImplicitSurface *leaf_v = new Bend(new Rotation_trs(new Rhombus(2.5, 0.4, 0.1, 1.4, Vector(0, 0, 0)),5,2 ), 0.05,0);
  //  ImplicitSurface *leaf_v = new Rotation_trs(new Rhombus(2.3, 0.4, 0.1, 1.4, Vector(0, 0, 0)), 2, 2);
  ImplicitSurface *leaf_v = new Bend(new Rotation_trs(new Rhombus(2.3, 0.4, 0.1, 1.4, Vector(0, 0, 0)), 2, 2), -0.02, 0);
  ImplicitSurface *leaf_v_a = new TranslationSdf(new Rotation_trs(leaf_v, 210, 1), Vector(3.2, -0.1, -0.7)); // 0 is ok form nice, leaf is 0?5 // Positive and minus good
  ImplicitSurface *leaf_v_b = new TranslationSdf(new Rotation_trs(leaf_v, -30, 1), Vector(-3.2, -0.1, -0.7));
  ImplicitSurface *leaf_v_c = new TranslationSdf(new Rotation_trs(leaf_v, 90, 1), Vector(0.0, -0.1, 4.6f));
  ImplicitSurface *leafs_v = new Union(new Union(leaf_v_a, leaf_v_b), leaf_v_c);

  // Violet Support
  ImplicitSurface *support_v_octo = new Difference(new Octo(1.2), new TranslationSdf(new Boxsdf(Vector(1.0, 1.0, 1.0), Vector(0.0, 0.0, 0.0)), Vector(0.0, 0.5f, 0.0f)));
  ImplicitSurface *support_v_deco = new Union(new Torus(0.2f, 1.0f), support_v_octo);
  ImplicitSurface *support_v_c = new TranslationSdf(support_v_deco, Vector(0.0, -0.1, 1.1f));
  ImplicitSurface *surface_v = (new Union(leafs_v, support_v_c));
  // VIOLET
  // INDIGO
  //  Indigo Leaf
  ImplicitSurface *leaf_i = new Rhombus(0.5, 0.10, 0.1, 0.6, Vector(0, 0, 0));
  ImplicitSurface *leaf_i_hollow = new Difference(leaf_i, (new Rhombus(0.5, 0.09, 0.1, 0.3, Vector(0, 0, 0))));
  ImplicitSurface *leaf_i_hollow_rot = new Rotation_trs(leaf_i_hollow, 10, 2);

  ImplicitSurface *leaf_i_hollow_rot_a = new TranslationSdf(new Rotation_trs(leaf_i_hollow_rot, 20, 1), Vector(2.4, 0.0, 0.0));
  ImplicitSurface *leaf_i_hollow_rot_b = new TranslationSdf(new Rotation_trs(leaf_i_hollow_rot, 160, 1), Vector(-2.4, 0.0, 0.0));
  ImplicitSurface *leaf_i_hollow_rot_c = new TranslationSdf(new Rotation_trs(leaf_i_hollow_rot, -90, 1), Vector(0.0, 0.0, 3.5f));
  ImplicitSurface *leafs_i = new Union(new Union(leaf_i_hollow_rot_a, leaf_i_hollow_rot_b), leaf_i_hollow_rot_c);

  ImplicitSurface *support_i_deco = new TranslationSdf((new Torus(0.15f, 0.9f)), Vector(0.0, -0.5, 0.9));

  ImplicitSurface *surface_i = new TranslationSdf(new Union(support_i_deco, leafs_i), Vector(0.0, 0.8, -0.15));

  // INDIGO
  // GREEN

  ImplicitSurface *leaf_g = new Bend(new Rhombus(0.30, 0.20, 0.12, 0.20, Vector(0, 0, 0)), -0.1, 0);
  ImplicitSurface *leaf_g2_bend = new Rotation_trs(new Bend(new Rhombus(0.35, 0.10, 0.1, 0.20, Vector(0, 0, 0)), 0.30, 0), -30, 2);
  ImplicitSurface *leaf_g2_b_r = new TranslationSdf(new Rotation_trs(leaf_g2_bend, 90, 2), Vector(-1.0, 1.6, 0.0));
  ImplicitSurface *crown_g = new SmoothUnion(leaf_g2_b_r, leaf_g);
  ImplicitSurface *crown_g_b = new TranslationSdf(new Rotation_trs(crown_g, 120, 1), Vector(-4.0, 0, -2.3));
  ImplicitSurface *crown_g_c = new TranslationSdf(new Rotation_trs(crown_g, 240, 1), Vector(-4.0, 0, 2.4));

  ImplicitSurface *surface_g = new TranslationSdf(new Union(new Union(crown_g, crown_g_b), crown_g_c), Vector(2.760, 0.4, 1.));
  ImplicitSurface *surface_g_move = new Rotation_trs(surface_g, -28, 1);
  // GREEN

  // PINK
  ImplicitSurface *structure_p1 = new TranslationSdf(new Cone(2.7, 0.9, Vector(0, 0, 0)), Vector(0.0, 2.0, 0.0));
  ImplicitSurface *structure_p2 = new TranslationSdf(new Rotation_trs(new Cone(1.0, 0.9, Vector(0, 0, 0)), 180, 2), Vector(0.0, -2.0, 0.0));
  ImplicitSurface *structure_p3 = new TranslationSdf(new Octo(0.9), Vector(0.0, 0.0, 0.0));
  ImplicitSurface *surface_p = new Union(new Union(new Bend(structure_p1, 0.2, 0), structure_p2), structure_p3);
  ImplicitSurface *surface_p_in_scne = new TranslationSdf(surface_p, Vector(0.0, 4.0, 1.1));
  // PINK

  // YELLOW
  ImplicitSurface *leaf_y = new Rotation_trs(new Rhombus(0.6, 0.12, 0.1, 0.5, Vector(0, 0, 0)), -20, 2);
  ImplicitSurface *leaf_y_a = new TranslationSdf(new Rotation_trs(leaf_y, -30, 1), Vector(-3.1, 0.0, -1.7));
  ImplicitSurface *leaf_y_b = new TranslationSdf(new Rotation_trs(leaf_y, 210, 1), Vector(3.1, 0.0, -1.7));
  ImplicitSurface *leaf_y_c = new TranslationSdf(new Rotation_trs(leaf_y, 90, 1), Vector(0.0, 0.0, 3.4f));
  ImplicitSurface *leafs_y = new Union(new Union(leaf_y_a, leaf_y_b), leaf_y_c);

  ImplicitSurface *support_y_deco = new TranslationSdf((new Torus(0.17f, 1.0f)), Vector(0.0, -1.0, 0.0));
  ImplicitSurface *surface_y = new Union(leafs_y, support_y_deco);
  ImplicitSurface *surface_y_moved = new TranslationSdf(new Rotation_trs(surface_y, 180, 1), Vector(-0.4, 1.3, 1.0));

  // YELLOW

  // RED
  // ImplicitSurface *structure_r = new Difference( new Rhombus(0.6, 0.8, 0.1, 0.4, Vector(0, 0, 0)) , leaf_i);// Double triangle //Essentially when  Lb is biggger// Strertch with raising 4.0
  /*ImplicitSurface *structure_r1 = new Displacement_sdf(new Capsule(0.5f, Vector(0, 0, 0), Vector(0, 1.1, 0)));
  ImplicitSurface *structure_r2 = new Union(new Capsule(0.81f, Vector(0, -0.5, 0), Vector(0, 1.8, 0)), structure_r1);
  ImplicitSurface *structure_r3 = new Difference(structure_r2, new Capsule(0.7f, Vector(0, 0, 0), Vector(0, 1.8, 0)));
  ImplicitSurface *structure_r3_in_scene = new TranslationSdf(structure_r3, Vector(0.0, 1.50, 1.1));

  */

  ImplicitSurface *structure_r1 = new Displacement_sdf(new Capsule(0.5f, Vector(0, 0, 0), Vector(0, 1.1, 0)));
  ImplicitSurface *structure_r2 = new Union(new Capsule(0.8f, Vector(0, -0.5, 0), Vector(0, 1.8, 0)), structure_r1);
  ImplicitSurface *structure_r3 = new Difference(structure_r2, new Capsule(0.7f, Vector(0, 0, 0), Vector(0, 1.89, 0)));
  ImplicitSurface *structure_r3_in_scene = new TranslationSdf(structure_r3, Vector(0.0, 1.40, 1.1));
  //  RED

  ImplicitSurface *linker_leaf = new TranslationSdf(new SmoothUnion(surface_g_move, surface_y_moved), Vector(0.5, 0.0, 0));
  ImplicitSurface *leafs_final = (new Union(surface_i, surface_v));
  ImplicitSurface *center_pistil = new TranslationSdf((new Union(surface_p_in_scne, structure_r3_in_scene)), Vector(0.0, 0.4, 0.0));
  Structure_Tree sculpture(new Union(new Union(leafs_final, center_pistil), linker_leaf));

  std::cout << "Mesh creation" << std::endl;

  Mesh_imp mesh;
  auto start = std::chrono::high_resolution_clock::now();

  sculpture.Polygonize(200, mesh, sculpture.Bbox, eps);
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> duration = end - start;
  std::cout << "Function execution time: " << duration.count() << " ms" << std::endl;

  std::cout << "Saved mesh creation" << std::endl;

  mesh.saveMeshObj("Implicitsur.obj");

  //////////////////////////////////////////////////CSG DEF
  //////////////////////////////////////////////////CSG DEF
  // Create a patch of arg * arg control points

  for (auto &vert : mesh.vertices)
  {
    tmp.positions.emplace_back(vert.x, vert.y, vert.z);
  }
  for (auto &vert : mesh.varray)
  {
    tmp.indices.push_back((unsigned int)vert);
  }
  for (auto &vert : mesh.normals)
  {
    tmp.normals.emplace_back(vert.x, vert.y, vert.z);
  }

  // tmp.positions = mesh.vertices;
  // tmp.indices = mesh.varray;
  // tmp.normals = mesh.normals;
  m_objet.push_back(tmp);

  GLuint tmp_vao = create_buffers(tmp.positions, tmp.indices, std::vector<Point>(), tmp.normals);
  m_vao.push_back(tmp_vao);

  // charge et compile les shaders
  tmp_vao = read_program("projets/test_sdf_shading.glsl");
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

  if (key_state('a'))
  {
    pitch += 0.4f;
  }
  if (key_state('e'))
  {
    pitch -= 0.4f;
  }
  if (key_state('z'))
  {
    yaw -= 0.4f;
  }
  if (key_state('s'))
  {
    yaw += 0.4f;
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
