#pragma once
#include "implicits.h"

constexpr float eps = 0.001f;
constexpr int tmax = 1000;

class ImplicitSurface
{
public:
    virtual float evaluate(const Vector &point) const = 0;
    virtual bool inside(const Vector &point) const;
    inline virtual Vector derivateapprox(const Vector &point) const;
    virtual Vector gradient(const Vector &point) const;
    virtual Vector normal(const Vector &point) const;
    // virtual float Volume(const Vector &gradient) const;

    // From TinyMesh
    Vector Dichotomy(Vector, Vector, double, double, double, const double & = 1.0e-4) const;
    void Polygonize(int, Mesh_imp &, const Box &, const double & = 1e-4) const;

    virtual bool itrsctRayMarch(const Vector &o, const Vector &d, Vector &pt) const
    {
        pt = Vector(o);
        for (int i = 0; i < 5000; i++)
        {
            float dist = evaluate(pt);
            if (dist < 0)
            {
                return true;
            }
            pt = pt + d * eps;
        }
        return false;
    }

    virtual bool itrsctSphereTrace(const Vector &o, const Vector &d, Vector &pt_cal) const
    {
        pt_cal = Vector(o);
        for (int i = 0; i < 100; i++)
        {
            float dist = evaluate(pt_cal);
            if ((dist) < 0)
            {
                return true;
            }
            pt_cal = pt_cal + d * std::max(eps, dist); // Lipchitz is 1
        }
        return false;
    }

    virtual ~ImplicitSurface() = default;
};

// TREEEE
class Structure_Tree : public ImplicitSurface
{
public:
    ImplicitSurface *root;
    Box Bbox;

    Structure_Tree(ImplicitSurface *init_root) : root(init_root)
    {
        Bbox = Box(Vector(8, 8, 8), Vector(-8, -8, -8));
        std::cout << "Root" << std::endl;
    }

    Structure_Tree(ImplicitSurface *init_root, const Box &box) : root(init_root)
    {
        Bbox = box;
        std::cout << "Root" << std::endl;
    }
    float evaluate(const Vector &pt) const override
    {
        return root->evaluate(pt);
    }

    bool inside(const Vector &pt) const override
    {
        return root->evaluate(pt);
    }
};

// Most functions are pulled straight from there
//  https://iquilezles.org/articles/distfunctions/

class Sphere : public ImplicitSurface
{
    float m_radius;
    Vector m_center;

public:
    Sphere(float radius, Vector center) : m_radius(radius), m_center(center) {};

    float evaluate(const Vector &pt) const override
    {
        return length(pt - m_center) - m_radius;
    }
};

// Utilitaires
Vector max_vec(const Vector &a, const Vector &b);
Vector min_vec(const Vector &a, const Vector &b);

// Need to translate
class Boxsdf : public ImplicitSurface
{
    float m_length;
    Vector m_center;

public:
    Boxsdf(const float &l, const Vector &center) : m_length(l), m_center(center) {};

    float evaluate(const Vector &pt) const override
    {
        Vector dir = Vector(std::abs(pt.x - m_center.x), std::abs(pt.y - m_center.y), std::abs(pt.z - m_center.z)) -
                     (Vector(m_length, m_length, m_length) / 2); // Distance to the boundary from the center
        Vector null;
        float az = length(max_vec(dir, null)) + std::min(std::max(dir.x, std::max(dir.y, dir.z)), 0.0f);
        return az;
    }
};

class Ellipsoid : public ImplicitSurface
{
    float m_radius;
    Vector m_center;
    // Size affect the shape up to having a "box"
public:
    Ellipsoid(float radius, const Vector &center) : m_radius(radius), m_center(center) {};

    float evaluate(const Vector &pt) const override
    {
        Vector ptc = (pt - m_center) / m_radius;
        float scaled_dist = length(ptc);
        float scaled_dist_square = length(ptc / m_radius);
        return scaled_dist * (scaled_dist - 1.0) / scaled_dist_square;
    }
};
inline float clamp(const float x, const float min, const float max)
{
    if (x < min)
        return min;
    else if (x > max)
        return max;
    else
        return x;
};
// A dot product with the addition exchange for a substraction
inline float ndot(const Vector &a, const Vector &b) { return a.x * b.x - a.y * b.y; }
inline Vector abs_vec(const Vector &pt) { return Vector(std::abs(pt.x), std::abs(pt.y), std::abs(pt.z)); }

class Rhombus : public ImplicitSurface
{
    float m_lenght_a, m_lenght_b, m_height, m_ra;
    // Stretch x axis, stretch y axis, stretch zaxis ,Rounding
    Vector m_center;

public:
    Rhombus(float la, float lb, float h, float ra, const Vector &center) : m_lenght_a(la), m_lenght_b(lb), m_height(h), m_ra(ra), m_center(center) {};

    float evaluate(const Vector &pt) const override
    {

        Vector ptc = abs_vec(pt);
        Vector b_val(m_lenght_a, m_lenght_b, 0.0);
        float f_val = clamp((ndot(b_val, b_val - 2.0f * Vector(ptc.x, ptc.z, 0.0))) /
                                dot(b_val, b_val),
                            -1.0, 1.0);
        //*sign(p.x*b.y+p.z*b.x-b.x*b.y)-ra, p.y-h); YOu can get stable thing by removing sign/everything then -h
        // Waarepdn rhombus, four cone or four sphere hollow rhombus *sign(p.x*b.y+p.z)-ra

        float param_val = (ptc.x * b_val.y + ptc.z * b_val.x - b_val.x * b_val.y);
        Vector q_val = Vector(length(Vector(ptc.x, ptc.z, 0.0f) - 0.5 * b_val * Vector(1.0 - f_val, 1.0 + f_val, 0.0f)) * param_val - m_ra,
                              ptc.y - m_height, 0.0);

        return std::min(std::max(q_val.x, q_val.y), 0.0f) + length(max_vec(q_val, Vector()));
    }
};

