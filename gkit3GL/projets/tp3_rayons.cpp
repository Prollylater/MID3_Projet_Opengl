
//! \file tuto_rayons.cpp

#include <vector>
#include <cfloat>
#include <chrono>

#include "vec.h"
#include "mat.h"
#include "color.h"
#include "image.h"
#include "image_io.h"
#include "image_hdr.h"
#include "orbiter.h"
#include "mesh.h"
#include "wavefront.h"
#include <random>

struct Ray
{
    Point o;    // origine
    Vector d;   // direction
    float tmax; // position de l'extremite, si elle existe. le rayon est un intervalle [0 tmax]

    // le rayon est un segment, on connait origine et extremite, et tmax= 1
    Ray(const Point &origine, const Point &extremite) : o(origine), d(Vector(origine, extremite)), tmax(1) {}

    // le rayon est une demi droite, on connait origine et direction, et tmax= \inf
    Ray(const Point &origine, const Vector &direction) : o(origine), d(direction), tmax(FLT_MAX) {}

    // renvoie le point sur le rayon pour t
    Point point(const float t) const { return o + t * d; }
};

/*
struct Bbox{

}


struct BVH{

}
*/
struct Hit
{
    float t;         // p(t)= o + td, position du point d'intersection sur le rayon
    float u, v;      // p(u, v), position du point d'intersection sur le triangle
    int triangle_id; // indice du triangle dans le mesh

    Hit() : t(FLT_MAX), u(), v(), triangle_id(-1) {}
    Hit(const float _t, const float _u, const float _v, const int _id) : t(_t), u(_u), v(_v), triangle_id(_id) {}

    // renvoie vrai si intersection
    operator bool() { return (triangle_id != -1); }
};

struct Triangle
{
    Point p;       // sommet a du triangle
    Vector e1, e2; // aretes ab, ac du triangle
    int id;        // indice du triangle
    Material material;

    Triangle(const TriangleData &data, const int _id) : p(data.a), e1(Vector(data.a, data.b)), e2(Vector(data.a, data.c)), id(_id) {}
    Triangle(const TriangleData &data, const int _id, Material mat)
        : p(data.a), e1(Vector(data.a, data.b)), e2(Vector(data.a, data.c)), id(_id), material(mat) {}

    /* calcule l'intersection ray/triangle
        cf "fast, minimum storage ray-triangle intersection"

        renvoie faux s'il n'y a pas d'intersection valide (une intersection peut exister mais peut ne pas se trouver dans l'intervalle [0 tmax] du rayon.)
        renvoie vrai + les coordonnees barycentriques (u, v) du point d'intersection + sa position le long du rayon (t).
        convention barycentrique : p(u, v)= (1 - u - v) * a + u * b + v * c
    */
    Hit intersect(const Ray &ray, const float tmax) const
    {
        Vector pvec = cross(ray.d, e2);
        float det = dot(e1, pvec);

        float inv_det = 1 / det;
        Vector tvec(p, ray.o);

        float u = dot(tvec, pvec) * inv_det;
        if (u < 0 || u > 1)
            return Hit(); // pas d'intersection

        Vector qvec = cross(tvec, e1);
        float v = dot(ray.d, qvec) * inv_det;
        if (v < 0 || u + v > 1)
            return Hit(); // pas d'intersection

        float t = dot(e2, qvec) * inv_det;
        if (t > tmax || t < 0)
            return Hit(); // pas d'intersection

        return Hit(t, u, v, id); // p(u, v)= (1 - u - v) * a + u * b + v * c
    }
};

// renvoie la normale au point d'intersection
Vector normal(const Mesh &mesh, const Hit &hit)
{
    // recuperer le triangle du mesh
    const TriangleData &data = mesh.triangle(hit.triangle_id);

    // interpoler la normale avec les coordonn�es barycentriques du point d'intersection
    float w = 1 - hit.u - hit.v;
    Vector n = w * Vector(data.na) + hit.u * Vector(data.nb) + hit.v * Vector(data.nc);
    return normalize(n);
}

struct Sampler
{
    std::default_random_engine rng;
    std::uniform_real_distribution<float> uniform;

    Sampler(const unsigned seed) : rng(seed), uniform(0, 1) {}

    float sample()
    {
        float u = uniform(rng);
        if (u >= 1)
            u = 0.99999994f;
        return u;
    }
};
struct Source
{
    // Source index in triangle scene
    int index;
    // Precomputed normal
    Vector normal;

