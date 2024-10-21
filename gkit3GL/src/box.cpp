// Box from https://github.com/eric-galin/TinyMesh

// Self include
#include "box.h"

/*!
\class Box box.h
\brief An axis aligned box.

The class stores the opposite two corners as vectors.
The center and the radius (diagonal vector) are computed on the
fly by inline functions.

The vertices of a box can be obtained by the Box::Vertex()
member function which returns one of the eight vertices of the box.
The two opposite corners can be obtained faster as follows:

\code
Box box(Vector(0.0,0.0,0.0),Vector(1.0,1.0,1.0)); // Unit box
Vector a=box[0]; // Lower vertex
Vector b=box[1]; // Opposite vertex
\endcode
*/

const double Box::epsilon = 1.0e-5; //!< Epsilon value used to check intersections and some round off errors.

const int Box::edge[24] =
{
  0,1,2,3,4,5,6,7,
  0,2,1,3,4,6,5,7,
  0,4,1,5,2,6,3,7
};

const Vector Box::normal[6] =
{
  Vector(-1.0,0.0,0.0),
  Vector(0.0,-1.0,0.0),
  Vector(0.0,0.0,-1.0),
  Vector(1.0,0.0,0.0),
  Vector(0.0, 1.0,0.0),
  Vector(0.0,0.0,1.0)
};

/*!
\brief Create a box given a center point and the half side length.
\param c Center.
\param r Half side length.
*/
Box::Box(const Vector& c, double r)
{
  a = c - Vector(r,r,r);
  b = c + Vector(r,r,r);
}

/*!
\brief Create a box given two opposite corners.

Note that this constructor does not check the coordinates of the two vectors.
Therefore, the coordinates of a should be lower than those of b.

To create the axis aligned bounding box of two vectors a and b in
the general case, one should use:
\code
Box box(Vector::Min(a,b),Vector::Max(a,b));
\endcode
\param a,b End vertices.
*/
Box::Box(const Vector& a, const Vector& b)
{
  Box::a = a;
  Box::b = b;
}



/*!
\brief Overloaded.
\param s Stream.
\param box The box.
*/
std::ostream& operator<<(std::ostream& s, const Box& box)
{
  s << "Box(" << box.a << ',' << box.b << ")";
  return s;
}

/*!
\brief Translates a box.

\param t Translation vector.
*/
void Box::Translate(const Vector& t)
{
  a = a + t;
  b = b + t;
}

/*!
\brief Scales a box.

Note that this function handles negative coefficients in
the scaling vector (by swapping coordinates if need be).
\param s Scaling.
*/
void Box::Scale(double s)
{
  a = a* s;
  b = b*s;

  // Swap coordinates for negative coefficients 
  if (s < 0.0)
  {
    Vector t = a;
    a = b;
    b = t;
  }
}