class Rhombus_sign : public ImplicitSurface
{
    float m_lenght_a, m_lenght_b, m_height, m_ra;
    // Stretch x axis, stretch y axis, stretch zaxis ,Rounding
    Vector m_center;

public:
    Rhombus_sign(float la, float lb, float h, float ra, const Vector &center) : m_lenght_a(la), m_lenght_b(lb), m_height(h), m_ra(ra), m_center(center) {};

    float evaluate(const Vector &pt) const override
    {

        Vector ptc = abs_vec(pt);
        Vector b_val(m_lenght_a, m_lenght_b, 0.0);
        float f_val = clamp((ndot(b_val, b_val - 2.0f * Vector(ptc.x, ptc.z, 0.0))) /
                                dot(b_val, b_val),
                            -1.0, 1.0);
        //*sign(p.x*b.y+p.z*b.x-b.x*b.y)-ra, p.y-h); YOu can get stable thing by removing sign/everything then -h
        // Waarepdn rhombus, four cone or four sphere hollow rhombus *sign(p.x*b.y+p.z)-ra

        float param_val = (ptc.x * b_val.y + ptc.z);
        param_val = (param_val) >= 0 ? ((param_val > 0) ? 1 : 0) : -1;
        Vector q_val = Vector(length(Vector(ptc.x, ptc.z, 0.0f) - 0.5 * b_val * Vector(1.0 - f_val, 1.0 + f_val, 0.0f)) * param_val - m_ra,
                              ptc.y - m_height, 0.0);

        return std::min(std::max(q_val.x, q_val.y), 0.0f) + length(max_vec(q_val, Vector()));
    }
};

class Capsule : public ImplicitSurface
{
    float m_radius;
    Vector m_pta;
    Vector m_ptb;

public:
    Capsule(float m_radius, const Vector &pta, const Vector &ptb) : m_radius(m_radius), m_pta(pta), m_ptb(ptb) {};

    float evaluate(const Vector &pt) const override
    {
        Vector pt_ap = pt - m_pta;
        Vector pt_ab = m_ptb - m_pta;
        float h = clamp(dot(pt_ap, pt_ab) / dot(pt_ab, pt_ab), 0.0, 1.0);
        return length(pt_ap - pt_ab * h) - m_radius;
    }
};

class Cone : public ImplicitSurface
{
    float m_height;
    float m_radius;

    Vector m_center;

public:
    Cone(float height, float radius, Vector center) : m_height(height), m_radius(radius), m_center(center) {};

    float evaluate(const Vector &pt) const override
    {
        Vector ptc = pt - (m_center);
        float q = length(Vector(ptc.x, ptc.z, 0.0));
        return std::max(dot(Vector(m_radius, 0.2, 0.0), Vector(q, ptc.y, 0.0)),
                        -m_height - ptc.y);
    }
    /*
    Carpet
     float evaluate(const Vector &pt) const override
    {
        float q = length(Vector(pt.x, pt.z, 0.0));
        return std::max(dot(Vector(pt.x, pt.y, 0.0), Vector(q,pt.y, 0.0)),
                        -m_height - pt.y);
    }

    */
};

class Octo : public ImplicitSurface
{
    float m_s;

public:
    Octo(float s) : m_s(s) {};

    float evaluate(const Vector &pt) const override
    {

        Vector p = abs_vec(pt);
        return (p.x + p.y + p.z - m_s) * 0.57735027;
    }
};

class Torus : public ImplicitSurface
{
    float radius;
    float R_rad;
    Vector m_center;

public:
    Torus(float r, float R) : radius(r), R_rad(R) {};

    float evaluate(const Vector &pt) const override
    {
        Vector q = Vector(length(Vector(pt.x, pt.z, 0.0)) - R_rad, pt.y, 0.0);
        return length(q) - radius;
    }
};

// Puttin up some simple transformations
class Transformations : public ImplicitSurface
{
protected:
    ImplicitSurface *m_original;

public:
    Transformations(ImplicitSurface *original)
        : m_original(original) {};

    virtual float evaluate(const Vector &point) const = 0;
    virtual ~Transformations() = default;
};
// Only simple one that don't warp space
class Rotation_trs : public Transformations
{
    // Transform rotation;
    Transform inv_rotation;

public:
    Rotation_trs(ImplicitSurface *original, int angle, int axis)
        : Transformations(original)
    {
        switch (axis)
        {
        case 0:
        {
            // rotation = RotationX(angle);
            inv_rotation = Inverse(RotationX(angle));

            break;
        }
        case 1:
        {
            // rotation = RotationY(angle);
            inv_rotation = Inverse(RotationY(angle));

            break;
        }
        case 2:
        {
            // rotation = RotationZ(angle);
            inv_rotation = Inverse(RotationZ(angle));

            break;
        }
        default:
            // rotation = RotationX(angle);
            inv_rotation = Inverse(RotationX(angle));
            break;
        }
    };

    float evaluate(const Vector &pt) const override
    {
        return (m_original->evaluate(inv_rotation(pt)));
    }
};
class TranslationSdf : public Transformations
{
    Vector m_dir;

public:
    TranslationSdf(ImplicitSurface *original, const Vector &dir)
        : Transformations(original), m_dir(dir) {};

    float evaluate(const Vector &pt) const override
    {
        return (m_original->evaluate(pt - m_dir /* *inverse(Rotation(mattrix)) */));
    }
};