    Source(const int &i, const Triangle &source) : index(i)
    {
        normal = cross(source.e1, source.e2);
    }
};

const double PI = 3.14159;
struct Scene
{
    std::vector<Triangle> triangles;
    std::vector<Source> sources;
    Orbiter camera;
    Transform inv;
};
// https://perso.univ-lyon1.fr/jean-claude.iehl/Public/educ/M1IMAGE/html/group__precision.html
float epsilon_point(const Point &p)
{
    // plus grande erreur
    float pmax = std::max(std::abs(p.x), std::max(std::abs(p.y), std::abs(p.z)));
    // evalue l'epsilon relatif du point d'intersection
    float pe = pmax * std::numeric_limits<float>::epsilon();
    return pe * 32;
}
// Hit and look for a collision, don't necessaryily search for their relf==flected light
Hit hit_in_scene(const Point &orig, const Vector &dir, const Scene &scene)
{
    Ray ray(orig, dir);

    // calculer les intersections avec tous les triangles
    for (unsigned i = 0; i < scene.triangles.size(); i++)
    {
        if (Hit h = scene.triangles[i].intersect(ray, ray.tmax))
        {
            // Forward the first intersection
            if (h.t > 0)
            {
                return h;
            }
        }
    }

    return {};
}

Hit hit_in_scene_sources(const Point &orig, const Scene &scene, int id_sources)
{

    const Triangle &curr_source = scene.triangles[scene.sources[id_sources].index];
    Ray ray(orig, curr_source.p);

    Hit hit;
    float tmax = ray.tmax;
    /*
    if (Hit h = curr_source.intersect(ray, ray.tmax))
    {
        if (h.t > 0)
        {
            return h;
        }
    }*/
    for (unsigned i = 0; i < scene.triangles.size(); i++)
    {
        if (Hit h = scene.triangles[i].intersect(ray, tmax))
        {
            // ne conserve que l'intersection *valide* la plus proche de l'origine du rayon
            assert(h.t > 0);
            hit = h;
            tmax = h.t;
        }
    }

    return hit;
}
// Create a Scene class that will hold  std::vector<Triangle> triangles;

Color montecarlo_estimator(const Vector &normal, int N, const Ray &ray, const Hit &hit, const Scene &scene)
{

    Color color;

    for (int i = 0; i < N; i++)
    {
        std::random_device hwseed;
        Sampler rng(hwseed());
        // genere u1 et u2
        float u1 = rng.sample(); // Cos value
        float u2 = rng.sample(); // sin  value
        float phi = 2 * PI * u2; // Cos value
        // construit la direction l et évalzzzue sa pdf
        Vector l = normalize(Vector(std::cos(phi) * std::sqrt(1 - u1 * u1), u1,
                                    std::sin(phi) * std::sqrt(1 - u1 * u1)));
        float pdf = 1 / (2 * PI);

        // evaluation de la fonction
        // Position of the triangle
        Point origin = ray.o + (hit.t * ray.d);
        origin = origin + epsilon_point(origin) * normal;
        float V = hit_in_scene(origin, l, scene) ? 0 : 1; //> 0 ? 1: 0; // 0 ou 1, selon les intersections

        float cos_theta = dot(normal, l);
        Color emission = l.y > 0 ? Color(1) : Black();

        // cos_theta = std::max(float(0),cos_theta);

        // moyenne
        color = color + scene.triangles[hit.triangle_id].material.diffuse * (emission * V * cos_theta / pdf);
    }
    color = color / float(N);
    ;
    return color;
}

