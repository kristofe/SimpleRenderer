#ifndef RENDERLIB_UNIFORMGRID_H
#define RENDERLIB_UNIFORMGRID_H
#include <stdint.h>
#include <vector>
#include "allmath.h"
#include "TriangleMesh.h"


namespace renderlib {
  class UniformGridCell{
  public:
    std::vector<uint32_t> triangleIDs;
    glm::vec3 worldPosition;
    glm::ivec3 gridIndices;
    uint32_t idx;
    
    UniformGridCell():worldPosition(glm::vec3(0)), gridIndices(glm::ivec3(0)), idx(0)
    {
       
    }
    
    UniformGridCell(glm::vec3 worldPos, glm::ivec3 indices, uint32_t pid)
    :worldPosition(worldPos), gridIndices(indices), idx(pid)
    {
    }
    
    void Add(uint32_t triID)
    {
      triangleIDs.push_back(triID);
    }
    
    
    void Clear()
    {
      triangleIDs.clear();
    }
    
  };
	class UniformGrid
	{
 //Methods
	public:
		UniformGrid();
		
		UniformGrid(uint32_t n, glm::vec3 origin);
		void storeTriangleMesh(TriangleMesh& triMesh);
		//void storeTriangle(TriangleMeshTriangle& tri, uint32_t triIDX);
		
		uint32_t IX(int i, int j, int k){
		  return i + (j*_n) + (k*_n*_n);
		}
		
		uint32_t SAFE_IX(int i, int j, int k){
		  if(i < 0 || i >= _n ||
			 j < 0 || j >= _n ||
			 k < 0 || k >= _n
			 )
			return -1;
		  
		  return IX(i,j,k);
		}
		uint32_t getIndexFromPos(glm::vec3 pos);
		glm::ivec3 getIndicesFromPos(glm::vec3 pos);

		std::vector<uint32_t>& getTrianglesNearPosition(glm::vec3 const& pos)
		{
			uint32_t idx = getIndexFromPos(pos);
			return _gridCells[idx].triangleIDs;
		}

		float getCellRadius() { return _cellSize*0.5f; }
		
		private:
		void operator=(const UniformGrid&) = delete;
		UniformGrid(const UniformGrid&) = delete;

		protected:
		std::vector<UniformGridCell> _gridCells;
		glm::vec3 _gridOrigin;
		float _cellSize;
		int _n;
		

	};

}
#endif // MESH_H