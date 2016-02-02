#ifndef RENDERLIB_UNIFOMHGRID_H
#define RENDERLIB_UNIFOMHGRID_H
#include <stdint.h>
#include <vector>
#include "allmath.h"
#include "UniformGrid.h"


namespace renderlib {
  class TriangleMesh;
  
	class UniformHGrid
	{
	public:
		UniformHGrid();
    ~UniformHGrid();
		
		UniformHGrid(uint32_t n, glm::vec3 origin);
		void storeTriangleMesh(TriangleMesh& triMesh);
		
    std::vector<uint32_t> getTrianglesNearPosition(const glm::vec3& pos) const;
    
		private:
		void operator=(const UniformHGrid&) = delete;
		UniformHGrid(const UniformHGrid&) = delete;

		protected:
		std::vector<UniformGrid*> _grids;
		int _numGrids;
		

	};

}
#endif // MESH_H