Color montecarlo_estimator_src(const Mesh &mesh, int N, const Ray &ray, const Hit &hit, const Scene &scene, Sampler &rng)
{

    Color color;
    Vector norm = normal(mesh, hit);
    // Position du triangle
    Point origin = ray.o + (hit.t * ray.d);
    origin = origin + epsilon_point(origin) * norm;
    float V = 1;

    for (int i = 0; i < N; i++)
    {

        // genere u1 et u2
        int rng_src = rng.sample() * scene.sources.size();
        const Source &source_data = scene.sources[rng_src];

        float u1 = rng.sample() / 2; // Cos value
        float u2 = rng.sample() / 2; // sin  value                                      // sin  value
        // Point d'interscetion et visibilité

        // BUG here
        // Hit point_hit = hit_in_scene_sources(origin, scene, source_data.index);
        // float V = (point_hit.triangle_id == source_data.index) ? 1 : 0;
        //  Point sur la spirce
        const Triangle &source = scene.triangles[source_data.index];
        Point q = source.p + u1 * source.e1 + u2 * source.e2;

        // Vector n will be calculated mutliple times
        const Vector l = Vector(origin, q);

        // Cosinus
        float cos_theta_p = dot(normalize(norm), normalize(l));
        cos_theta_p = std::max(float(0), cos_theta_p);
        float cos_theta_q = dot(normalize(source_data.normal), normalize(-l));
        cos_theta_q = std::max(float(0), cos_theta_q);
        cos_theta_q /= distance2(origin,q);

        // Assumming we consider a triangle
        // Recheck the opeation
        float pdf = 1 / ((length(source_data.normal) / 2) * (float)scene.sources.size());

        Color emission = source.material.emission;
        Color diffuse_color = scene.triangles[hit.triangle_id].material.diffuse / float(M_PI);
        Color diffuse_ligth = diffuse_color * emission * cos_theta_p * cos_theta_q/pdf;
        // Color specular_light = scene.triangles[hit.triangle_id].material.specular * ( cos_theta_p *emission * cos_theta_q / pdf);

        color = color + diffuse_ligth;
    }
    color = color / float(N);
    // Adding emission component
    return color;
}

int main(const int argc, const char **argv)
{
    Scene scene;
    const char *mesh_filename = "data/cornell.obj";
    if (argc > 1)
        mesh_filename = argv[1];

    const char *orbiter_filename = "data/cornell_orbiter.txt";
    if (argc > 2)
        orbiter_filename = argv[2];

    if (scene.camera.read_orbiter(orbiter_filename) < 0)
        return 1;

    Mesh mesh = read_mesh(mesh_filename);

    // recupere les triangles dans le mesh
    scene.triangles.reserve(mesh.triangle_count());
    {
        int n = mesh.triangle_count();
        for (int i = 0; i < n; i++)
        {
            scene.triangles.emplace_back(mesh.triangle(i), i,
                                         mesh.triangle_material(i));
            const Color &emission = mesh.triangle_material(i).emission;
            float val = (emission.r + emission.g + emission.b);
            if (val != 0)
            {
                scene.sources.emplace_back(i, scene.triangles[i]);
            }
        }
    }

    // Crééer une image
    Image image(1024, 768);

    // recupere les transformations pour generer les rayons
    scene.camera.projection(image.width(), image.height(), 45);
    Transform model = Identity();
    Transform view = scene.camera.view();
    Transform projection = scene.camera.projection();
    Transform viewport = scene.camera.viewport();
    Transform inv = Inverse(viewport * projection * view * model);
    scene.inv = inv;
    auto start = std::chrono::high_resolution_clock::now();

    // Génération de la direction et de la pdf
    std::random_device hwseed;
    Sampler rng(hwseed());
    // parcours tous les pixels de l'image
    for (int y = 0; y < image.height(); y++)
        for (int x = 0; x < image.width(); x++)
        {
            // generer le rayon au centre du pixel
            // On le passe dans le repère de projectif
            // Origine est plus extremité 1, et extremité plus extremité2
            Point origine = inv(Point(x + float(0.5), y + float(0.5), 0));
            Point extremite = inv(Point(x + float(0.5), y + float(0.5), 1));

            Ray ray(origine, extremite);

            // calculer les intersections avec tous les triangles
            Hit hit;               // proprietes de l'intersection
            float tmax = ray.tmax; // extremite du rayon
            for (int i = 0; i < int(scene.triangles.size()); i++)
            {
                if (Hit h = scene.triangles[i].intersect(ray, tmax))
                {
                    // ne conserve que l'intersection *valide* la plus proche de l'origine du rayon
                    assert(h.t > 0);
                    hit = h;
                    tmax = h.t;
                }
            }

            if (hit)
            {
                // normale interpolee a l'intersection
                image(x, y) = Color(montecarlo_estimator_src(mesh, 8, ray, hit, scene, rng), 1);
                // image(x, y) =Color(std::abs(n.x), std::abs(n.y), std::abs(n.z));
            }
        }

    auto stop = std::chrono::high_resolution_clock::now();
    int cpu = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
    printf("%dms\n", cpu);

    write_image(image, "render.png");
    write_image_hdr(image, "render.hdr");
    return 0;
}
