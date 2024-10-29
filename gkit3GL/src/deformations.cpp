#include "deformations.h"

void FreeFormDeform::createGrid(const Point &position, float step, float size_x,
                                float size_y, float size_z)
{
    control_points.resize(size_x);
    for (float i = 0; i < size_x; i++)
    {
        control_points[i].resize(size_y);
        for (float j = 0; j < size_y; j++)
        {
            for (float k = 0; k < size_z; k++)
            {
                Point tmp = position + Point((i /size_x), (j /size_y), (k /size_z));
                control_points[i][j].emplace_back(tmp.x, tmp.y, tmp.z);
                // std::cout << "one of our point is: " << control_points[i][j][k] << std::endl;
                // std::cout << tmp << std::endl;
            }
        }
    }
    // Or define repere prior to this and then use repere for position ?
    //  Determine S , T ,U
    // Should be good
    // SHould i switch betwween y and z ?
    repere.at(0) = normalize(control_points[1][0][0] - control_points[0][0][0]); //* size_x);
    repere.at(1) = normalize(control_points[0][1][0] - control_points[0][0][0]); // * size_y);
    repere.at(2) = normalize(control_points[0][0][1] - control_points[0][0][0]); // * size_z);
    repere_normal.at(0) = normalize(cross(repere.at(1), repere.at(2)));
    repere_normal.at(1) = normalize(cross(repere.at(0), repere.at(2)));
    repere_normal.at(2) = normalize(cross(repere.at(0), repere.at(1)));

    /*
    std::cout << "Repere vectors:" << std::endl;
    std::cout << "repere.at(0): " << repere.at(0) << std::endl;
    std::cout << "repere.at(1): " << repere.at(1) << std::endl;
    std::cout << "repere.at(2): " << repere.at(2) << std::endl;

    // Output repere_normal vectors
    std::cout << "Repere normal vectors:" << std::endl;
    std::cout << "repere_normal.at(0): " << repere_normal.at(0) << std::endl;
    std::cout << "repere_normal.at(1): " << repere_normal.at(1) << std::endl;
    std::cout << "repere_normal.at(2): " << repere_normal.at(2) << std::endl;
    */
}

void FreeFormDeform::createBoundingGrid(const std::vector<Point> &positions, int div)
{
    Point pmin(std::numeric_limits<float>::max(),
               std::numeric_limits<float>::max(),
               std::numeric_limits<float>::max());

    Point pmax(std::numeric_limits<float>::lowest(),
               std::numeric_limits<float>::lowest(),
               std::numeric_limits<float>::lowest());


    // Iterate through all vertices
    for (const auto &vertex : positions)
    {
        // Update min values
        pmin.x = std::min(pmin.x, vertex.x);
        pmin.y = std::min(pmin.y, vertex.y);
        pmin.z = std::min(pmin.z, vertex.z);

        // Update max values
        pmax.x = std::max(pmax.x, vertex.x);
        pmax.y = std::max(pmax.y, vertex.y);
        pmax.z = std::max(pmax.z, vertex.z);
    }
std::cout<<pmax<<std::endl;
std::cout<<pmin<<std::endl;

    Vector distance = (pmax - pmin) / (div);
    control_points.resize(div);
    for (int i = 0; i < div; ++i)
    {
        control_points[i].resize(div);
        for (int j = 0; j < div; ++j)
        {
            control_points[i][j].resize(div); // Resize the innermost vector
            for (int k = 0; k < div; ++k)
            {
                Point tmp = pmin + Point(i * distance.x, j * distance.y, k * distance.z);
                control_points[i][j][k] = tmp; // Store the point directly
                // std::cout << "one of our points is: " << tmp << std::endl;
            }
        }
    }
    // Or define repere prior to this and then use repere for position ?
    //  Determine S , T ,U
    // Should be good
    // SHould i switch betwween y and z ?
    repere.at(0) = normalize(control_points[1][0][0] - control_points[0][0][0]); //* size_x);
    repere.at(1) = normalize(control_points[0][1][0] - control_points[0][0][0]); // * size_y);
    repere.at(2) = normalize(control_points[0][0][1] - control_points[0][0][0]); // * size_z);
    repere_normal.at(0) = normalize(cross(repere.at(1), repere.at(2)));
    repere_normal.at(1) = normalize(cross(repere.at(0), repere.at(2)));
    repere_normal.at(2) = normalize(cross(repere.at(0), repere.at(1)));

    /*
    std::cout << "Repere vectors:" << std::endl;
    std::cout << "repere.at(0): " << repere.at(0) << std::endl;
    std::cout << "repere.at(1): " << repere.at(1) << std::endl;
    std::cout << "repere.at(2): " << repere.at(2) << std::endl;

    // Output repere_normal vectors
    std::cout << "Repere normal vectors:" << std::endl;
    std::cout << "repere_normal.at(0): " << repere_normal.at(0) << std::endl;
    std::cout << "repere_normal.at(1): " << repere_normal.at(1) << std::endl;
    std::cout << "repere_normal.at(2): " << repere_normal.at(2) << std::endl;
    */
}

