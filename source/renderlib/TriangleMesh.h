#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H
#include "OpenGLHelper.h"
#include "shader.h"
#include <vector>
#include "vertex.h"
#include "renderlib/Texture.h"
#include "UniformGrid.h"

namespace renderlib {

	class TriangleMeshTriangle 
	{ 
	  public: 
		  glm::vec3 p0; 
		  glm::vec3 p1; 
		  glm::vec3 p2; 
		  glm::vec3 min; 
		  glm::vec3 max; 
	};

	class TriangleMesh
	{
	public:
		//Methods
	public:
		TriangleMesh();
		uint32_t  getTriangleCount() { return (uint32_t)_triangles.size(); };
		std::vector<TriangleMeshTriangle>& getTriangleVector() { return _triangles; }

		void clearArrays()
		{
			_triangles.clear();
		}

		void addTriangle(glm::vec3 p0,glm::vec3 p1,glm::vec3 p2)
		{
			TriangleMeshTriangle t;
			t.p0 = p0;
			t.p1 = p1;
			t.p2 = p2;

			t.min.x = t.min.y = t.min.z = 99999999.0f;
			t.max.x = t.max.y = t.max.z = -99999999.0f;
			std::vector<glm::vec3> verts{ t.p0, t.p1, t.p2 };
			for (glm::vec3 v : verts)
			{
				if (v.x < t.min.x)
					t.min.x = v.x;
				if (v.y < t.min.y)
					t.min.y = v.y;
				if (v.z < t.min.z)
					t.min.z = v.z;

				if (v.x > t.max.x)
					t.max.x = v.x;
				if (v.y > t.max.y)
					t.max.y = v.y;
				if (v.z > t.max.z)
					t.max.z = v.z;
			}
			_triangles.push_back(t);
		}

    void calculateBoundingBox(vec3& min, vec3& max);

	float getClosestPoint(UniformGrid& grid, glm::vec3 p, glm::vec3& closestPoint,
		glm::vec3& closestNormal);
    /*
	//TODO: These are copies from mesh.h
	//TODO: Move the following into a GEOMETRY class as static members.  
    glm::vec3 closestPointOnTriangle(const glm::vec3& p,
                                           const glm::vec3& a,
                                           const glm::vec3& b,
                                           const glm::vec3& c);

	void interpolateNormal(glm::vec3 const& n0, glm::vec3 const& n1, glm::vec3 const& n2,
		glm::vec3 const& barycentricCoord, glm::vec3& interpolatedNormal);

	float getSignOfDistanceToPoint(const glm::vec3 p,
		const glm::vec3 a,
		const glm::vec3 b,
		const glm::vec3 c);

	float distancePointTriangleExact(
		glm::vec3 const& point, glm::vec3 const& a, glm::vec3 const& b, glm::vec3 const& c,
		glm::vec3& closestPoint, glm::vec3& barycentricCoords);
     */


	private:
		DISALLOW_COPY_AND_ASSIGN(TriangleMesh);

	protected:
		std::vector<TriangleMeshTriangle> _triangles;
		glm::vec3 _min;
		glm::vec3 _max;

	};
  

}
#endif // MESH_H
