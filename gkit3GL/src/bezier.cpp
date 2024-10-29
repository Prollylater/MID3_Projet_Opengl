#include "bezier.h"

// Just in case you need 25
inline double compute_bernst_basis(float n, float t, float k);
inline double compute_bernst_basis_deriv(float n, float t, float k);
// n,k
// Avec n le degré de la coube +1

inline void BezierPatches::add_control_point(float x, float y, float z, int axis)
{
    control_point[axis].push_back(Vector(x, y, z));
}

void BezierPatches::create_patch(uint8_t degree_m, uint8_t degree_n)
{

    control_point.resize(degree_m);
    for (uint8_t m = 0; m < degree_m; m++)
    {
        control_point[m].resize(degree_n);
        for (uint8_t n = 0; n < degree_n; n++)
        {

            float x = static_cast<float>(m) * 2.0f;
            float y = static_cast<float>(n) * 2.5f;
            float z = std::sin(x * 0.5f) * std::cos(y * 1.0f) * 5.0f;

            control_point[m][n] = {x/5, y/5, z/5};
        }
    }
    /*
    float z = 0;
    for (float m = 0; m < degree_m; m++)
    {
        control_point[m].resize(degree_n);
        for (float n = 0; n < degree_n; n++)
        {
            if (n == 1 & m == 1)
            {
                z = 1;
            }
            control_point[m][n] = {(m) / 5, z, (n) / 5};
            z = 0;
        }
    }
    */
}

Vector BezierPatches::compute_partial_derivU(float u, float v)
{
    Vector pu;
    int m = control_point.size();

    for (int i = 0; i < m - 1; i++)
    {
        int n = control_point[i].size();
        float bernstein_u = compute_bernst_basis(m - 1, u, i);

        for (int j = 0; j < control_point[i].size(); j++)
        {
            //                float bernstein_v = compute_bernst_basis_deriv(n, v, j);
            float bernstein_v = compute_bernst_basis(n, v, j);
            pu = pu + m * (control_point[i + 1][j] - control_point[i][j]) * bernstein_u * bernstein_v;
        }
    }

    return pu;
}

Vector BezierPatches::compute_partial_derivV(float u, float v)
{
    Vector pv;
    int n = control_point[0].size();
    int m = control_point.size();

    for (int i = 0; i < m; i++)
    {
        int n = control_point[i].size();
        float bernstein_u = compute_bernst_basis(m, u, i);
        for (int j = 0; j < n - 1; j++)
        {
            // float bernstein_v = compute_bernst_basis_deriv(n-1, v, j);

            float bernstein_v = compute_bernst_basis(n - 1, v, j);
            pv = pv + m * (control_point[i][j + 1] - control_point[i][j]) * bernstein_u * bernstein_v;
        }
    }

    return pv;
}

Vector BezierPatches::compute_point(float u, float v)
{
    Vector p_uv(0.0, 0.0, 0.0);
    int n = control_point[0].size();
    int m = control_point.size();
    int counter = 0;
    for (int i = 0; i < m; i++)
    {
        float bernstein_u = compute_bernst_basis(m - 1, u, i);
        for (int j = 0; j < n; j++)
        {
            float bernstein_v = compute_bernst_basis(n - 1, v, j);
            p_uv = p_uv + (control_point[i][j] * bernstein_u * bernstein_v);
            // std::cout<<"NEwround"<< p_uv<< " " <<u<< " "<<v<< std::endl;
            // std::cout<<"BErny"<< bernstein_u<< " " <<bernstein_v<< std::endl;
            // counter++;
        }
    }
    // std::cout<<"Iteration"<< counter<< " for " <<m<< " "<<n<< std::endl;

    return p_uv;
}

void BezierPatches::convert_to_mesh(int nb_vertex, std::vector<Vector> &vertices,
                                    std::vector<Vector> &faces, std::vector<Vector> &normals)
{
    if (nb_vertex < 2)
        return;
    // Get Géométrie

    // Check that control point axis 1 exist
    vertices.clear();
    faces.clear();
    normals.clear();

    vertices.resize(nb_vertex * nb_vertex);
    normals.resize(nb_vertex * nb_vertex);

    float step = 1.0f / (nb_vertex - 1);
    for (int i = 0; i < nb_vertex; i++)
    {
        float u = i * step;

        for (int j = 0; j < nb_vertex; j++)
        {

            float v = j * step;
            std::cout << "Computing " << i << " " << j << std::endl;
            vertices[i * nb_vertex + j] = compute_point(u, v);
            Vector partialU = compute_partial_derivU(u, v);
            Vector partialV = compute_partial_derivV(u, v);

            // Check for NaN before normalizing
            if (!std::isnan(partialU.x) || !std::isnan(partialV.x))
            {
                normals[i * nb_vertex + j] = normalize(cross(partialU, partialV));
            }
            else
            {
                normals[i * nb_vertex + j] = Vector(0, 0, 0); // Default normal or handle error
            }
            if (std::isnan(normals[i * nb_vertex + j].x))
            {
                std::cout << "weird point at it " << i << " and j " << j << std::endl;
            }
        }
    }

    std::cout << "Stitching the face" << std::endl;

    // Get Topologie
    faces.reserve(2 * ((nb_vertex - 1) * (nb_vertex - 1)));
    for (int i = 1; i < nb_vertex - 1; i++)
    {
        for (int j = 1; j < nb_vertex - 1; j++)
        {
            faces.emplace_back(i * nb_vertex + j, (i + 1) * nb_vertex + j, (i + 1) * nb_vertex + j + 1);
            faces.emplace_back(i * nb_vertex + j, (i + 1) * nb_vertex + j + 1, i * nb_vertex + j + 1);
        }
    }
    std::cout << faces.size() << std::endl;
}
void BezierPatches::returnPointGrid(std::vector<Point> &vertices,
                                    std::vector<unsigned> &faces)
{
    vertices.clear();
    faces.clear();

    for (int i = 0; i < control_point.size(); i++)
    {
        for (int j = 0; j < control_point[i].size() - 1; j++)
        {
            vertices.emplace_back(2 * control_point[i][j].x, 2 * control_point[i][j].y, 2 * control_point[i][j].z);
        }
    } 

    int rows = control_point[0].size();
    int cols = control_point.size();

    // Create lines between adjacent points
    for (int i = 0; i < cols - 1; ++i)
    {
        for (int j = 0; j < rows - 1; ++j)
        { // TODO: Precompute value that are "common"
            faces.emplace_back(j * cols + i);       // Top left
            faces.emplace_back((j + 1) * cols + i); // Bottom left
            faces.emplace_back(j * cols + i + 1);   // Top right

            faces.emplace_back(j * cols + i + 1);       // Top right
            faces.emplace_back((j + 1) * cols + i);     // Bottom left
            faces.emplace_back((j + 1) * cols + i + 1); // Bottom right
        }
    }
}