// SHould be good

Vector FreeFormDeform::getPointInParall(const Point &point)
{
    Vector computed = point - control_points[0][0][0];
    float s = dot(repere_normal.at(0), computed) / dot(repere_normal.at(0), repere.at(0));
    float t = dot(repere_normal.at(1), computed) / dot(repere_normal.at(1), repere.at(1));
    float u = dot(repere_normal.at(2), computed) / dot(repere_normal.at(2), repere.at(2));
    // std::cout <<"COmputed" << std::endl;
    // std::cout << computed << std::endl;
    // std::cout << Vector(s, t, u) << std::endl;

    return Vector(s, t, u);
}
Point FreeFormDeform::warpPoint(const Point &positions)
{
    // std::cout<<"warping Point"<< positions<<std::endl;
    size_t x_size = control_points.size();
    size_t y_size = control_points[0].size();
    size_t z_size = control_points[0][0].size();
    Vector stu = getPointInParall(positions);
    // std::cout<<"stu"<< stu<<std::endl;

    Point q;
    for (int i = 0; i < x_size; i++)
    {
        float bern_u = compute_bernst_basis(x_size - 1, stu.x, i);
        for (int j = 0; j < y_size; j++)
        {
            float bern_v = compute_bernst_basis(y_size - 1, stu.y, j);

            for (int k = 0; k < z_size; k++)
            {
                float bern_w = compute_bernst_basis(z_size - 1, stu.z, k);
             q = q + (bern_u * bern_v * bern_w * control_points[i][j][k]);
             //Bern_w is paper mode
            }
        }
    }
    // std::cout<<"final"<< q<<std::endl;

    return q;
}

void FreeFormDeform::randomModif()
{
    // Random number generator setup
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<float> dist(0.01, 0.1);

    int counter = 0;
    // Iterate through the 3D vector and translate each point
    for (auto &layer : control_points)
    {
        for (auto &row : layer)
        {
            for (auto &point : row)
            {
                counter++;
                if (counter % 2 == 0)
                {
                    point.x += dist(engine);
                    // point.y += dist(engine);
                    point.z -= dist(engine);
                }
                else
                {
                    point.z += dist(engine);
                    point.x -= dist(engine);
                }
            }
            break;
        }
    }
}

void FreeFormDeform::modifPoint(const int& axis_x, const int& axis_y,const int& axis_z,const Vector& dir )
{
  control_points[axis_x][axis_y][axis_z]=  control_points[axis_x][axis_y][axis_z] +dir;  
}


// Self contained function to handle
void FreeFormDeform::outputGrid(std::vector<Point> &vertices, std::vector<unsigned> &ffd_grid_indices)
{
    vertices.clear();
    ffd_grid_indices.clear();
    for (const auto &slice : control_points)
    {
        for (const auto &row : slice)
        {
            for (const auto &point : row)
            {
                vertices.push_back(point);
            }
        }
    }

    // Populate line indices (for connecting points)
    unsigned slices = control_points.size();
    unsigned rows = control_points[0].size();
    unsigned cols = control_points[0][0].size();

    // Create lines between adjacent points
    for (int i = 0; i < slices; ++i)
    {
        for (int j = 0; j < rows; ++j)
        {
            for (int k = 0; k < cols; ++k)
            { // One point and the thing around
                if (k < cols - 1)
                {
                    ffd_grid_indices.push_back(i * rows * cols + j * cols + k);
                    ffd_grid_indices.push_back(i * rows * cols + j * cols + (k + 1));
                }
                if (j < rows - 1)
                {
                    ffd_grid_indices.push_back(i * rows * cols + j * cols + k);
                    ffd_grid_indices.push_back(i * rows * cols + (j + 1) * cols + k);
                }
                if (i < slices - 1)
                {
                    ffd_grid_indices.push_back(i * rows * cols + j * cols + k);
                    ffd_grid_indices.push_back((i + 1) * rows * cols + j * cols + k);
                }
            }
        }
    }
}

MeshDeform FreeFormDeform::warpMesh(const std::vector<Point> &positions, const std::vector<unsigned> &indices, const std::vector<Vector> &normals)
{
    std::cout << "warp" << std::endl;
    MeshDeform new_mesh;
    new_mesh.positions.reserve(positions.size());
    new_mesh.indices.reserve(indices.size());
    new_mesh.indices.clear();
    for (size_t counter = 0; counter < positions.size(); counter++)
    {
        new_mesh.positions.push_back(warpPoint(positions[counter]));
        // std::cout<< new_mesh.positions[counter]<<std::endl;
        // std::cout<< positions[counter]<<std::endl;
        // std::cout<<std::endl;
    }

    std::copy(indices.begin(), indices.end(),
              std::back_inserter(new_mesh.indices));
    return new_mesh;
}

