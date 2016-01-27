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
			//TODO: Calc bbox
			/*
			_min.x = _min.y = _min.z = 99999999.0f;
			_max.x = _max.y = _max.z = -99999999.0f;
			for (Vertex v : _verts)
			{
				if (v.position.x < _min.x)
					_min.x = v.position.x;
				if (v.position.y < _min.y)
					_min.y = v.position.y;
				if (v.position.z < _min.z)
					_min.z = v.position.z;

				if (v.position.x > _max.x)
					_max.x = v.position.x;
				if (v.position.y > _max.y)
					_max.y = v.position.y;
				if (v.position.z > _max.z)
					_max.z = v.position.z;
			}
			*/

		}

		void calculateBoundingBox(vec3& min, vec3& max);

	private:
		DISALLOW_COPY_AND_ASSIGN(TriangleMesh);
		void getBoundingBox(vec3& min, vec3& max);

		//Data Members
	protected:

		std::vector<glm::vec3> _positions;
		std::vector<Triangle> _triangles;

		vec3 _min;
		vec3 _max;

	};

}
#endif // MESH_H