class Symmetry : public Transformations
{
    int axis;

public:
    Symmetry(ImplicitSurface *original, const int &axs)
        : Transformations(original), axis(axs)
    {
    }

    float evaluate(const Vector &pt) const override
    {
        Vector p = pt;
        p(axis) = abs(pt(axis));
        return (m_original->evaluate(p));
    }
};
class Onion_Sdf : public Transformations
{
    float m_thickness;

public:
    Onion_Sdf(ImplicitSurface *original, float thickness)
        : Transformations(original), m_thickness(thickness)
    {
    }

    float evaluate(const Vector &pt) const override
    {

        return abs(m_original->evaluate(pt)) - m_thickness;
    }
};

class Displacement_sdf : public Transformations
{
    unsigned axis_x, axis_y, axis_z;

public:
    Displacement_sdf(ImplicitSurface *original)
        : Transformations(original)
    {
    }

    float evaluate(const Vector &pt) const override
    {
        float x = sin(1 * pt.x);
        float y = sin(17 * pt.y);
        float z = sin(2 * pt.z);

        float dis = y;
        // float dis = sin(5*pt.y); Soft simmetry
        return (m_original->evaluate(pt) + dis);
    }
};

constexpr float default_bend = 10.0;

class Bend : public Transformations
{
    float bendtensity;
    int m_axis;

public:
    Bend(ImplicitSurface *original, const float &bend = default_bend, const int &axis = 0)
        : Transformations(original), bendtensity(bend), m_axis(axis)
    {
    }

    float evaluate(const Vector &pt) const override
    {
        Vector p = pt;
        float s = sin(bendtensity * p(m_axis));
        float c = cos(bendtensity * p(m_axis));
        float bend_x = c * p.x - s * p.y;
        float bend_y = s * p.x + c * p.y;
        p.x = bend_x;
        p.y = bend_y;

        return (m_original->evaluate(p));
    }
};

class CSGNode : public ImplicitSurface
{
protected:
    ImplicitSurface *left;
    ImplicitSurface *right;

public:
    CSGNode(ImplicitSurface *l, ImplicitSurface *r)
        : left(l), right(r) {};

    virtual float evaluate(const Vector &point) const = 0;
    inline virtual float gOffsetFunc(const float &a, const float &b) const
    {
        float r = (b - a); // r can be whatver TODO What is r
        float h = std::max(r - std::abs(a - b), 0.0f) / r;
        return h * h * h * r * (1.0f / 6.0f);
    };
    inline virtual float rvachev(const float &a, const float &b) const
    {
        return std::sqrt(a * a + b * b);
    };

    virtual Vector gradient(const Vector &point) const override
    {
        // Determine which side is closer to compute the gradient.
        float leftValue = left->inside(point);
        float rightValue = right->inside(point);

        if (leftValue < rightValue)
        {
            return left->gradient(point);
        }
        else
        {
            return right->gradient(point);
        }
    }
    virtual Vector normal(const Vector &point) const override
    {
        float leftValue = left->inside(point);
        float rightValue = right->inside(point);

        if (leftValue < rightValue)
        {
            return left->normal(point);
        }
        else
        {
            return right->normal(point);
        }
    };
    // virtual float Volume(const Vector &gradient) const;
    virtual ~CSGNode() = default;
};

class Union : public CSGNode
{

public:
    Union(ImplicitSurface *l, ImplicitSurface *r)
        : CSGNode(l, r) {};

    float evaluate(const Vector &pt) const override
    {
        return std::min(left->evaluate(pt), right->evaluate(pt));
    }
};

class Intersection : public CSGNode
{
public:
    Intersection(ImplicitSurface *l, ImplicitSurface *r)
        : CSGNode(l, r) {};

    float evaluate(const Vector &pt) const override
    {
        return std::max(left->evaluate(pt), right->evaluate(pt));
    }
};

class Difference : public CSGNode
{
public:
    Difference(ImplicitSurface *l, ImplicitSurface *r)
        : CSGNode(l, r) {};

    float evaluate(const Vector &pt) const override
    {
        return std::max(left->evaluate(pt), -right->evaluate(pt));
    }
};

class SmoothUnion : public CSGNode
{
public:
    SmoothUnion(ImplicitSurface *l, ImplicitSurface *r)
        : CSGNode(l, r) {};
    float evaluate(const Vector &pt) const override
    {
        float a = left->evaluate(pt);
        float b = right->evaluate(pt);
        return std::min(a, b) -
               gOffsetFunc(a, b);
    }
};

class Xor : public CSGNode
{
public:
    Xor(ImplicitSurface *l, ImplicitSurface *r)
        : CSGNode(l, r) {};
    float evaluate(const Vector &pt) const override
    {
        float a = left->evaluate(pt);
        float b = right->evaluate(pt);
        return std::max(std::min(a, b), std::min(-a, -b));
    }
};

class SmoothIntersection : public CSGNode
{
public:
    SmoothIntersection(ImplicitSurface *l, ImplicitSurface *r)
        : CSGNode(l, r) {};
    float evaluate(const Vector &pt) const override
    {
        float a = left->evaluate(pt);
        float b = right->evaluate(pt);
        return std::max(a, b) -
               gOffsetFunc(a, b);
    }
};

class SmoothDifference : public CSGNode
{
public:
    SmoothDifference(ImplicitSurface *l, ImplicitSurface *r)
        : CSGNode(l, r) {};
    float evaluate(const Vector &pt) const override
    {
        float a = left->evaluate(pt);
        float b = right->evaluate(pt);
        return std::max(a, -b) -
               gOffsetFunc(a, b);
    }
};

