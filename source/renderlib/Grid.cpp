#include <cstring>
#include "OpenGLHelper.h"
#include "Grid.h"

namespace renderlib
{
  ///////////////////////////////////////////////////////////////////////////////
  // Grid Class Methods
  ///////////////////////////////////////////////////////////////////////////////
  Grid::Grid()
  {
  }


  Grid::Grid(int n, glm::vec3 origin){
    _n = n;
    _gridOrigin = origin;
    _cellSize = 1.0f/n;
    
    for(int z = 0; z < _n; ++z){
      for(int y = 0; y < _n; ++y){
        for(int x = 0; x < _n; ++x){
          glm::vec3 pos(x*_cellSize,y*_cellSize,z*_cellSize);
          
          //*****
          //The loop nesting matters because we are adding sequentially to
          //the grid
          //******
          glm::ivec3 indices(x,y,z);
          GridCell gc(pos + _gridOrigin, indices,IX(x,y,z));
          _gridCells.push_back(gc);
        }
      }
    }
  }
  
  void Grid::storeTriangleMesh(TriangleMesh& triMesh)
  {
    uint32_t i = 0;
    std::vector<TriangleMesh::Triangle>& tris = triMesh.getTriangleVector();
    for(TriangleMesh::Triangle& tri : tris)
    {
      storeTriangle(tri, i);
      i++;
    }
    
  }
  
  void Grid::storeTriangle(TriangleMesh::Triangle& tri, uint32_t i)
  {
    //FIXME: TRANSFORM TRIANGLE COORDS INTO LOCAL GRID COORDS.
    //RIGHT NOW I AM ASSUMING THE GRID IS AT THE ORIGIN
    
    //Find the range of cells that the bounding box of this tri covers and
    //add it to those cells
    glm::vec3 extents = tri.max - tri.min;
    glm::ivec3 cellCounts(
                          ceil(extents.x/_cellSize),
                          ceil(extents.y/_cellSize),
                          ceil(extents.z/_cellSize)
                          );
    //FIXME: ASSUMING that triangles are within range[0,1]
    glm::ivec3 originIDX(
                         floor(tri.min.x/_cellSize),
                         floor(tri.min.y/_cellSize),
                         floor(tri.min.z/_cellSize)
                         );
    //It should now be possible to get all of the overlapping cells
    //By incrementing the indices from the origin indices.
  }

  uint32_t Grid::getIndexFromPos(glm::vec3 pos)
  {
    glm::vec3 localPos = pos - _gridOrigin;
	   float scale = 1.0f/_cellSize;
    localPos *= scale;
    int i = (int)(localPos.x);
    int j = (int)(localPos.y);
    int k = (int)(localPos.z);
    
    
    if(i < 0 || i >= _n ||
       j < 0 || j >= _n ||
       k < 0 || k >= _n
       )
      return 0;
    
    return IX(i,j,k);
  }
  
  glm::ivec3 Grid::getIndicesFromPos(glm::vec3 pos)
  {
    glm::vec3 localPos = pos - _gridOrigin;
	   float scale = 1.0f/_cellSize;
    localPos *= scale;
    int i = (int)(localPos.x);
    int j = (int)(localPos.y);
    int k = (int)(localPos.z);
    
    return glm::ivec3(i,j,k);
    
  }
  
} //namespace renderlib

