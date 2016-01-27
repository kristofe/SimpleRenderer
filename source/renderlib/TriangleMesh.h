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
		struct Triangle { glm::vec3 p0; glm::vec3 p1; glm::vec3 p2; };
		//Methods
	public:
		TriangleMesh();
		uint32_t  getTriangleCount() { return (uint32_t)_triangles.size(); };
		std::vector<glm::vec3>& getPositionVector() { return _positions; }
		std::vector<Triangle>& getTriangleVector() { return _triangles; }

		void clearArrays()
		{
			_positions.clear();
			_triangles.clear();
		}

		void addPosition(glm::vec3 p)
		{
			_positions.push_back(p);
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
