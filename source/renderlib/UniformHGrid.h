#ifndef RENDERLIB_UNIFOMHGRID_H
#define RENDERLIB_UNIFOMHGRID_H
#include <stdint.h>
#include <vector>
#include "allmath.h"
#include "TriangleMesh.h"


namespace renderlib {
	class UniformHGrid
	{
	public:
		UniformHGrid();
		
		UniformHGrid(uint32_t n, glm::vec3 origin);
		void storeTriangleMesh(TriangleMesh& triMesh);
		
    std::vector<uint32_t> getTrianglesNearPosition(const glm::vec3& pos);
    
		private:
		void operator=(const UniformHGrid&) = delete;
		UniformHGrid(const UniformHGrid&) = delete;

		protected:
		std::vector<UniformGrid&> _grids;
		int _numGrids;
		

	};

}
#endif // MESH_H
