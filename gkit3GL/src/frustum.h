#pragma once
#include <math.h>
#include <array>
#include "vec.h"
#include "mat.h"

// Struct Frustum_Plane

struct Region
{

    std::array<Point, 8> corners;

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

/*
bool insideFrustum(Region &regions, const Frustum &frustum)
{
    // Assume Frustum has six planes, with each plane as vec4 (a, b, c, d) for ax + by + cz + d = 0
    for (const auto& plane : frustum.planes) {
        int outside_count = 0;

        // Count corners outside this plane
        for (const auto& corner : regions.corners) {
            vec4 point(corner, 1.0f);
            if (dot(plane, point) < 0) {
                outside_count++;
            }
        }

        // If all corners are outside one plane, region is fully outside the frustum
        if (outside_count == regions.corners.size()) {
            return false;
        }
    }

    // If not fully outside any plane, region intersects or is inside the frustum
    return true;
}
*/

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

/*
bool insideFrustum(Region &regions, const Frustrum &frust)
{
    // check box outside/inside of frustum
    for (int i = 0; i < 6; i++)
    {

        int counter = 0;

        for (int j = 0; j < regions.corners.size(); j++)
        {
            counter += ((dot(frust.planes.at(i).normal, regions.corners.at(j)) < 0.0)) ? 1 : 0;
        }

        if (counter == 8)
            return false;
    }
}*/

bool insideFrustum(Region &regions, const Transform &mvp)
{
    // Check if any corner is inside the frustum
    for (int j = 0; j < regions.corners.size(); j++)
    {
        vec4 curr_corner = vec4(regions.corners.at(j)); 
        vec4 corner_clip_space = mvp(curr_corner); // Apply MVP matrix

        // Check if the corner is inside all six frustum planes
        bool is_inside = (corner_clip_space.x >= -corner_clip_space.w && corner_clip_space.x <= corner_clip_space.w) && // Left & Right
                         (corner_clip_space.y >= -corner_clip_space.w && corner_clip_space.y <= corner_clip_space.w) && // Top & Bottom
                         (corner_clip_space.z >= -corner_clip_space.w*2 && corner_clip_space.z <= corner_clip_space.w);   // Near & Far

        if (is_inside) {
            // Early exit 
            return true;
        }
    }
    return false;
}
