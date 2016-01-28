#ifndef RENDERLIB_GRID_H
#define RENDERLIB_GRID_H
#include <stdint.h>
#include <vector>
#include "allmath.h"
#include "TriangleMesh.h"


namespace renderlib {
  class GridCell{
  public:
    std::vector<uint32_t> triangleIDs;
    glm::vec3 worldPosition;
    glm::ivec3 gridIndices;
    uint32_t idx;
    
    GridCell():worldPosition(glm::vec3(0)), gridIndices(glm::ivec3(0)), idx(0)
    {
       
    }
    
    GridCell(glm::vec3 worldPos, glm::ivec3 indices, uint32_t tid)
    :worldPosition(worldPos), gridIndices(indices), idx(tid)
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
	class Grid
	{
 //Methods
	public:
		Grid();
    
    Grid(int n, glm::vec3 origin);
    void storeTriangleMesh(TriangleMesh& triMesh);
    void storeTriangle(TriangleMesh::Triangle& tri, uint32_t i);
    
    int IX(int i, int j, int k){
      return i + (j*_n) + (k*_n*_n);
    }
    
    int SAFE_IX(int i, int j, int k){
      if(i < 0 || i >= _n ||
         j < 0 || j >= _n ||
         k < 0 || k >= _n
         )
        return -1;
      
      return IX(i,j,k);
    }
    
	private:
    void operator=(const Grid&) = delete;
    Grid(const Grid&) = delete;

	protected:
    std::vector<GridCell> _gridCells;
    glm::vec3 _gridOrigin;
    float _cellSize;
    int _n;
    

	};

}
#endif // MESH_H