class UnionRvachev : public CSGNode
{
public:
    UnionRvachev(ImplicitSurface *l, ImplicitSurface *r)
        : CSGNode(l, r) {};
    float evaluate(const Vector &pt) const override
    {
        float a = left->evaluate(pt);
        float b = right->evaluate(pt);

        return a + b -
               rvachev(a, b);
    }
};

class IntersectionRvachev : public CSGNode
{
public:
    IntersectionRvachev(ImplicitSurface *l, ImplicitSurface *r)
        : CSGNode(l, r) {};
    float evaluate(const Vector &pt) const override
    {
        float a = left->evaluate(pt);
        float b = right->evaluate(pt);

        return a + b +
               rvachev(a, b);
    }
};

class DifferenceRvachev : public CSGNode
{
public:
    DifferenceRvachev(ImplicitSurface *l, ImplicitSurface *r)
        : CSGNode(l, r) {};
    float evaluate(const Vector &pt) const override
    {
        float a = left->evaluate(pt);
        float b = right->evaluate(pt);

        return a - b -
               rvachev(a, -b);
    }
};

#include <array>
class ErodedSdf : public Transformations
{
    Vector m_origin;
    float sphere_max;
    float sphere_min;
    static constexpr int num_dir = 10;
    std::array<Vector, num_dir> directions;

public:
    ErodedSdf(ImplicitSurface *original, const Vector &ori)
        : Transformations(original), m_origin(ori)
    {
        for (int i = 0; i < num_dir; ++i)
        {
            float theta = (float)i / num_dir * M_PI;                                          // Angle in radians
            float phi = (float)i / num_dir * 2 * M_PI;                                        // Angle in radians
            directions[i] = Vector(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta)); // Convert to Cartesian
        }
    };

    float evaluate(const Vector &pt) const override
    {
        // Intersect

        Vector pt_cal = pt;

        ImplicitSurface *hole = m_original;

        for (int i = 0; i < num_dir; ++i)
        {
            if (m_original->itrsctSphereTrace(m_origin, directions.at(i), pt_cal))
            {
                hole = new SmoothDifference(hole, new Sphere(0.1f, pt_cal));
            }
        }
        return hole->evaluate(pt);
    }
};

constexpr int edgeTable[256] = {
    0, 273, 545, 816, 1042, 1283, 1587, 1826, 2082, 2355, 2563, 2834, 3120, 3361, 3601, 3840,
    324, 85, 869, 628, 1366, 1095, 1911, 1638, 2406, 2167, 2887, 2646, 3444, 3173, 3925, 3652,
    644, 917, 165, 436, 1686, 1927, 1207, 1446, 2726, 2999, 2183, 2454, 3764, 4005, 3221, 3460,
    960, 721, 481, 240, 2002, 1731, 1523, 1250, 3042, 2803, 2499, 2258, 4080, 3809, 3537, 3264,
    1096, 1369, 1641, 1912, 90, 331, 635, 874, 3178, 3451, 3659, 3930, 2168, 2409, 2649, 2888,
    1292, 1053, 1837, 1596, 286, 15, 831, 558, 3374, 3135, 3855, 3614, 2364, 2093, 2845, 2572,
    1740, 2013, 1261, 1532, 734, 975, 255, 494, 3822, 4095, 3279, 3550, 2812, 3053, 2269, 2508,
    1928, 1689, 1449, 1208, 922, 651, 443, 170, 4010, 3771, 3467, 3226, 3000, 2729, 2457, 2184,
    2184, 2457, 2729, 3000, 3226, 3467, 3771, 4010, 170, 443, 651, 922, 1208, 1449, 1689, 1928,
    2508, 2269, 3053, 2812, 3550, 3279, 4095, 3822, 494, 255, 975, 734, 1532, 1261, 2013, 1740,
    2572, 2845, 2093, 2364, 3614, 3855, 3135, 3374, 558, 831, 15, 286, 1596, 1837, 1053, 1292,
    2888, 2649, 2409, 2168, 3930, 3659, 3451, 3178, 874, 635, 331, 90, 1912, 1641, 1369, 1096,
    3264, 3537, 3809, 4080, 2258, 2499, 2803, 3042, 1250, 1523, 1731, 2002, 240, 481, 721, 960,
    3460, 3221, 4005, 3764, 2454, 2183, 2999, 2726, 1446, 1207, 1927, 1686, 436, 165, 917, 644,
    3652, 3925, 3173, 3444, 2646, 2887, 2167, 2406, 1638, 1911, 1095, 1366, 628, 869, 85, 324,
    3840, 3601, 3361, 3120, 2834, 2563, 2355, 2082, 1826, 1587, 1283, 1042, 816, 545, 273, 0};