bool BezierPatches::writeMeshBezier(const char *filename, std::vector<Vector> vertices, std::vector<Vector> faces, std::vector<Vector> normals)
{

    // Create and open a file
    std::ofstream mesh_file(filename);

    if (!mesh_file)
    {
        std::cerr << "Error opening file!" << std::endl;
        return 0;
    }

    for (auto vert_it = vertices.begin(); vert_it != vertices.end(); vert_it++)
    {
        mesh_file << "v " << (*vert_it).x
                  << " " << (*vert_it).y
                  << " " << (*vert_it).z << "\n";
    }
    int counter = 0;
    std::cout << vertices.size() << std::endl;

    if (normals.size())
    {

        for (auto normal_it = normals.begin(); normal_it != normals.end(); normal_it++)
        {
            mesh_file << "vn " << (*normal_it).x
                      << " " << (*normal_it).y
                      << " " << (*normal_it).z << "\n";
        }
        for (auto face_it = faces.begin(); face_it != faces.end(); face_it++)
        {
            mesh_file << "f " << (*face_it).x << "//" << (*face_it).x
                      << " " << (*face_it).y << "//" << (*face_it).y
                      << " " << (*face_it).z << "//" << (*face_it).z
                      << "\n";
        }
        std::cout << faces.size() << std::endl;
        std::cout << faces.size() << std::endl;
    }

    else
    {
        for (auto face_it = faces.begin(); face_it != faces.end(); face_it++)
        {
            mesh_file << "f " << (*face_it).x
                      << " " << (*face_it).y
                      << " " << (*face_it).z
                      << "\n";
        }
    }

    std::cout << "Data written!" << std::endl;

    return 0;
};

// Degree of the curves = n/m
// Indices of the control point k/i, control point
// Binomials n,k/
// Divide bernstein basis in three terms
double compute_bernst_basis(float n, float t, float k)
{
    // could even calculate (all B n,k in a row) and return it
    //  t^k = term a;
    if (k < 0 || k > n)
        return 0.0;
    int a = n;                                                                  // Total degree and not number of ctrl point
    int b = k;                                                                  // Basis index
                                                                                // Output each part
    float term1 = std::pow(t, k);                                               // Make sure (t)^0 is  1
    float term2 = (t == 1 && (n - k) == 0) ? 1.0f : std::pow((1 - t), (n - k)); // Make sure (1-t)^0 is  1 if t == 1
    return binomial_coeff[a][b] * term1 * term2;
}

double compute_bernst_basis_deriv(float n, float t, float k)
{

    if (k > n)
    {
        return 0.0;
    }
    int a = n; // Total degree
    int b = k; // Basis index

    // Derivative formula
    double term1 = binomial_coeff[a][b] * b * std::pow(t, b - 1) * std::pow((1 - t), (a - b));
    double term2 = binomial_coeff[a][b - 1] * (a - b) * std::pow(t, b) * std::pow((1 - t), (a - b - 1));

    return term1 - term2;
}

/* Test
int main()
{
    BezierPatches patch;
    std::cout << "ge" << std::endl;

    patch.create_patch(4, 4);

    std::vector<Vector> vertices;
    std::vector<Vector> faces;
    std::vector<Vector> normals;

    std::cout << "ge" << std::endl;
    patch.convert_to_mesh(10, vertices, faces, normals);
    std::cout << "ge" << std::endl;

    write_mesh("Bez.obj", vertices, faces, 10);
    return 0;

    std::cout << "ge" << std::endl;

    patch.convert_to_mesh(25, vertices, faces, normals);
    write_mesh("Bezi.obj", vertices, faces, 25);
    std::cout << "ge" << std::endl;

    patch.convert_to_mesh(50, vertices, faces, normals);
    write_mesh("Bezie.obj", vertices, faces, 50);
    return 0;
}*/
