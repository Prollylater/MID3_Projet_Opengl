#include "bezier.h"
#include "vec.h"
#include "mat.h"
#include <random>

struct MeshDeform
{
	std::vector<Point> positions;
	std::vector<unsigned> indices;
	std::vector<Vector> normals;
};

typedef std::vector<std::vector<std::vector<Point>>> DeformBox;
struct FreeFormDeform
{
	DeformBox control_points;

	std::array<Vector, 3> repere;		 // S, T, U
	std::array<Vector, 3> repere_normal; // txu, sxu, sxy //s,t,u

	void createGrid(const Point &position, float step, float size_x, float size_y, float size_z);
	void createBoundingGrid(const std::vector<Point> &positions , int div);
	
	Point warpPoint(const Point &positions);
	void outputGrid(std::vector<Point> &vertices, std::vector<unsigned> &ffd_grid_indices);

	MeshDeform warpMesh(const std::vector<Point> &positions, const std::vector<unsigned> &indices, const std::vector<Vector> &normals);
	void randomModif();

	Vector getPointInParall(const Point &point);

};
namespace TwistDeform
{

	Point twistPoint(const Point &point, int axis, float T);
	Vector twistNormalZ(const Point &point, const Vector &normal, float T);
	MeshDeform twistMesh(const std::vector<Point> &positions, const std::vector<unsigned> &indices, const std::vector<Vector> &normals, int axis, float period);

};

namespace LocalDeformations
{

	// This namespace function simply handle translation
	namespace onSphere
	{

		float attenuation(const Point &point, const Point &center, float radius);

		Point translatePoint(const Point &pt, const Vector &translation, const Point &center, float radius);

		MeshDeform warpMesh(const std::vector<Point> &positions, const std::vector<unsigned> &indices, const std::vector<Vector> &normals,
							const Vector &translation, const Point &center, float radius);

	}
}

bool writeMeshDeform(const char *filename, const MeshDeform &mesh);
