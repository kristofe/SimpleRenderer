#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H
#include "OpenGLHelper.h"
#include "shader.h"
#include <vector>
#include "vertex.h"
#include "renderlib/Texture.h"

namespace renderlib {

  class UniformHGrid;
  
	class TriangleMeshTriangle 
	{ 
	  public: 
		  glm::vec3 p0; 
		  glm::vec3 p1; 
		  glm::vec3 p2; 
		  glm::vec3 n0; 
		  glm::vec3 n1; 
		  glm::vec3 n2; 
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

		void addTriangle(
			glm::vec3 p0,glm::vec3 p1,glm::vec3 p2,
			glm::vec3 n0,glm::vec3 n1,glm::vec3 n2)
		{
			TriangleMeshTriangle t;
			t.p0 = p0;
			t.p1 = p1;
			t.p2 = p2;
			t.n0 = n0;
			t.n1 = n1;
			t.n2 = n2;

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
      _indices.push_back((uint32_t)_indices.size());
		}

    void calculateBoundingBox(vec3& min, vec3& max);

	float getClosestPoint(glm::vec3 p, glm::vec3& closestPoint,
                        glm::vec3& closestNormal) const;

    void setGrid(std::shared_ptr<UniformHGrid> grid){ _grid = grid;}

	private:
		DISALLOW_COPY_AND_ASSIGN(TriangleMesh);

	protected:
		std::vector<TriangleMeshTriangle> _triangles;
		glm::vec3 _min;
		glm::vec3 _max;
    std::shared_ptr<UniformHGrid> _grid;
    std::vector<uint32_t> _indices;

	};
  

}
#endif // MESH_H