constexpr int TriangleTable[256][16] = {
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {5, 8, 4, 9, 8, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 10, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 10, 1, 8, 10, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {5, 9, 0, 1, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 1, 5, 9, 10, 9, 8, 10, -1, -1, -1, -1, -1, -1, -1},
    {5, 1, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 4, 5, 1, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 1, 11, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 8, 4, 1, 11, 8, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
    {4, 11, 5, 10, 11, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 11, 5, 0, 8, 11, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
    {4, 9, 0, 4, 10, 9, 10, 11, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 8, 11, 11, 8, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 4, 0, 6, 4, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 9, 8, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 5, 9, 2, 6, 5, 6, 4, 5, -1, -1, -1, -1, -1, -1, -1},
    {8, 2, 6, 4, 10, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 2, 6, 10, 1, 2, 1, 0, 2, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 5, 8, 2, 6, 1, 4, 10, -1, -1, -1, -1, -1, -1, -1},
    {2, 6, 10, 9, 2, 10, 9, 10, 1, 9, 1, 5, -1, -1, -1, -1},
    {5, 1, 11, 8, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 2, 6, 4, 0, 2, 5, 1, 11, -1, -1, -1, -1, -1, -1, -1},
    {9, 1, 11, 9, 0, 1, 8, 2, 6, -1, -1, -1, -1, -1, -1, -1},
    {1, 11, 9, 1, 9, 6, 1, 6, 4, 6, 9, 2, -1, -1, -1, -1},
    {4, 11, 5, 4, 10, 11, 6, 8, 2, -1, -1, -1, -1, -1, -1, -1},
    {5, 10, 11, 5, 2, 10, 5, 0, 2, 6, 10, 2, -1, -1, -1, -1},
    {2, 6, 8, 9, 0, 10, 9, 10, 11, 10, 0, 4, -1, -1, -1, -1},
    {2, 6, 10, 2, 10, 9, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 2, 0, 8, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 7, 2, 5, 7, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 7, 2, 8, 4, 7, 4, 5, 7, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 2, 1, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 10, 1, 0, 8, 10, 2, 9, 7, -1, -1, -1, -1, -1, -1, -1},
    {0, 7, 2, 0, 5, 7, 1, 4, 10, -1, -1, -1, -1, -1, -1, -1},
    {1, 5, 7, 1, 7, 8, 1, 8, 10, 2, 8, 7, -1, -1, -1, -1},
    {5, 1, 11, 9, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 0, 8, 5, 1, 11, 2, 9, 7, -1, -1, -1, -1, -1, -1, -1},
    {7, 1, 11, 7, 2, 1, 2, 0, 1, -1, -1, -1, -1, -1, -1, -1},
    {1, 11, 7, 4, 1, 7, 4, 7, 2, 4, 2, 8, -1, -1, -1, -1},
    {11, 4, 10, 11, 5, 4, 9, 7, 2, -1, -1, -1, -1, -1, -1, -1},
    {2, 9, 7, 0, 8, 5, 8, 11, 5, 8, 10, 11, -1, -1, -1, -1},
    {7, 2, 0, 7, 0, 10, 7, 10, 11, 10, 0, 4, -1, -1, -1, -1},
    {7, 2, 8, 7, 8, 11, 11, 8, 10, -1, -1, -1, -1, -1, -1, -1},
    {9, 6, 8, 7, 6, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 4, 0, 9, 7, 4, 7, 6, 4, -1, -1, -1, -1, -1, -1, -1},
    {0, 6, 8, 0, 5, 6, 5, 7, 6, -1, -1, -1, -1, -1, -1, -1},
    {5, 7, 4, 4, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {6, 9, 7, 6, 8, 9, 4, 10, 1, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 6, 9, 6, 1, 9, 1, 0, 1, 6, 10, -1, -1, -1, -1},
    {1, 4, 10, 0, 5, 8, 5, 6, 8, 5, 7, 6, -1, -1, -1, -1},
    {10, 1, 5, 10, 5, 6, 6, 5, 7, -1, -1, -1, -1, -1, -1, -1},
    {9, 6, 8, 9, 7, 6, 11, 5, 1, -1, -1, -1, -1, -1, -1, -1},
    {11, 5, 1, 9, 7, 0, 7, 4, 0, 7, 6, 4, -1, -1, -1, -1},
    {8, 0, 1, 8, 1, 7, 8, 7, 6, 11, 7, 1, -1, -1, -1, -1},
    {1, 11, 7, 1, 7, 4, 4, 7, 6, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 8, 8, 7, 6, 11, 5, 4, 11, 4, 10, -1, -1, -1, -1},
    {7, 6, 0, 7, 0, 9, 6, 10, 0, 5, 0, 11, 10, 11, 0, -1},
    {10, 11, 0, 10, 0, 4, 11, 7, 0, 8, 0, 6, 7, 6, 0, -1},
    {10, 11, 7, 6, 10, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {6, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 0, 8, 10, 6, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 9, 10, 6, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 5, 9, 8, 4, 5, 10, 6, 3, -1, -1, -1, -1, -1, -1, -1},
    {6, 1, 4, 3, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {6, 0, 8, 6, 3, 0, 3, 1, 0, -1, -1, -1, -1, -1, -1, -1},
    {1, 6, 3, 1, 4, 6, 0, 5, 9, -1, -1, -1, -1, -1, -1, -1},
    {5, 3, 1, 5, 8, 3, 5, 9, 8, 8, 6, 3, -1, -1, -1, -1},
    {11, 5, 1, 3, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {5, 1, 11, 4, 0, 8, 3, 10, 6, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 1, 11, 9, 3, 10, 6, -1, -1, -1, -1, -1, -1, -1},
    {3, 10, 6, 1, 11, 4, 11, 8, 4, 11, 9, 8, -1, -1, -1, -1},
    {11, 6, 3, 11, 5, 6, 5, 4, 6, -1, -1, -1, -1, -1, -1, -1},
    {11, 6, 3, 5, 6, 11, 5, 8, 6, 5, 0, 8, -1, -1, -1, -1},
    {0, 4, 6, 0, 6, 11, 0, 11, 9, 3, 11, 6, -1, -1, -1, -1},
    {6, 3, 11, 6, 11, 8, 8, 11, 9, -1, -1, -1, -1, -1, -1, -1},
    {3, 8, 2, 10, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 3, 10, 4, 0, 3, 0, 2, 3, -1, -1, -1, -1, -1, -1, -1},
    {8, 3, 10, 8, 2, 3, 9, 0, 5, -1, -1, -1, -1, -1, -1, -1},
    {9, 2, 3, 9, 3, 4, 9, 4, 5, 10, 4, 3, -1, -1, -1, -1},
    {8, 1, 4, 8, 2, 1, 2, 3, 1, -1, -1, -1, -1, -1, -1, -1},
    {0, 2, 1, 2, 3, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {5, 9, 0, 1, 4, 2, 1, 2, 3, 2, 4, 8, -1, -1, -1, -1},
    {5, 9, 2, 5, 2, 1, 1, 2, 3, -1, -1, -1, -1, -1, -1, -1},
    {3, 8, 2, 3, 10, 8, 1, 11, 5, -1, -1, -1, -1, -1, -1, -1},
    {5, 1, 11, 4, 0, 10, 0, 3, 10, 0, 2, 3, -1, -1, -1, -1},
    {2, 10, 8, 2, 3, 10, 0, 1, 9, 1, 11, 9, -1, -1, -1, -1},
    {11, 9, 4, 11, 4, 1, 9, 2, 4, 10, 4, 3, 2, 3, 4, -1},
    {8, 5, 4, 8, 3, 5, 8, 2, 3, 3, 11, 5, -1, -1, -1, -1},
    {11, 5, 0, 11, 0, 3, 3, 0, 2, -1, -1, -1, -1, -1, -1, -1},
    {2, 3, 4, 2, 4, 8, 3, 11, 4, 0, 4, 9, 11, 9, 4, -1},
    {11, 9, 2, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 9, 7, 6, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 4, 2, 9, 7, 10, 6, 3, -1, -1, -1, -1, -1, -1, -1},
    {7, 0, 5, 7, 2, 0, 6, 3, 10, -1, -1, -1, -1, -1, -1, -1},
    {10, 6, 3, 8, 4, 2, 4, 7, 2, 4, 5, 7, -1, -1, -1, -1},
    {6, 1, 4, 6, 3, 1, 7, 2, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 7, 2, 0, 8, 3, 0, 3, 1, 3, 8, 6, -1, -1, -1, -1},
    {4, 3, 1, 4, 6, 3, 5, 7, 0, 7, 2, 0, -1, -1, -1, -1},
    {3, 1, 8, 3, 8, 6, 1, 5, 8, 2, 8, 7, 5, 7, 8, -1},
    {9, 7, 2, 11, 5, 1, 6, 3, 10, -1, -1, -1, -1, -1, -1, -1},
    {3, 10, 6, 5, 1, 11, 0, 8, 4, 2, 9, 7, -1, -1, -1, -1},
    {6, 3, 10, 7, 2, 11, 2, 1, 11, 2, 0, 1, -1, -1, -1, -1},
    {4, 2, 8, 4, 7, 2, 4, 1, 7, 11, 7, 1, 10, 6, 3, -1},
    {9, 7, 2, 11, 5, 3, 5, 6, 3, 5, 4, 6, -1, -1, -1, -1},
    {5, 3, 11, 5, 6, 3, 5, 0, 6, 8, 6, 0, 9, 7, 2, -1},
    {2, 0, 11, 2, 11, 7, 0, 4, 11, 3, 11, 6, 4, 6, 11, -1},
    {6, 3, 11, 6, 11, 8, 7, 2, 11, 2, 8, 11, -1, -1, -1, -1},
    {3, 9, 7, 3, 10, 9, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
    {4, 3, 10, 0, 3, 4, 0, 7, 3, 0, 9, 7, -1, -1, -1, -1},
    {0, 10, 8, 0, 7, 10, 0, 5, 7, 7, 3, 10, -1, -1, -1, -1},
    {3, 10, 4, 3, 4, 7, 7, 4, 5, -1, -1, -1, -1, -1, -1, -1},
    {7, 8, 9, 7, 1, 8, 7, 3, 1, 4, 8, 1, -1, -1, -1, -1},
    {9, 7, 3, 9, 3, 0, 0, 3, 1, -1, -1, -1, -1, -1, -1, -1},
    {5, 7, 8, 5, 8, 0, 7, 3, 8, 4, 8, 1, 3, 1, 8, -1},
    {5, 7, 3, 1, 5, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {5, 1, 11, 9, 7, 10, 9, 10, 8, 10, 7, 3, -1, -1, -1, -1},
    {0, 10, 4, 0, 3, 10, 0, 9, 3, 7, 3, 9, 5, 1, 11, -1},
    {10, 8, 7, 10, 7, 3, 8, 0, 7, 11, 7, 1, 0, 1, 7, -1},
    {3, 10, 4, 3, 4, 7, 1, 11, 4, 11, 7, 4, -1, -1, -1, -1},
    {5, 4, 3, 5, 3, 11, 4, 8, 3, 7, 3, 9, 8, 9, 3, -1},
    {11, 5, 0, 11, 0, 3, 9, 7, 0, 7, 3, 0, -1, -1, -1, -1},
    {0, 4, 8, 7, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 7, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {11, 3, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 4, 7, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 5, 7, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {5, 8, 4, 5, 9, 8, 7, 11, 3, -1, -1, -1, -1, -1, -1, -1},
    {1, 4, 10, 11, 3, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 0, 8, 10, 1, 0, 11, 3, 7, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 9, 1, 4, 10, 7, 11, 3, -1, -1, -1, -1, -1, -1, -1},
    {7, 11, 3, 5, 9, 1, 9, 10, 1, 9, 8, 10, -1, -1, -1, -1},
    {5, 3, 7, 1, 3, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {5, 3, 7, 5, 1, 3, 4, 0, 8, -1, -1, -1, -1, -1, -1, -1},
    {9, 3, 7, 9, 0, 3, 0, 1, 3, -1, -1, -1, -1, -1, -1, -1},
    {7, 9, 8, 7, 8, 1, 7, 1, 3, 4, 1, 8, -1, -1, -1, -1},
    {3, 4, 10, 3, 7, 4, 7, 5, 4, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 10, 0, 10, 7, 0, 7, 5, 7, 10, 3, -1, -1, -1, -1},
    {4, 10, 3, 0, 4, 3, 0, 3, 7, 0, 7, 9, -1, -1, -1, -1},
    {3, 7, 9, 3, 9, 10, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
    {7, 11, 3, 2, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 4, 0, 2, 6, 4, 3, 7, 11, -1, -1, -1, -1, -1, -1, -1},
    {5, 9, 0, 7, 11, 3, 8, 2, 6, -1, -1, -1, -1, -1, -1, -1},
    {11, 3, 7, 5, 9, 6, 5, 6, 4, 6, 9, 2, -1, -1, -1, -1},
    {4, 10, 1, 6, 8, 2, 11, 3, 7, -1, -1, -1, -1, -1, -1, -1},
    {7, 11, 3, 2, 6, 1, 2, 1, 0, 1, 6, 10, -1, -1, -1, -1},
    {0, 5, 9, 2, 6, 8, 1, 4, 10, 7, 11, 3, -1, -1, -1, -1},
    {9, 1, 5, 9, 10, 1, 9, 2, 10, 6, 10, 2, 7, 11, 3, -1},
    {3, 5, 1, 3, 7, 5, 2, 6, 8, -1, -1, -1, -1, -1, -1, -1},
    {5, 1, 7, 7, 1, 3, 4, 0, 2, 4, 2, 6, -1, -1, -1, -1},
    {8, 2, 6, 9, 0, 7, 0, 3, 7, 0, 1, 3, -1, -1, -1, -1},
    {6, 4, 9, 6, 9, 2, 4, 1, 9, 7, 9, 3, 1, 3, 9, -1},
    {8, 2, 6, 4, 10, 7, 4, 7, 5, 7, 10, 3, -1, -1, -1, -1},
    {7, 5, 10, 7, 10, 3, 5, 0, 10, 6, 10, 2, 0, 2, 10, -1},
    {0, 7, 9, 0, 3, 7, 0, 4, 3, 10, 3, 4, 8, 2, 6, -1},
    {3, 7, 9, 3, 9, 10, 2, 6, 9, 6, 10, 9, -1, -1, -1, -1},
    {11, 2, 9, 3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 11, 3, 2, 9, 11, 0, 8, 4, -1, -1, -1, -1, -1, -1, -1},
    {11, 0, 5, 11, 3, 0, 3, 2, 0, -1, -1, -1, -1, -1, -1, -1},
    {8, 4, 5, 8, 5, 3, 8, 3, 2, 3, 5, 11, -1, -1, -1, -1},
    {11, 2, 9, 11, 3, 2, 10, 1, 4, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 1, 1, 8, 10, 2, 9, 11, 2, 11, 3, -1, -1, -1, -1},
    {4, 10, 1, 0, 5, 3, 0, 3, 2, 3, 5, 11, -1, -1, -1, -1},
    {3, 2, 5, 3, 5, 11, 2, 8, 5, 1, 5, 10, 8, 10, 5, -1},
    {5, 2, 9, 5, 1, 2, 1, 3, 2, -1, -1, -1, -1, -1, -1, -1},
    {4, 0, 8, 5, 1, 9, 1, 2, 9, 1, 3, 2, -1, -1, -1, -1},
    {0, 1, 2, 2, 1, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 4, 1, 8, 1, 2, 2, 1, 3, -1, -1, -1, -1, -1, -1, -1},
    {9, 3, 2, 9, 4, 3, 9, 5, 4, 10, 3, 4, -1, -1, -1, -1},
    {8, 10, 5, 8, 5, 0, 10, 3, 5, 9, 5, 2, 3, 2, 5, -1},
    {4, 10, 3, 4, 3, 0, 0, 3, 2, -1, -1, -1, -1, -1, -1, -1},
    {3, 2, 8, 10, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {6, 11, 3, 6, 8, 11, 8, 9, 11, -1, -1, -1, -1, -1, -1, -1},
    {0, 6, 4, 0, 11, 6, 0, 9, 11, 3, 6, 11, -1, -1, -1, -1},
    {11, 3, 6, 5, 11, 6, 5, 6, 8, 5, 8, 0, -1, -1, -1, -1},
    {11, 3, 6, 11, 6, 5, 5, 6, 4, -1, -1, -1, -1, -1, -1, -1},
    {1, 4, 10, 11, 3, 8, 11, 8, 9, 8, 3, 6, -1, -1, -1, -1},
    {1, 0, 6, 1, 6, 10, 0, 9, 6, 3, 6, 11, 9, 11, 6, -1},
    {5, 8, 0, 5, 6, 8, 5, 11, 6, 3, 6, 11, 1, 4, 10, -1},
    {10, 1, 5, 10, 5, 6, 11, 3, 5, 3, 6, 5, -1, -1, -1, -1},
    {5, 1, 3, 5, 3, 8, 5, 8, 9, 8, 3, 6, -1, -1, -1, -1},
    {1, 3, 9, 1, 9, 5, 3, 6, 9, 0, 9, 4, 6, 4, 9, -1},
    {6, 8, 0, 6, 0, 3, 3, 0, 1, -1, -1, -1, -1, -1, -1, -1},
    {6, 4, 1, 3, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 9, 3, 8, 3, 6, 9, 5, 3, 10, 3, 4, 5, 4, 3, -1},
    {0, 9, 5, 10, 3, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {6, 8, 0, 6, 0, 3, 4, 10, 0, 10, 3, 0, -1, -1, -1, -1},
    {6, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 7, 11, 6, 7, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {10, 7, 11, 10, 6, 7, 8, 4, 0, -1, -1, -1, -1, -1, -1, -1},
    {7, 10, 6, 7, 11, 10, 5, 9, 0, -1, -1, -1, -1, -1, -1, -1},
    {11, 6, 7, 11, 10, 6, 9, 8, 5, 8, 4, 5, -1, -1, -1, -1},
    {1, 7, 11, 1, 4, 7, 4, 6, 7, -1, -1, -1, -1, -1, -1, -1},
    {8, 1, 0, 8, 7, 1, 8, 6, 7, 11, 1, 7, -1, -1, -1, -1},
    {9, 0, 5, 7, 11, 4, 7, 4, 6, 4, 11, 1, -1, -1, -1, -1},
    {9, 8, 1, 9, 1, 5, 8, 6, 1, 11, 1, 7, 6, 7, 1, -1},
    {10, 5, 1, 10, 6, 5, 6, 7, 5, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 4, 5, 1, 6, 5, 6, 7, 6, 1, 10, -1, -1, -1, -1},
    {9, 6, 7, 9, 1, 6, 9, 0, 1, 1, 10, 6, -1, -1, -1, -1},
    {6, 7, 1, 6, 1, 10, 7, 9, 1, 4, 1, 8, 9, 8, 1, -1},
    {5, 4, 7, 4, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 6, 0, 6, 5, 5, 6, 7, -1, -1, -1, -1, -1, -1, -1},
    {9, 0, 4, 9, 4, 7, 7, 4, 6, -1, -1, -1, -1, -1, -1, -1},
    {9, 8, 6, 7, 9, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {7, 8, 2, 7, 11, 8, 11, 10, 8, -1, -1, -1, -1, -1, -1, -1},
    {7, 0, 2, 7, 10, 0, 7, 11, 10, 10, 4, 0, -1, -1, -1, -1},
    {0, 5, 9, 8, 2, 11, 8, 11, 10, 11, 2, 7, -1, -1, -1, -1},
    {11, 10, 2, 11, 2, 7, 10, 4, 2, 9, 2, 5, 4, 5, 2, -1},
    {1, 7, 11, 4, 7, 1, 4, 2, 7, 4, 8, 2, -1, -1, -1, -1},
    {7, 11, 1, 7, 1, 2, 2, 1, 0, -1, -1, -1, -1, -1, -1, -1},
    {4, 11, 1, 4, 7, 11, 4, 8, 7, 2, 7, 8, 0, 5, 9, -1},
    {7, 11, 1, 7, 1, 2, 5, 9, 1, 9, 2, 1, -1, -1, -1, -1},
    {1, 7, 5, 1, 8, 7, 1, 10, 8, 2, 7, 8, -1, -1, -1, -1},
    {0, 2, 10, 0, 10, 4, 2, 7, 10, 1, 10, 5, 7, 5, 10, -1},
    {0, 1, 7, 0, 7, 9, 1, 10, 7, 2, 7, 8, 10, 8, 7, -1},
    {9, 2, 7, 1, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 2, 7, 8, 7, 4, 4, 7, 5, -1, -1, -1, -1, -1, -1, -1},
    {0, 2, 7, 5, 0, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {8, 2, 7, 8, 7, 4, 9, 0, 7, 0, 4, 7, -1, -1, -1, -1},
    {9, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 10, 6, 2, 9, 10, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 4, 2, 9, 6, 9, 10, 6, 9, 11, 10, -1, -1, -1, -1},
    {5, 11, 10, 5, 10, 2, 5, 2, 0, 6, 2, 10, -1, -1, -1, -1},
    {4, 5, 2, 4, 2, 8, 5, 11, 2, 6, 2, 10, 11, 10, 2, -1},
    {1, 9, 11, 1, 6, 9, 1, 4, 6, 6, 2, 9, -1, -1, -1, -1},
    {9, 11, 6, 9, 6, 2, 11, 1, 6, 8, 6, 0, 1, 0, 6, -1},
    {4, 6, 11, 4, 11, 1, 6, 2, 11, 5, 11, 0, 2, 0, 11, -1},
    {5, 11, 1, 8, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 10, 6, 9, 10, 2, 9, 1, 10, 9, 5, 1, -1, -1, -1, -1},
    {9, 6, 2, 9, 10, 6, 9, 5, 10, 1, 10, 5, 0, 8, 4, -1},
    {10, 6, 2, 10, 2, 1, 1, 2, 0, -1, -1, -1, -1, -1, -1, -1},
    {10, 6, 2, 10, 2, 1, 8, 4, 2, 4, 1, 2, -1, -1, -1, -1},
    {2, 9, 5, 2, 5, 6, 6, 5, 4, -1, -1, -1, -1, -1, -1, -1},
    {2, 9, 5, 2, 5, 6, 0, 8, 5, 8, 6, 5, -1, -1, -1, -1},
    {2, 0, 4, 6, 2, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 11, 8, 11, 10, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 0, 9, 4, 9, 10, 10, 9, 11, -1, -1, -1, -1, -1, -1, -1},
    {0, 5, 11, 0, 11, 8, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
    {4, 5, 11, 10, 4, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 4, 8, 1, 8, 11, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
    {9, 11, 1, 0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 4, 8, 1, 8, 11, 0, 5, 8, 5, 11, 8, -1, -1, -1, -1},
    {5, 11, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {5, 1, 10, 5, 10, 9, 9, 10, 8, -1, -1, -1, -1, -1, -1, -1},
    {4, 0, 9, 4, 9, 10, 5, 1, 9, 1, 10, 9, -1, -1, -1, -1},
    {0, 1, 10, 8, 0, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {4, 1, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {5, 4, 8, 9, 5, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 9, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 4, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};
