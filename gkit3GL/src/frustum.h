#pragma once
#include <math.h>
#include <array>
#include "vec.h"
#include "mat.h"

// Struct Frustum_Plane

struct Region
{

    std::array<Point, 8> corners;

//    std::vector<short> element_a_count;
//   std::vector<short> element_a_start;

    Region(const Point &corn_max, const Point &corn_min)
    {
        corners.at(0) = Point(corn_min.x, corn_min.y, corn_min.z); // Min corner
        corners.at(1) = Point(corn_max.x, corn_min.y, corn_min.z); // Min-X Max-Y Min-Z
        corners.at(2) = Point(corn_min.x, corn_max.y, corn_min.z); // Max-Y Min-X Min-Z
        corners.at(3) = Point(corn_max.x, corn_max.y, corn_min.z); // Max corner on XY plane

        corners.at(4) = Point(corn_min.x, corn_min.y, corn_max.z); // Min-X Min-Y Max-Z
        corners.at(5) = Point(corn_max.x, corn_min.y, corn_max.z); // Min-Y Max-X Max-Z
        corners.at(6) = Point(corn_min.x, corn_max.y, corn_max.z); // Max-Y Min-X Max-Z
        corners.at(7) = Point(corn_max.x, corn_max.y, corn_max.z); // Max corner
    };
};

struct Plane
{
    Vector normal;
    float distance;

    Plane() = default;

    Plane(const vec3 &normal, float distance)
        : normal(normal), distance(distance) {}
};


struct Frustrum
{

    std::array<Plane, 6> planes;

    Frustrum(const Plane &top, const Plane &bottom, const Plane &right, const Plane &left,
             const Plane &far, const Plane &near)
    {

        planes.at(0) = far;
        planes.at(1) = left;
        planes.at(2) = right;
        planes.at(3) = top;
        planes.at(4) = bottom;
        planes.at(5) = near;
    }
};

Frustrum createFrustumplane(Transform projection);
bool insideFrustum(Region &regions, const Transform &mvp);

Frustrum createFrustumplane(Transform projection)
{
    Plane top, bottom, right, left, far, near;

    left.normal.x = projection[0].z + projection[0].x;
    left.normal.y = projection[1].z + projection[1].x;
    left.normal.z = projection[2].z + projection[2].x;
    left.distance = projection[3].z + projection[3].x;
    far.normal = normalize(far.normal);

    // Right plane: M3 - M0
    right.normal.x = projection[0].z - projection[0].x;
    right.normal.y = projection[1].z - projection[1].x;
    right.normal.z = projection[2].z - projection[2].x;
    right.distance = projection[3].z - projection[3].x;
    right.normal = normalize(right.normal);

    // Bottom plane: M3 + M1
    bottom.normal.x = projection[0].z + projection[0].y;
    bottom.normal.y = projection[1].z + projection[1].y;
    bottom.normal.z = projection[2].z + projection[2].y;
    bottom.distance = projection[3].z + projection[3].y;
    bottom.normal = normalize(bottom.normal);

    // Top plane: M3 - M1
    top.normal.x = projection[0].z - projection[0].y;
    top.normal.y = projection[1].z - projection[1].y;
    top.normal.z = projection[2].z - projection[2].y;
    top.distance = projection[3].z - projection[3].y;
    top.normal = normalize(top.normal);

    // Near plane: M3 + M2
    near.normal.x = projection[0].z + projection[0].y;
    near.normal.y = projection[1].z + projection[1].y;
    near.normal.z = projection[2].z + projection[2].y;
    near.distance = projection[3].z + projection[3].y;
    near.normal = normalize(near.normal);

    // Far plane: M3 - M2
    far.normal.x = projection[0].z - projection[0].y;
    far.normal.y = projection[1].z - projection[1].y;
    far.normal.z = projection[2].z - projection[2].y;
    far.distance = projection[3].z - projection[3].y;

    far.normal = normalize(far.normal);

    return Frustrum(top, bottom, right, left, far, near);
}


bool insideFrustum(Region &regions, const Transform &mvp)
{
    // Check if any corner is inside the frustum
    for (unsigned int j = 0; j < regions.corners.size(); j++)
    {
        vec4 curr_corner = vec4(regions.corners.at(j)); 
        vec4 corner_clip_space = mvp(curr_corner); // Apply MVP matrix

        // Check if the corner is inside all six frustum planes
        bool is_inside = (corner_clip_space.x >= -corner_clip_space.w && corner_clip_space.x <= corner_clip_space.w) && // Left & Right
                         (corner_clip_space.y >= -corner_clip_space.w && corner_clip_space.y <= corner_clip_space.w) && // Top & Bottom
                         (corner_clip_space.z >= -corner_clip_space.w && corner_clip_space.z <= corner_clip_space.w);   // Near & Far

        if (is_inside) {
            // Early exit 
            return true;
        }
    }
    return false;
}