// Test other alpha formula
Point TwistDeform::twistPoint(const Point &point, int axis, float T)
{
    Point return_point;
    float alpha = 360*point(axis) / T;
    //float alpha = cos(2*M_PI/ T)* point(axis);
    switch (axis)
    {
    case (0):
        return_point = RotationX(alpha)(point);
        break;
    case (1):
        return_point = RotationY(alpha)(point);
        break;
    case (2):
        return_point = RotationZ(alpha)(point);
        break;
    default:
        return_point = RotationZ(alpha)(point);

        break;
    }

    return return_point;
}; // Tesst with the professor formula. Substraction on the y coordinate

Vector TwistDeform::twistNormalZ(const Point &point, const Vector &normal, float T)
{
    float alpha = point.z * ((2 * 3.14) / T);
    float alpha_deriv = ((2 * 3.14) / T);

    float sin_t = sin(alpha);
    float cos_t = cos(alpha);

    return Transform(
        cos_t, -sin_t, 0, 0,
        sin_t, cos_t, 0, 0,
        point.y * alpha_deriv, -point.x * alpha_deriv, 0, 0,
        0, 0, 0, 0)(normal);
};

MeshDeform TwistDeform::twistMesh(const std::vector<Point> &positions, const std::vector<unsigned> &indices, const std::vector<Vector> &normals, int axis, float period)
{

    MeshDeform new_mesh;
    new_mesh.positions.reserve(positions.size());
    new_mesh.normals.resize(normals.size());
    new_mesh.indices.reserve(indices.size());

    for (size_t counter = 0; counter < positions.size(); counter++)
    {
        new_mesh.positions.push_back(twistPoint(positions[counter], axis, period));
    }
    for (size_t counter = 0; counter < normals.size(); counter++)
    {
        new_mesh.normals[counter] = twistNormalZ(positions[counter], normals[counter], period);
    }

    std::copy(indices.begin(), indices.end(),
              std::back_inserter(new_mesh.indices));
    return new_mesh;
}

namespace LocalDeformations
{

    // This namespace function simply handle translation
    namespace onSphere
    {

        float attenuation(const Point &pt, const Point &center, float radius)
        {
            Vector pc = pt - center;
            float d = length(pc);
            if (d < radius)
            {
                // 1 - d / radius;
                // (1 - d * d) * (1 - d * d)
                return (1 - d * d);
            }
            else
            {
                return 0;
            }
        }

        Point translatePoint(const Point &pt, const Vector &translation, const Point &center, float radius)
        {
            return pt + translation * attenuation(pt, center, radius);
        }

        MeshDeform warpMesh(const std::vector<Point> &positions, const std::vector<unsigned> &indices, const std::vector<Vector> &normals,
                            const Vector &translation, const Point &center, float radius)
        {

            MeshDeform new_mesh;
            new_mesh.positions.reserve(positions.size());
            new_mesh.normals.reserve(normals.size());
            new_mesh.indices.reserve(indices.size());

            for (size_t counter = 0; counter < positions.size(); counter++)
            {
                new_mesh.positions.push_back(translatePoint(positions[counter], translation, center, radius));
            }
            std::copy(normals.begin(), normals.end(),
                      std::back_inserter(new_mesh.normals));

            std::copy(indices.begin(), indices.end(),
                      std::back_inserter(new_mesh.indices));
            return new_mesh;
        }
    }
}

bool writeMeshDeform(const char *filename, const MeshDeform &deformed)
{

    // Create and open a file
    std::ofstream mesh_file(filename);

    if (!mesh_file)
    {
        std::cerr << "Error opening file!" << std::endl;
        return 0;
    }

    // Write vertices
    for (const auto &vert : deformed.positions)
    {
        mesh_file << "v " << vert.x << " " << vert.y << " " << vert.z << "\n";
        if (!mesh_file) return false;  // Check for write success
    }

    // Write normals if present
    if (deformed.normals.size() > 0)
    {
        for (const auto &normal : deformed.normals)
        {
            mesh_file << "vn " << normal.x << " " << normal.y << " " << normal.z << "\n";
            if (!mesh_file) return false;  // Check for write success
        }
    }

    // Write faces
    //TODO BEtter usage of the operator
    int num_faces = deformed.indices.size();
    for (int i = 0; i < num_faces; i += 3)
    {
        if (i + 2 < num_faces)
        {
            if (deformed.normals.size() > 0)
            {
                mesh_file << "f " << deformed.indices[i]+1 << " "
                          << deformed.indices[i + 1]+1 << " "
                          << deformed.indices[i + 2]+1 << "\n";
            }
            else
            {
                mesh_file << "f " << deformed.indices[i]+1 << "//" << deformed.indices[i]+1 << " "
                          << deformed.indices[i + 1]+1 << "//" << deformed.indices[i + 1]+1 << " "
                          << deformed.indices[i + 2]+1 << "//" << deformed.indices[i + 2]+1 << "\n";
            }
            if (!mesh_file) return false;  // Check for write success
        }
    }
    std::cout << "Data written!" << std::endl;

    return true;
};
