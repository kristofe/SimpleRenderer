#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H
#include "OpenGLHelper.h"
#include "shader.h"
#include <vector>
#include "vertex.h"
#include "renderlib/Texture.h"

namespace renderlib {

	class TriangleMesh
	{
	public:
		struct Triangle { glm::vec3 p0; glm::vec3 p1; glm::vec3 p2; glm::vec3 min; glm::vec3 max; };
		//Methods
	public:
		TriangleMesh();
		uint32_t  getTriangleCount() { return (uint32_t)_triangles.size(); };
		std::vector<Triangle>& getTriangleVector() { return _triangles; }

		void clearArrays()
		{
			_triangles.clear();
		}

		void addTriangle(glm::vec3 p0,glm::vec3 p1,glm::vec3 p2)
		{
			Triangle t;
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


	private:
		DISALLOW_COPY_AND_ASSIGN(TriangleMesh);

	protected:
		std::vector<Triangle> _triangles;


	};

}
#endif // MESH_H
