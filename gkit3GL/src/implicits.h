// from https://github.com/eric-galin/TinyMesh
//  Implicits

#pragma once

#include <iostream>
#include "mat.h"
#include "box.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cmath>

struct Mesh_imp
{
  std::vector<Vector> vertices; //!< Vertices.
  std::vector<Vector> normals;  //!< Normals.
  std::vector<int> varray;      //!< Vertex indexes.
  std::vector<int> narray;      //!< Normal indexes.
  Mesh_imp() = default;
  Mesh_imp(const std::vector<Vector> &vert,
           const std::vector<Vector> &norm,
           const std::vector<int> &varr,
           const std::vector<int> &nar) : vertices(vert), normals(norm), varray(varr), narray(nar) {};
  void saveMeshObj(const char *name) const;
};

class AnalyticScalarField
{
protected:
public:
  AnalyticScalarField();
  virtual double Value(const Vector &) const;
  virtual Vector Gradient(const Vector &) const;

  // Normal
  virtual Vector Normal(const Vector &) const;

  // Dichotomy
  Vector Dichotomy(Vector, Vector, double, double, double, const double & = 1.0e-4) const;

  virtual void Polygonize(int, Mesh_imp &, const Box &, const double & = 1e-4) const;

protected:
  static const double Epsilon; //!< Epsilon value for partial derivatives
protected:
  static int TriangleTable[256][16]; //!< Two dimensionnal array storing the straddling edges for every marching cubes configuration.
  static int edgeTable[256];         //!< Array storing straddling edges for every marching cubes configuration.
};
