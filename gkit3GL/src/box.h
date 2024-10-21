// Box from https://github.com/eric-galin/TinyMesh

#pragma once

#include <vector>
#include <iostream>

#include "vec.h"

struct Box
{
  Vector a, b; //!< Lower and upper vertex.
  //! Empty.
  Box() {}
  explicit Box(const Vector &, double);
  explicit Box(const Vector &, const Vector &);
  //! Empty.
  ~Box() = default;

  // Access vertexes
  Vector &operator[](int);
  Vector operator[](int) const;

  // Acces to vertices
  Vector Center() const;
  Vector Vertex(int) const;

  Vector Size() const;
  Vector Diagonal() const;
  double Radius() const;

  bool Inside(const Box &) const;
  bool Inside(const Vector &) const;

  double Volume() const;
  double Area() const;

  // Compute sub-box
  Box Sub(int) const;

  // Translation, scale
  void Translate(const Vector &);
  void Scale(double);

  friend std::ostream &operator<<(std::ostream &, const Box &);

public:
  static const double epsilon;   //!< Internal \htmlonly\epsilon;\endhtmlonly for ray intersection tests.
  static const Box Null;         //!< Empty box.
  static const int edge[24];     //!< Edge vertices.
  static const Vector normal[6]; //!< Face normals.
};

//! Returns either end vertex of the box.
inline Vector &Box::operator[](int i)
{
  if (i == 0)
    return a;
  else
    return b;
}

//! Overloaded.
inline Vector Box::operator[](int i) const
{
  if (i == 0)
    return a;
  else
    return b;
}

//! Returns the center of the box.
inline Vector Box::Center() const
{
  return 0.5 * (a + b);
}

/*!
\brief Returns the diagonal of the box.
*/
inline Vector Box::Diagonal() const
{
  return (b - a);
}

/*!
\brief Compute the size (width, length and height) of a box.
\sa Box::Diagonal()
*/
inline Vector Box::Size() const
{
  return b - a;
}

/*!
\brief Returns the radius of the box, i.e. the length of the half diagonal of the box.
*/
inline double Box::Radius() const
{
  return 0.5 * length(b - a);
}

//! Compute the volume of a box.
inline double Box::Volume() const
{
  Vector side = b - a;
  return side.x * side.y * side.z;
}

/*!
\brief Compute the surface area of a box.
*/
inline double Box::Area() const
{
  Vector side = b - a;
  return 2.0 * (side.x * side.y + side.x * side.z + side.y * side.z);
}

/*!
\brief Check if a point is inside the box.
\param p Point.
*/
inline bool Box::Inside(const Vector &p) const
{
  return (((a.x < p.x) && (a.y < p.y) && (a.z < p.z)) &&
          ((b.x > p.x) && (b.y > p.y) && (b.z > p.z)));
}

/*!
\brief Check if an argument box is inside the box.
\param box The box.
*/
inline bool Box::Inside(const Box &box) const
{
  return this->Inside(box.a) && this->Inside(box.b);
}
