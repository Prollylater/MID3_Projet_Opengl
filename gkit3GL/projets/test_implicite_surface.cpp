#include "sdf.h"

int main()
{
  std::cout << "Creating NOde" << std::endl;
  /*ImplicitSurface *node = new SmoothUnion(new Cone(1.0f, Vector(0, 0, 0)),
                                          (new Torus(1.0f, 0.6f)));
  */ std::cout << "Sculpture creation" << std::endl;
  // Structure_Tree sculpture(new Intersection(new Sphere(0.9f, Vector(0, 0, 0)), new Sphere(0.9f, Vector(0, 1, 1))));
  //  Structure_Tree sculpture(new Sphere(0.5f,Vector(0,0,0)));
  //   Structure_Tree sculpture(new DifferenceRvachev(new Sphere(0.5f,Vector(0,0,0)) ,new Sphere(0.5f,Vector(0,0.3,0))));
  //  Structure_Tree sculpture(new Torus(0.2f,0.3f));
  //  Structure_Tree sculpture(new Capsule(1.0f,Vector(0,0,0),Vector(0,1,0)));
  //  Structure_Tree sculpture(new Rhombus(0.4, 0.4, 0.05, 0.05,Vector(0,0,0)));

  ImplicitSurface *leaf = new Rhombus(0.8, 0.2, 0.1, 0.4, Vector(0, 0, 0));
  // ImplicitSurface* leafb= new Rhombus(0.4, 0.4, 0.05, 0.05,Vector(0,0,0));

  Structure_Tree sculpture(leaf);

  // Sort of ok
  //  Structure_Tree sculpture(new Cone(2.8f,0.1f, Vector(0, 0, 0)));

  // BAD
  //

  // Structure_Tree sculpture(new Ellipsoid(0.6f,Vector(0,0,0)));

  std::cout << "Mehs creation" << std::endl;
  Mesh_imp mesh;
  sculpture.Polygonize(100, mesh, sculpture.Bbox, eps);
  std::cout << "Saveg mesh creation" << std::endl;

  mesh.saveMeshObj("Implicitsur.obj");
}
