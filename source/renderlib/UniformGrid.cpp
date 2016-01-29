#include <cstring>
#include "OpenGLHelper.h"
#include "TriangleMesh.h"
#include "UniformGrid.h"

namespace renderlib
{
  ///////////////////////////////////////////////////////////////////////////////
  // UniformGrid Class Methods
  ///////////////////////////////////////////////////////////////////////////////
  UniformGrid::UniformGrid()
  {
  }


  UniformGrid::UniformGrid(uint32_t n, glm::vec3 origin){
    _n = n;
    _gridOrigin = origin;
    _cellSize = 1.0f/n;
    
	size_t cellCount = n*n*n;
	_gridCells.reserve(cellCount);

    for(int z = 0; z < _n; ++z){
      for(int y = 0; y < _n; ++y){
        for(int x = 0; x < _n; ++x){
          glm::vec3 pos(x*_cellSize,y*_cellSize,z*_cellSize);
          
          //*****
          //The loop nesting matters because we are adding sequentially to
          //the grid
          //******
          glm::ivec3 indices(x,y,z);
          UniformGridCell gc(pos + _gridOrigin, indices,IX(x,y,z));
          _gridCells.push_back(gc);
        }
      }
    }
  }
  
  void UniformGrid::storeTriangleMesh(TriangleMesh& triMesh)
  {
    uint32_t triangleIDX = 0;
    std::vector<TriangleMeshTriangle>& tris = triMesh.getTriangleVector();
    for(TriangleMeshTriangle& tri : tris)
    {
      //storeTriangle(tri, i);
	  //FIXME: TRANSFORM TRIANGLE COORDS INTO LOCAL GRID COORDS.
	  //RIGHT NOW I AM ASSUMING THE GRID IS AT THE ORIGIN

	  //FIXME: ASSUMING that triangles are within range[0,1]
	  //Otherwise we have to subtract _gridOrigin from each vertex and scale them
		assert(tri.min.x >= 0.0f && tri.min.y >= 0.0f && tri.min.z >= 0.0f);
		assert(tri.max.x <= 1.0f && tri.max.y <= 1.0f && tri.max.z <= 1.0f);

		//Find the range of cells that the bounding box of this tri covers and
		//add it to those cells
		glm::vec3 extents = tri.max - tri.min;
		glm::ivec3 cellCounts(
			ceil(extents.x / _cellSize),
			ceil(extents.y / _cellSize),
			ceil(extents.z / _cellSize)
			);
		assert(cellCounts.x <= _n);
		assert(cellCounts.y <= _n);
		assert(cellCounts.z <= _n);


		glm::ivec3 originIDX(
			floor(tri.min.x / _cellSize),
			floor(tri.min.y / _cellSize),
			floor(tri.min.z / _cellSize)
			);

		//Assertions are just for debugging now
		assert(originIDX.x >= 0 && originIDX.x < _n);
		assert(originIDX.y >= 0 && originIDX.y < _n);
		assert(originIDX.z >= 0 && originIDX.z < _n);

		//Check that we won't go out of bounds
		glm::ivec3 maxIndices = originIDX + cellCounts;
		assert(maxIndices.x <= _n);
		assert(maxIndices.y <= _n);
		assert(maxIndices.z <= _n);

		//It should now be possible to get all of the overlapping cells
		//By incrementing the indices from the origin indices.
		for (int i = originIDX.x; i < originIDX.x + cellCounts.x; i++)
		{
			for (int j = originIDX.y; j < originIDX.y + cellCounts.y; j++)
			{
				for (int k = originIDX.z; k < originIDX.z + cellCounts.z; k++)
				{
					uint32_t idx = IX(i, j, k);
					UniformGridCell& gc = _gridCells[idx];
					gc.Add(triangleIDX);
				}
			}
		}
	  triangleIDX++;
    }
    
  }
  /*
  void UniformGrid::storeTriangle(TriangleMeshTriangle& tri, uint32_t triangleIDX)
  {
    //FIXME: TRANSFORM TRIANGLE COORDS INTO LOCAL GRID COORDS.
    //RIGHT NOW I AM ASSUMING THE GRID IS AT THE ORIGIN

    //FIXME: ASSUMING that triangles are within range[0,1]
	//Otherwise we have to subtract _gridOrigin from each vertex and scale them
	assert(tri.min.x >= 0.0f && tri.min.y >= 0.0f && tri.min.z >= 0.0f);
	assert(tri.max.x <= 1.0f && tri.max.y <= 1.0f && tri.max.z <= 1.0f);
    
    //Find the range of cells that the bounding box of this tri covers and
    //add it to those cells
    glm::vec3 extents = tri.max - tri.min;
    glm::ivec3 cellCounts(
                          ceil(extents.x/_cellSize),
                          ceil(extents.y/_cellSize),
                          ceil(extents.z/_cellSize)
                          );
	assert(cellCounts.x <= _n);
	assert(cellCounts.y <= _n);
	assert(cellCounts.z <= _n);


    glm::ivec3 originIDX(
                         floor(tri.min.x/_cellSize),
                         floor(tri.min.y/_cellSize),
                         floor(tri.min.z/_cellSize)
                         );

	//Assertions are just for debugging now
	assert(originIDX.x >= 0 && originIDX.x < _n);
	assert(originIDX.y >= 0 && originIDX.y < _n);
	assert(originIDX.z >= 0 && originIDX.z < _n);

	//Check that we won't go out of bounds
	glm::ivec3 maxIndices = originIDX + cellCounts;
	assert(maxIndices.x <= _n);
	assert(maxIndices.y <= _n);
	assert(maxIndices.z <= _n);

    //It should now be possible to get all of the overlapping cells
    //By incrementing the indices from the origin indices.
	for (int i = originIDX.x; i < originIDX.x + cellCounts.x; i++)
	{
		for (int j = originIDX.y; j < originIDX.y + cellCounts.y; j++)
		{
			for (int k = originIDX.z; k < originIDX.z + cellCounts.z; k++)
			{
				uint32_t idx = IX(i, j, k);
				UniformGridCell& gc = _gridCells[idx];
				gc.Add(triangleIDX);
			}
		}
	}
  }
  */
  

  uint32_t UniformGrid::getIndexFromPos(glm::vec3 pos)
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
  
  glm::ivec3 UniformGrid::getIndicesFromPos(glm::vec3 pos)
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

