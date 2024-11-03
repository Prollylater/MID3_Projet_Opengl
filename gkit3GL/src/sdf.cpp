#include "sdf.h"

bool ImplicitSurface::inside(const Vector &point) const
{
    return this->evaluate(point) < 0;
};


 inline Vector ImplicitSurface::derivateapprox(const Vector &point) const
{
    float dfx = evaluate(Vector(point.x + eps, point.y, point.z)) - evaluate(Vector(point.x - eps, point.y, point.z));
    float dfy = evaluate(Vector(point.x, point.y + eps, point.z)) - evaluate(Vector(point.x, point.y - eps, point.z));
    float dfz = evaluate(Vector(point.x, point.y, point.z + eps)) - evaluate(Vector(point.x, point.y, point.z - eps));
    return Vector(dfx, dfy, dfz);
}
Vector ImplicitSurface::gradient(const Vector &point) const
{
    return derivateapprox(point) / (2 * eps);
}
Vector ImplicitSurface::normal(const Vector &point) const
{
    return normalize(derivateapprox(point));
};

Vector max_vec(const Vector &a, const Vector &b)
{
    return Vector(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}

Vector min_vec(const Vector &a, const Vector &b)
{
    return Vector(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}

/*!
\brief Compute the polygonal Mesh_imp approximating the implicit surface.

\param box %Box defining the region that will be polygonized.
\param n Discretization parameter.
\param g Returned geometry.
\param epsilon Epsilon value for computing vertices on straddling edges.
*/
void ImplicitSurface::Polygonize(int n, Mesh_imp &g, const Box &box, const double &epsilon) const
{
    std::vector<Vector> vertex;
    std::vector<Vector> normal;

    std::vector<int> triangle;

    vertex.reserve(20000);
    normal.reserve(20000);
    triangle.reserve(20000);

    int nv = 0;
    const int nx = n;
    const int ny = n;
    const int nz = n;

    Box clipped = box;

    // Clamped integer values
    const int nax = 0;
    const int nbx = nx;
    const int nay = 0;
    const int nby = ny;
    const int naz = 0;
    const int nbz = nz;

    const int size = nx * ny;

    // Intensities
    double *a = new double[size];
    double *b = new double[size];

    // Vertex
    Vector *u = new Vector[size];
    Vector *v = new Vector[size];

    // Edges
    int *eax = new int[size];
    int *eay = new int[size];
    int *ebx = new int[size];
    int *eby = new int[size];
    int *ez = new int[size];

    // Diagonal of a cell
    Vector d = clipped.Diagonal() / (n - 1);

    double za = 0.0;
    std::cout<<"Point 1"<<std::endl;
    // Compute field inside lower Oxy plane
    for (int i = nax; i < nbx; i++)
    {
        for (int j = nay; j < nby; j++)
        {
            u[i * ny + j] = clipped[0] + Vector(i * d.x, j * d.y, za);
            a[i * ny + j] = this->evaluate(u[i * ny + j]);
        }
    }
    std::cout<<"Point 2"<<std::endl;
    // Compute straddling edges inside lower Oxy plane
    for (int i = nax; i < nbx - 1; i++)
    {
        for (int j = nay; j < nby; j++)
        {
            // We need a xor b, which can be implemented a == !b
            if (!((a[i * ny + j] < 0.0) == !(a[(i + 1) * ny + j] >= 0.0)))
            {
                vertex.push_back(Dichotomy(u[i * ny + j], u[(i + 1) * ny + j], a[i * ny + j], a[(i + 1) * ny + j], d.x, epsilon));
                normal.push_back(this->normal(vertex.back()));
                eax[i * ny + j] = nv;
                nv++;
            }
        }
    }
    std::cout<<"Point 3"<<std::endl;

    for (int i = nax; i < nbx; i++)
    {
        for (int j = nay; j < nby - 1; j++)
        {
            if (!((a[i * ny + j] < 0.0) == !(a[i * ny + (j + 1)] >= 0.0)))
            {
                vertex.push_back(Dichotomy(u[i * ny + j], u[i * ny + (j + 1)], a[i * ny + j], a[i * ny + (j + 1)], d.y, epsilon));
                normal.push_back(this->normal(vertex.back()));
                eay[i * ny + j] = nv;
                nv++;
            }
        }
    }

    // Array for edge vertices
    int e[12];

    std::cout<<"Point 4"<<std::endl;

    // For all layers
    for (int k = naz; k < nbz; k++)
    {
        double zb = za + d.z;
        for (int i = nax; i < nbx; i++)
        {
            for (int j = nay; j < nby; j++)
            {
                v[i * ny + j] = clipped[0] + Vector(i * d.x, j * d.y, zb);
                b[i * ny + j] = this->evaluate(v[i * ny + j]);
            }
        }

        // Compute straddling edges inside lower Oxy plane
        for (int i = nax; i < nbx - 1; i++)
        {
            for (int j = nay; j < nby; j++)
            {
                //   if (((b[i*ny + j] < 0.0) && (b[(i + 1)*ny + j] >= 0.0)) || ((b[i*ny + j] >= 0.0) && (b[(i + 1)*ny + j] < 0.0)))
                if (!((b[i * ny + j] < 0.0) == !(b[(i + 1) * ny + j] >= 0.0)))
                {
                    vertex.push_back(Dichotomy(v[i * ny + j], v[(i + 1) * ny + j], b[i * ny + j], b[(i + 1) * ny + j], d.x, epsilon));
                    normal.push_back(this->normal(vertex.back()));
                    ebx[i * ny + j] = nv;
                    nv++;
                }
            }
        }

        for (int i = nax; i < nbx; i++)
        {
            for (int j = nay; j < nby - 1; j++)
            {
                // if (((b[i*ny + j] < 0.0) && (b[i*ny + (j + 1)] >= 0.0)) || ((b[i*ny + j] >= 0.0) && (b[i*ny + (j + 1)] < 0.0)))
                if (!((b[i * ny + j] < 0.0) == !(b[i * ny + (j + 1)] >= 0.0)))
                {
                    vertex.push_back(Dichotomy(v[i * ny + j], v[i * ny + (j + 1)], b[i * ny + j], b[i * ny + (j + 1)], d.y, epsilon));
                    normal.push_back(this->normal(vertex.back()));
                    eby[i * ny + j] = nv;
                    nv++;
                }
            }
        }

        // Create vertical straddling edges
        for (int i = nax; i < nbx; i++)
        {
            for (int j = nay; j < nby; j++)
            {
                // if ((a[i*ny + j] < 0.0) && (b[i*ny + j] >= 0.0) || (a[i*ny + j] >= 0.0) && (b[i*ny + j] < 0.0))
                if (!((a[i * ny + j] < 0.0) == !(b[i * ny + j] >= 0.0)))
                {
                    vertex.push_back(Dichotomy(u[i * ny + j], v[i * ny + j], a[i * ny + j], b[i * ny + j], d.z, epsilon));
                    normal.push_back(this->normal(vertex.back()));
                    ez[i * ny + j] = nv;
                    nv++;
                }
            }
        }

        // Create Mesh_imp
        for (int i = nax; i < nbx - 1; i++)
        {
            for (int j = nay; j < nby - 1; j++)
            {
                int cubeindex = 0;
                if (a[i * ny + j] < 0.0)
                    cubeindex |= 1;
                if (a[(i + 1) * ny + j] < 0.0)
                    cubeindex |= 2;
                if (a[i * ny + j + 1] < 0.0)
                    cubeindex |= 4;
                if (a[(i + 1) * ny + j + 1] < 0.0)
                    cubeindex |= 8;
                if (b[i * ny + j] < 0.0)
                    cubeindex |= 16;
                if (b[(i + 1) * ny + j] < 0.0)
                    cubeindex |= 32;
                if (b[i * ny + j + 1] < 0.0)
                    cubeindex |= 64;
                if (b[(i + 1) * ny + j + 1] < 0.0)
                    cubeindex |= 128;

                // Cube is straddling the surface
                if ((cubeindex != 255) && (cubeindex != 0))
                {
                    e[0] = eax[i * ny + j];
                    e[1] = eax[i * ny + (j + 1)];
                    e[2] = ebx[i * ny + j];
                    e[3] = ebx[i * ny + (j + 1)];
                    e[4] = eay[i * ny + j];
                    e[5] = eay[(i + 1) * ny + j];
                    e[6] = eby[i * ny + j];
                    e[7] = eby[(i + 1) * ny + j];
                    e[8] = ez[i * ny + j];
                    e[9] = ez[(i + 1) * ny + j];
                    e[10] = ez[i * ny + (j + 1)];
                    e[11] = ez[(i + 1) * ny + (j + 1)];

                    for (int h = 0; TriangleTable[cubeindex][h] != -1; h += 3)
                    {
                        triangle.emplace_back(e[TriangleTable[cubeindex][h + 0]]);
                        triangle.emplace_back(e[TriangleTable[cubeindex][h + 1]]);
                        triangle.emplace_back(e[TriangleTable[cubeindex][h + 2]]); 
                    }
                }
            }
        }

        std::swap(a, b);

        za = zb;
        std::swap(eax, ebx);
        std::swap(eay, eby);
        std::swap(u, v);
    }
    std::cout<<"Point 5"<<std::endl;


    delete[] a;
    delete[] b;
    delete[] u;
    delete[] v;

    delete[] eax;
    delete[] eay;
    delete[] ebx;
    delete[] eby;
    delete[] ez;

    std::vector<int> normals = triangle;

    g = Mesh_imp(vertex, normal, triangle, normals);
}

/*!
\brief Compute the intersection between a segment and an implicit surface.

\param a,b End vertices of the segment straddling the surface.
\param va,vb Field function value at those end vertices.
\param length Distance between vertices.
\param epsilon Precision.
\return Point on the implicit surface.
*/
Vector ImplicitSurface::Dichotomy(Vector a, Vector b, double va, double vb, double length, const double &epsilon) const
{
    int ia = va > 0.0 ? 1 : -1;

    // Get an accurate first guess
    Vector c = (vb * a - va * b) / (vb - va);

    while (length > epsilon)
    {
        double vc = this->evaluate(c);
        int ic = vc > 0.0 ? 1 : -1;
        if (ia + ic == 0)
        {
            b = c;
        }
        else
        {
            ia = ic;
            a = c;
        }
        length *= 0.5;
        c = 0.5 * (a + b);
    }
    return c;
}
